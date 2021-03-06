// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include <stdio.h>
#include <string>

#include "Common/Common.h"
#include "Federation/Federation.h"

#include "Host.h"
#include "PyInterpreter.h"
#include "PyHostConfig.h"
#include "BroadcastMessageBody.h"

using namespace Common;
using namespace Federation;
using namespace std;
using namespace Transport;

using namespace PyHost;

StringLiteral const TraceComponent("Host");

string const PyCallback_OnInitialize = "OnInitialize";
string const PyCallback_OnNodeIdAcquired = "OnNodeIdAcquired";
string const PyCallback_OnNodeIdReleased = "OnNodeIdReleased";
string const PyCallback_OnBroadcast = "OnBroadcast";

string const PyQuery_SeedNodes = "seednodes";

//
// Impl
//

class Host::Impl
{
private:
    struct ModuleEntry;

public:
    Impl() 
        : pyInterpreter_()
        , fs_()
        , modules_()
        , lock_()
    {
        Register_PyCallbacks();
    }

public:

    //
    // DLL exported functions
    //

    ErrorCode Initialize(shared_ptr<FederationSubsystem> const & fs)
    {
        fs_ = fs;

        auto moduleNames = PyHostConfig::GetConfig().ModuleNames;

        if (moduleNames.empty())
        {
            Trace.WriteInfo(TraceComponent, "No Python modules to initialize");
            return ErrorCodeValue::Success;
        }

        vector<string> tokens;
        StringUtility::Split<string>(moduleNames, tokens, ",");

        for (auto const & moduleName : tokens)
        {
            if (moduleName.empty()) { continue; }

            Trace.WriteInfo(TraceComponent, "Initializing Python module '{0}' on node {1} ...", moduleName, fs->Id);

            vector<string> args;
            args.push_back(fs->IdString);
            auto error = pyInterpreter_.Execute(moduleName, PyCallback_OnInitialize, args);
            if (!error.IsSuccess()) { return error; } 
            
            AcquireWriteLock lock(lock_);

            auto findIter = modules_.find(moduleName);
            if (findIter == modules_.end())
            {
                Trace.WriteWarning(TraceComponent, "Python module '{0}' added to node {1} ...", moduleName, fs->Id);

                modules_.insert(make_pair(moduleName, nullptr));
            }
        }

        return ErrorCodeValue::Success;
    }

    ErrorCode OnRoutingTokenChanged()
    {
        ErrorCode finalError;

        AcquireWriteLock lock(lock_);

        for (auto const & module : modules_)
        {
            auto const & moduleName = module.first;
            auto const & entry = module.second;

            if (entry.get() == nullptr)
            {
                continue;
            }

            if (entry->IsOwned && !IsNodeIdOwned(entry->Id))
            {
                auto error = NotifyNodeIdReleased(moduleName, entry);
                if (error.IsSuccess())
                {
                    Trace.WriteInfo(TraceComponent, "NotifyNodeIdReleased({0}, {1}) succeeded", moduleName, entry->Id);
                }
                else
                {
                    Trace.WriteWarning(TraceComponent, "NotifyNodeIdReleased({0}, {1}) failed: {2}", moduleName, entry->Id, error);

                    finalError = ErrorCode::FirstError(finalError, error);
                }
            }
            else if (!entry->IsOwned && IsNodeIdOwned(entry->Id))
            {
                auto error = NotifyNodeIdAcquired(moduleName, entry);
                if (error.IsSuccess())
                {
                    Trace.WriteInfo(TraceComponent, "NotifyNodeIdAcquired({0}, {1}) succeeded", moduleName, entry->Id);
                }
                else
                {
                    Trace.WriteWarning(TraceComponent, "NotifyNodeIdAcquired({0}, {1}) failed: {2}", moduleName, entry->Id, error);

                    finalError = ErrorCode::FirstError(finalError, error);
                }
            }
        }

        return finalError;
    }

    ErrorCode OnBroadcast(MessageUPtr & msg)
    {
        ErrorCode finalError;

        auto contents = BroadcastMessageBody::DeserializeBody(msg).GetContents();

        AcquireReadLock lock(lock_);

        for (auto const & module : modules_)
        {
            vector<string> args;
            args.push_back(contents);

            auto error = pyInterpreter_.ExecuteIfFound(module.first, PyCallback_OnBroadcast, args);
            if (!error.IsSuccess())
            {
                finalError = ErrorCode::FirstError(finalError, error);
            }
        }

        return finalError;
    }

private:
    void Register_PyCallbacks()
    {
        pyInterpreter_.Register_SetNodeIdOwnership([this](string const & moduleName, string const & nodeId)
        {
            return SetNodeIdOwnership(moduleName, nodeId);
        });

        pyInterpreter_.Register_Broadcast([this](string const & contents)
        {
            return Broadcast(contents);
        });

        pyInterpreter_.Register_Query([this](string const & query, string & result)
        {
            return Query(query, result);
        });
    }

private:

    //
    // Python -> C++
    //

    ErrorCode SetNodeIdOwnership(string const & moduleName, string const & nodeIdString)
    {
        AcquireWriteLock lock(lock_);

        NodeId newNodeId;
        if (!NodeId::TryParse(nodeIdString, newNodeId))
        {
            auto msg = formatString("Failed to parse '{0}' as NodeId", nodeIdString);
            Trace.WriteWarning(TraceComponent, "{0}", msg);
            return ErrorCode(ErrorCodeValue::InvalidArgument, move(msg));
        }

        bool newEntry = false;
        shared_ptr<ModuleEntry> entry;

        auto findIter = modules_.find(moduleName);
        if (findIter == modules_.end())
        {
            newEntry = true;
            entry = make_shared<ModuleEntry>(newNodeId, false);
            modules_.insert(make_pair(moduleName, entry));
        }
        else
        {
            entry = findIter->second;
        }
        
        if (entry->IsOwned && !IsNodeIdOwned(entry->Id))
        {
            auto error = NotifyNodeIdReleased(moduleName, entry);
            if (!error.IsSuccess()) 
            { 
                // Trace error if release notification fails, but
                // continue to process acquire notification (if any).
                // i.e. Do not return error here.
                //
                Trace.WriteWarning(
                    TraceComponent, 
                    "failed to notify release of previous NodeId '{0}': {1}", 
                    entry->Id, 
                    error);
            }
        }

        if (!newEntry)
        {
            entry->Id = newNodeId;
        }

        if (!entry->IsOwned && IsNodeIdOwned(entry->Id))
        {
            return NotifyNodeIdAcquired(moduleName, entry);
        }
        else
        {
            return ErrorCodeValue::Success;
        }
    }

    ErrorCode Broadcast(string const & contents)
    {
        auto msg = BroadcastMessageBody::CreateMessage(contents);

        auto fs = LockFederationSubsystem();
        if (fs.get() == nullptr) { return ErrorCodeValue::ObjectClosed; }

        fs->Broadcast(move(msg));

        return ErrorCodeValue::Success;
    }

    ErrorCode Query(string const & query, string & result)
    {
        if (query == PyQuery_SeedNodes)
        {
            auto fs = LockFederationSubsystem();
            if (fs.get() == nullptr) { return ErrorCodeValue::ObjectClosed; }

            vector<NodeId> seedNodes;
            fs->GetSeedNodes(seedNodes);

            // Manually build JSON for now
            result = "{\"seednodes\":[";
            for (auto const & nodeId : seedNodes)
            {
                if (result.back() != '[')
                {
                    result.append(",");
                }
                result.append("\"");
                result.append(nodeId.ToString());
                result.append("\"");
            }
            result.append("]}");

            return ErrorCodeValue::Success;
        }
        else
        {
            auto msg = formatString("Invalid query '{0}'. Valid queries: ", query);
            msg.append(formatString("'{0}'", PyQuery_SeedNodes));

            Trace.WriteWarning(TraceComponent, "{0}", msg);

            return ErrorCode(ErrorCodeValue::InvalidArgument, move(msg));
        }
    }

private:

    bool IsNodeIdOwned(NodeId const & nodeId)
    {
        auto fs = LockFederationSubsystem();
        if (fs.get() == nullptr) { return false; }

        return fs->Token.getRange().Contains(nodeId);
    }

    ErrorCode NotifyNodeIdAcquired(string const & moduleName, shared_ptr<ModuleEntry> const & entry)
    {
        entry->IsOwned = true;

        vector<string> args;
        args.push_back(entry->Id.ToString());
        return pyInterpreter_.Execute(moduleName, PyCallback_OnNodeIdAcquired, args);
    }

    ErrorCode NotifyNodeIdReleased(string const & moduleName, shared_ptr<ModuleEntry> const & entry)
    {
        entry->IsOwned = false;

        vector<string> args;
        args.push_back(entry->Id.ToString());
        return pyInterpreter_.Execute(moduleName, PyCallback_OnNodeIdReleased, args);
    }

    shared_ptr<FederationSubsystem> LockFederationSubsystem()
    {
        auto fs = fs_.lock();
        if (fs.get() == nullptr)
        {
            Trace.WriteInfo(TraceComponent, "federation is closed");
        }

        return fs;
    }

private:

    struct ModuleEntry
    {
        ModuleEntry(NodeId const & id, bool isOwned)
            : Id(id)
            , IsOwned(isOwned)
        {
        }
            
        NodeId Id;
        bool IsOwned;
    };

    PyInterpreter pyInterpreter_;
    weak_ptr<FederationSubsystem> fs_;
    unordered_map<string, shared_ptr<ModuleEntry>> modules_;
    RwLock lock_;
};

//
// Host
//

Host::Host() : impl_(make_shared<Impl>())
{
}

ErrorCode Host::Initialize(shared_ptr<FederationSubsystem> const & fs)
{
    return impl_->Initialize(fs);
}

ErrorCode Host::OnRoutingTokenChanged()
{
    return impl_->OnRoutingTokenChanged();
}

ErrorCode Host::OnBroadcast(MessageUPtr & msg)
{
    return impl_->OnBroadcast(msg);
}

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

wstring const PyCallback_OnInitialize = L"OnInitialize";
wstring const PyCallback_OnNodeIdAcquired = L"OnNodeIdAcquired";
wstring const PyCallback_OnNodeIdReleased = L"OnNodeIdReleased";
wstring const PyCallback_OnBroadcast = L"OnBroadcast";

//
// Impl
//

class Host::Impl
{
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

        auto moduleName = PyHostConfig::GetConfig().ModuleName;

        if (moduleName.empty())
        {
            Trace.WriteInfo(TraceComponent, "No Python module to initialize");
            return ErrorCodeValue::Success;
        }
        else
        {
            Trace.WriteInfo(TraceComponent, "Initializing Python module '{0}' on node {1} ...", moduleName, fs->Id);

            vector<wstring> args;
            args.push_back(fs->IdString);
            auto error = pyInterpreter_.Execute(moduleName, PyCallback_OnInitialize, args);

            if (error.IsSuccess())
            {
                AcquireWriteLock lock(lock_);

                auto findIter = modules_.find(moduleName);
                if (findIter == modules_.end())
                {
                    Trace.WriteWarning(TraceComponent, "Python module '{0}' added to node {1} ...", moduleName, fs->Id);

                    modules_.insert(make_pair(moduleName, nullptr));
                }
            }

            return error;
        }
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
                auto error = NotifyNodeIdReleased(moduleName, entry->Id);
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
                auto error = NotifyNodeIdAcquired(moduleName, entry->Id);
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
            vector<wstring> args;
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
        pyInterpreter_.Register_SetNodeIdOwnership([this](wstring const & moduleName, wstring const & nodeId)
        {
            return SetNodeIdOwnership(moduleName, nodeId);
        });

        pyInterpreter_.Register_Broadcast([this](wstring const & contents)
        {
            return Broadcast(contents);
        });
    }

private:

    //
    // Python -> C++
    //

    ErrorCode SetNodeIdOwnership(wstring const & moduleName, wstring const & nodeIdString)
    {
        AcquireWriteLock lock(lock_);

        NodeId nodeId;
        if (!NodeId::TryParse(nodeIdString, nodeId))
        {
            auto msg = wformatString("Failed to parse '{0}' as NodeId", nodeIdString);
            Trace.WriteWarning(TraceComponent, "{0}", msg);
            return ErrorCode(ErrorCodeValue::InvalidArgument, move(msg));
        }

        shared_ptr<ModuleEntry> entry;
        auto findIter = modules_.find(moduleName);
        if (findIter == modules_.end())
        {
            entry = make_shared<ModuleEntry>(nodeId, false);
            modules_.insert(make_pair(moduleName, entry));
        }
        else
        {
            entry = findIter->second;
        }

        if (entry->IsOwned && !IsNodeIdOwned(nodeId))
        {
            return NotifyNodeIdReleased(moduleName, nodeId);
        }
        else if (!entry->IsOwned && IsNodeIdOwned(nodeId))
        {
            return NotifyNodeIdAcquired(moduleName, nodeId);
        }
        else
        {
            return ErrorCodeValue::Success;
        }
    }

    ErrorCode Broadcast(wstring const & contents)
    {
        auto msg = BroadcastMessageBody::CreateMessage(contents);

        auto fs = fs_.lock();

        if (fs.get() == nullptr)
        {
            Trace.WriteInfo(TraceComponent, "Broadcast: federation is closed");

            return ErrorCodeValue::ObjectClosed;
        }

        fs->Broadcast(move(msg));

        return ErrorCodeValue::Success;
    }

private:

    bool IsNodeIdOwned(NodeId const & nodeId)
    {
        auto fs = fs_.lock();
        if (fs.get() == nullptr)
        {
            Trace.WriteInfo(TraceComponent, "IsNodeIdOwned: federation is closed"); 

            return false;
        }

        return fs->Token.getRange().Contains(nodeId);
    }

    ErrorCode NotifyNodeIdAcquired(wstring const & moduleName, NodeId const & nodeId)
    {
        vector<wstring> args;
        args.push_back(nodeId.ToString());
        return pyInterpreter_.Execute(moduleName, PyCallback_OnNodeIdAcquired, args);
    }

    ErrorCode NotifyNodeIdReleased(wstring const & moduleName, NodeId const & nodeId)
    {
        vector<wstring> args;
        args.push_back(nodeId.ToString());
        return pyInterpreter_.Execute(moduleName, PyCallback_OnNodeIdReleased, args);
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
    unordered_map<wstring, shared_ptr<ModuleEntry>> modules_;
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

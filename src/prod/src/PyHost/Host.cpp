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

using namespace Common;
using namespace Federation;
using namespace std;

using namespace PyHost;

StringLiteral const TraceComponent("Host");

wstring const PyCallback_OnInitialize = L"OnInitialize";
wstring const PyCallback_OnNodeIdAcquired = L"OnNodeIdAcquired";
wstring const PyCallback_OnNodeIdReleased = L"OnNodeIdReleased";

//
// Impl
//

class Host::Impl
{
public:
    Impl() 
        : pyInterpreter_()
        , fs_()
        , nodeIdOwners_()
        , lock_()
    {
        pyInterpreter_.Register_SetNodeIdOwnership([this](wstring const & moduleName, wstring const & nodeId)
        {
            return SetNodeIdOwnership(moduleName, nodeId);
        });
    }

public:

    //
    // DLL exported functions
    //

    void Initialize(shared_ptr<FederationSubsystem> const & fs)
    {
        fs_ = fs;

        auto moduleName = PyHostConfig::GetConfig().ModuleName;

        if (moduleName.empty())
        {
            Trace.WriteInfo(TraceComponent, "No Python module to initialize");
        }
        else
        {
            Trace.WriteInfo(TraceComponent, "Initializing Python module '{0}' on node {1} ...", moduleName, fs->Id);

            vector<wstring> args;
            args.push_back(fs->IdString);
            pyInterpreter_.Execute(moduleName, PyCallback_OnInitialize, args);
        }
    }

    void OnRoutingTokenChanged()
    {
        AcquireWriteLock lock(lock_);

        for (auto const & mapEntry : nodeIdOwners_)
        {
            auto const & entry = mapEntry.second;

            if (entry->IsOwned && !IsNodeIdOwned(entry->Id))
            {
                auto error = NotifyNodeIdReleased(mapEntry.first, entry->Id);
                if (error.IsSuccess())
                {
                    Trace.WriteInfo(TraceComponent, "NotifyNodeIdReleased({0}, {1}) succeeded", mapEntry.first, entry->Id);
                }
                else
                {
                    Trace.WriteWarning(TraceComponent, "NotifyNodeIdReleased({0}, {1}) failed: {2}", mapEntry.first, entry->Id, error);
                }
            }
            else if (!entry->IsOwned && IsNodeIdOwned(entry->Id))
            {
                auto error = NotifyNodeIdAcquired(mapEntry.first, entry->Id);
                if (error.IsSuccess())
                {
                    Trace.WriteInfo(TraceComponent, "NotifyNodeIdAcquired({0}, {1}) succeeded", mapEntry.first, entry->Id);
                }
                else
                {
                    Trace.WriteWarning(TraceComponent, "NotifyNodeIdAcquired({0}, {1}) failed: {2}", mapEntry.first, entry->Id, error);
                }
            }
        }
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

        shared_ptr<NodeIdOwnershipEntry> entry;
        auto findIter = nodeIdOwners_.find(moduleName);
        if (findIter == nodeIdOwners_.end())
        {
            entry = make_shared<NodeIdOwnershipEntry>(nodeId, false);
            nodeIdOwners_.insert(make_pair(moduleName, entry));
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

private:

    bool IsNodeIdOwned(NodeId const & nodeId)
    {
        auto fs = fs_.lock();
        if (fs.get() == nullptr)
        {
            Trace.WriteInfo(TraceComponent, "Federation is closed"); 
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

    struct NodeIdOwnershipEntry
    {
        NodeIdOwnershipEntry(NodeId const & id, bool isOwned)
            : Id(id)
            , IsOwned(isOwned)
        {
        }
            
        NodeId Id;
        bool IsOwned;
    };

    PyInterpreter pyInterpreter_;
    weak_ptr<FederationSubsystem> fs_;
    unordered_map<wstring, shared_ptr<NodeIdOwnershipEntry>> nodeIdOwners_;
    RwLock lock_;
};

//
// Host
//

Host::Host() : impl_(make_shared<Impl>())
{
}

void Host::Initialize(shared_ptr<FederationSubsystem> const & fs)
{
    impl_->Initialize(fs);
}

void Host::OnRoutingTokenChanged()
{
    impl_->OnRoutingTokenChanged();
}

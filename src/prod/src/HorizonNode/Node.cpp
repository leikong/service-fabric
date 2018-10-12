// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "Common/Common.h"
#include "Federation/Federation.h"
#include "PyHost/dll/dll.h"
#include "Node.h"

using namespace Common;
using namespace Federation;
using namespace HorizonNode;
using namespace std;
using namespace Transport;

StringLiteral const TraceComponent("HorizonNode");

class Node::Impl : public ComponentRoot
{
private:
    Impl(NodeConfig const & cfg)
        : ComponentRoot()
        , fs_()
    {
        fs_ = make_shared<FederationSubsystem>(
            cfg,
            FabricCodeVersion(1, 0, 960, 0), // minimum acceptable version at Federation layer
            Uri(),
            SecuritySettings(),
            *this);
    }

public:

    static shared_ptr<Impl> Create(NodeConfig const & cfg)
    {
        return shared_ptr<Impl>(new Impl(cfg));
    }

    ErrorCode Open()
    {
        auto error = OpenFederationSubsystem();
        if (!error.IsSuccess()) { return error; }

        return InitializePyHost();
    }

private:
    ErrorCode OpenFederationSubsystem()
    {
        ManualResetEvent event(false);

        auto operation = fs_->BeginOpen(
            TimeSpan::MaxValue,
            [&event](AsyncOperationSPtr const &) { event.Set(); },
            this->CreateAsyncOperationRoot());

        event.WaitOne();

        auto error = fs_->EndOpen(operation);

        Trace.WriteInfo(TraceComponent, "opened FederationSubsystem: {0}", error);

        return error;
    }

    ErrorCode InitializePyHost()
    {
        auto error = PyHost_Initialize(fs_);
        if (!error.IsSuccess()) { return error; }

        Trace.WriteInfo(TraceComponent, "initialized PyHost");

        auto selfRoot = CreateComponentRoot();

        Trace.WriteInfo(TraceComponent, "created component root");

        fs_->RegisterRoutingTokenChangedEvent([this, selfRoot](EventArgs const &) 
            { 
                OnRoutingTokenChanged(); 
            });

        Trace.WriteInfo(TraceComponent, "registered for routing token events");

        fs_->RegisterMessageHandler(
            Actor::PyHost,
            [this, selfRoot](MessageUPtr & msg, OneWayReceiverContextUPtr &) { OnBroadcast(msg); },
            [this, selfRoot](MessageUPtr & msg, RequestReceiverContextUPtr & ctx) { OnRequest(msg, ctx); },
            false); // dispatchOnTransportThread

        Trace.WriteInfo(TraceComponent, "registered for broadcast events");

        return ErrorCodeValue::Success;
    }
    
    void OnRoutingTokenChanged()
    {
        auto error = PyHost_OnRoutingTokenChanged();
        if (!error.IsSuccess())
        {
            Trace.WriteWarning(TraceComponent, "PyHost_OnRoutingTokenChanged failed: {0}", error);
        }
    }

    void OnBroadcast(MessageUPtr & msg)
    {
        auto error = PyHost_OnBroadcast(msg);
        if (!error.IsSuccess())
        {
            Trace.WriteWarning(TraceComponent, "PyHost_OnBroadcast failed: {0}", error);
        }
    }

    void OnRequest(MessageUPtr &, RequestReceiverContextUPtr &)
    {
        // no-op
    }

private:

    shared_ptr<FederationSubsystem> fs_;
};

Node::Node(NodeConfig const & cfg) : impl_(Impl::Create(cfg))
{
}

ErrorCode Node::Open()
{
    return impl_->Open();
}

// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

#include "stdafx.h"
#include "Transport/Message.h"
#include "Transport/MessageHeader.h"
#include "Transport/MessageHeaderId.h"
#include "Transport/MessageHeaders.h"
#include "Federation/FederationMessage.h"
#include "Federation/Constants.h"

namespace Federation
{
    using namespace std;
    using namespace Common;
    using namespace Transport;

    Transport::Actor::Enum const FederationMessage::Actor(Actor::Federation);

    // Federation messages
    // Messages to be processed by the Join Manager.
    Global<FederationMessage> FederationMessage::NeighborhoodQueryRequest = make_global<FederationMessage>("NeighborhoodQueryRequest");
    Global<FederationMessage> FederationMessage::NeighborhoodQueryReply = make_global<FederationMessage>("NeighborhoodQueryReply");
    Global<FederationMessage> FederationMessage::ResumeJoin = make_global<FederationMessage>("ResumeJoin");
    Global<FederationMessage> FederationMessage::LockRequest = make_global<FederationMessage>("LockRequest");
    Global<FederationMessage> FederationMessage::LockGrant = make_global<FederationMessage>("LockGrant");
    Global<FederationMessage> FederationMessage::LockDeny = make_global<FederationMessage>("LockDeny");
    Global<FederationMessage> FederationMessage::LockTransferRequest = make_global<FederationMessage>("LockTransferRequest");
    Global<FederationMessage> FederationMessage::LockTransferReply = make_global<FederationMessage>("LockTransferReply");
    Global<FederationMessage> FederationMessage::LockRenew = make_global<FederationMessage>("LockRenew");
    Global<FederationMessage> FederationMessage::LockRelease = make_global<FederationMessage>("LockRelease");
    Global<FederationMessage> FederationMessage::UnlockRequest = make_global<FederationMessage>("UnlockRequest");
    Global<FederationMessage> FederationMessage::UnlockGrant = make_global<FederationMessage>("UnlockGrant");
    Global<FederationMessage> FederationMessage::UnlockDeny = make_global<FederationMessage>("UnlockDeny");


    Global<FederationMessage> FederationMessage::Depart = make_global<FederationMessage>("Depart");
    Global<FederationMessage> FederationMessage::NodeDoesNotMatchFault = make_global<FederationMessage>("NodeDoesNotMatchFault");
    Global<FederationMessage> FederationMessage::RejectFault = make_global<FederationMessage>("RejectFault");
    Global<FederationMessage> FederationMessage::TokenRequest = make_global<FederationMessage>("TokenRequest");
    Global<FederationMessage> FederationMessage::TokenTransfer = make_global<FederationMessage>("TokenTransfer");
    Global<FederationMessage> FederationMessage::TokenTransferAccept = make_global<FederationMessage>("TokenTransferAccept");
    Global<FederationMessage> FederationMessage::TokenTransferReject = make_global<FederationMessage>("TokenTransferReject");
    Global<FederationMessage> FederationMessage::EdgeProbe = make_global<FederationMessage>("EdgeProbe");
    Global<FederationMessage> FederationMessage::RingAdjust = make_global<FederationMessage>("RingAdjust");
    Global<FederationMessage> FederationMessage::TokenProbe = make_global<FederationMessage>("TokenProbe");
    Global<FederationMessage> FederationMessage::TokenEcho = make_global<FederationMessage>("TokenEcho");
    Global<FederationMessage> FederationMessage::Ping = make_global<FederationMessage>("Ping");
    Global<FederationMessage> FederationMessage::UpdateRequest = make_global<FederationMessage>("UpdateRequest");
    Global<FederationMessage> FederationMessage::UpdateReply = make_global<FederationMessage>("UpdateReply");

    // VoteManager messages
    Global<FederationMessage> FederationMessage::VotePing = make_global<FederationMessage>("VotePing");
    Global<FederationMessage> FederationMessage::VotePingReply = make_global<FederationMessage>("VotePingReply");
    Global<FederationMessage> FederationMessage::VoteTransferRequest = make_global<FederationMessage>("VoteTransferRequest");
    Global<FederationMessage> FederationMessage::VoteTransferReply = make_global<FederationMessage>("VoteTransferReply");
	Global<FederationMessage> FederationMessage::VoteRenewRequest = make_global<FederationMessage>("VoteRenewRequest");
	Global<FederationMessage> FederationMessage::VoteRenewReply = make_global<FederationMessage>("VoteRenewReply");

    // ArbitrationManager messages
    Global<FederationMessage> FederationMessage::ArbitrateRequest = make_global<FederationMessage>("ArbitrateRequest");
    Global<FederationMessage> FederationMessage::ExtendedArbitrateRequest = make_global<FederationMessage>("ExtendedArbitrateRequest");
    Global<FederationMessage> FederationMessage::ArbitrateReply = make_global<FederationMessage>("ArbitrateReply");
    Global<FederationMessage> FederationMessage::DelayedArbitrateReply = make_global<FederationMessage>("DelayedArbitrateReply");
    Global<FederationMessage> FederationMessage::ArbitrateKeepAlive = make_global<FederationMessage>("ArbitrateKeepAlive");

    // Neighborhood recovery messages
    Global<FederationMessage> FederationMessage::TicketGapRequest = make_global<FederationMessage>("TicketGapRequest");
    Global<FederationMessage> FederationMessage::TicketGapReply = make_global<FederationMessage>("TicketGapReply");

    Global<FederationMessage> FederationMessage::LivenessQueryRequest = make_global<FederationMessage>("LivenessQueryRequest");
    Global<FederationMessage> FederationMessage::LivenessQueryReply = make_global<FederationMessage>("LivenessQueryReply");

    // ACKs are used by high layers and we don't need them to carry liveness headers
    Global<FederationMessage> FederationMessage::RoutingAck = make_global<FederationMessage>("RoutingAck", Actor::Empty);
    Global<FederationMessage> FederationMessage::BroadcastAck = make_global<FederationMessage>("BroadcastAck", Actor::Empty);
    Global<FederationMessage> FederationMessage::MulticastAck = make_global<FederationMessage>("MulticastAck", Actor::Empty);

    Global<FederationMessage> FederationMessage::LivenessUpdate = make_global<FederationMessage>("LivenessUpdate");

    Global<FederationMessage> FederationMessage::ExternalRingPing = make_global<FederationMessage>("ExternalRingPing");
    Global<FederationMessage> FederationMessage::ExternalRingPingResponse = make_global<FederationMessage>("ExternalRingPingResponse");

    Global<FederationMessage> FederationMessage::VoterStoreConfigQueryRequest = make_global<FederationMessage>("VoterStoreConfigQueryRequest");
    Global<FederationMessage> FederationMessage::VoterStoreConfigQueryReply = make_global<FederationMessage>("VoterStoreConfigQueryReply");
    Global<FederationMessage> FederationMessage::VoterStoreIntroduceRequest = make_global<FederationMessage>("VoterStoreIntroduceRequest");
    Global<FederationMessage> FederationMessage::VoterStoreIntroduceReply = make_global<FederationMessage>("VoterStoreIntroduceReply");
    Global<FederationMessage> FederationMessage::VoterStoreBootstrapRequest = make_global<FederationMessage>("VoterStoreBootstrapRequest");
    Global<FederationMessage> FederationMessage::VoterStoreBootstrapReply = make_global<FederationMessage>("VoterStoreBootstrapReply");
    Global<FederationMessage> FederationMessage::VoterStoreLeaderProbeRequest = make_global<FederationMessage>("VoterStoreLeaderProbeRequest");
    Global<FederationMessage> FederationMessage::VoterStoreLeaderProbeReply = make_global<FederationMessage>("VoterStoreLeaderProbeReply");
    Global<FederationMessage> FederationMessage::VoterStoreJoin = make_global<FederationMessage>("VoterStoreJoin");
    Global<FederationMessage> FederationMessage::VoterStoreSyncRequest = make_global<FederationMessage>("VoterStoreSyncRequest");
    Global<FederationMessage> FederationMessage::VoterStoreSyncReply = make_global<FederationMessage>("VoterStoreSyncReply");
    Global<FederationMessage> FederationMessage::VoterStoreProgressRequest = make_global<FederationMessage>("VoterStoreProgressRequest");
    Global<FederationMessage> FederationMessage::VoterStoreProgressReply = make_global<FederationMessage>("VoterStoreProgressReply");
    Global<FederationMessage> FederationMessage::VoterStoreReadRequest = make_global<FederationMessage>("VoterStoreReadRequest");
    Global<FederationMessage> FederationMessage::VoterStoreReadReply = make_global<FederationMessage>("VoterStoreReadReply");
    Global<FederationMessage> FederationMessage::VoterStoreWriteRequest = make_global<FederationMessage>("VoterStoreWriteRequest");
    Global<FederationMessage> FederationMessage::VoterStoreWriteReply = make_global<FederationMessage>("VoterStoreWriteReply");

    Global<FederationMessage> FederationMessage::EmptyRequest = make_global<FederationMessage>("EmptyRequest");
    Global<FederationMessage> FederationMessage::EmptyReply = make_global<FederationMessage>("EmptyReply");

    Transport::MessageUPtr FederationMessage::CreateMessage() const
    {
        auto message = Common::make_unique<Message>();
        message->Headers.Add(ActorHeader(actor_));
        message->Headers.Add(actionHeader_);
        message->Headers.Add(HighPriorityHeader(true));
        return std::move(message);
    }

    bool FederationMessage::IsFederationMessage(Message const & msg)
    {
        return msg.Actor == Actor;
    }

    FederationRoutingTokenHeader::FederationRoutingTokenHeader(NodeIdRange const & range, uint64 sourceVersion, uint64 const & targetVersion)
        : range_(range), sourceVersion_(sourceVersion), targetVersion_(targetVersion)
    {
        expiryTime_ = Common::Stopwatch::Now() + FederationConfig::GetConfig().TokenProbeInterval;
    }

    void FederationRoutingTokenHeader::WriteTo(TextWriter& w, FormatOptions const &) const
    { 
        w.Write("[{0}:{1:x},{2:x}]", range_, sourceVersion_, targetVersion_); 
    }
}

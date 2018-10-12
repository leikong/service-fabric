// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once
#include "stdafx.h"

namespace Federation
{
    class StateMachineTestInfo
    {
    public:
        StateMachineTestInfo(
            std::string name,
            std::string initialState,
            std::string triggerEvent,
            std::string finalState,
            std::string actionsTaken)
            : name_(name), 
            initialState_(initialState), 
            triggerEvent_(triggerEvent), 
            finalState_(finalState),
            actionsTaken_(actionsTaken)
        {
        }

        __declspec(property(get=getName)) std::string Name;
        __declspec(property(get=getInitialState)) std::string InitialState;
        __declspec(property(get=getTriggerEvent)) std::string TriggerEvent;
        __declspec(property(get=getFinalState)) std::string FinalState;
        __declspec(property(get=getActionsTaken)) std::string ActionsTaken;

        std::string getName() const { return name_; }
        std::string getInitialState() const { return initialState_; }
        std::string getTriggerEvent() const { return triggerEvent_; }
        std::string getFinalState() const { return finalState_; }
        std::string getActionsTaken() const { return actionsTaken_; }

    private:
        std::string name_;
        std::string initialState_;
        std::string triggerEvent_;
        std::string finalState_;
        std::string actionsTaken_;
    };

    class TestNodeInfo
    {
    public:
        TestNodeInfo()
        {
        }

        TestNodeInfo(NodeId id, NodePhase::Enum phase)
            : nodeId_(id), phase_(phase)
        {
        }

        static bool Create(std::string string, __out TestNodeInfo & testNode);

        _declspec(property(get=getNodeId)) NodeId Id;
        _declspec(property(get=getPhase)) NodePhase::Enum Phase;

        NodeId getNodeId() const { return nodeId_; }
        NodePhase::Enum getPhase() const { return phase_; }

        std::string ToString();
    private:
        NodeId nodeId_;
        NodePhase::Enum phase_;
    };

    class TestVoteInfo
    {
         public:
        TestVoteInfo()
        {
        }

        TestVoteInfo(NodeId voteId, Common::StopwatchTime superTicket, Common::StopwatchTime globalTicket)
            : voteId_(voteId), superTicket_(superTicket), globalTicket_(globalTicket)
        {
        }

        static bool Create(std::string string, __out TestVoteInfo & testVote);

        _declspec(property(get=getId)) NodeId Id;
        _declspec(property(get=getSuperTicket)) Common::StopwatchTime SuperTicket;
        _declspec(property(get=getGlobalTicket)) Common::StopwatchTime GlobalTicket;

        NodeId getId() const { return voteId_; }
        Common::StopwatchTime getSuperTicket() const { return superTicket_; }
        Common::StopwatchTime getGlobalTicket() const { return globalTicket_; }

        std::string ToString();
    private:
        NodeId voteId_;
        Common::StopwatchTime superTicket_;
        Common::StopwatchTime globalTicket_;
    };
};

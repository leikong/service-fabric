// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once
#include "stdafx.h"
#include "StateMachineTestInfo.Test.h"

using namespace Common;

namespace Federation
{
    bool TestNodeInfo::Create(std::string string, __out TestNodeInfo & testNode)
    {
        StringCollection nodeInfo;
        StringUtility::Split<std::string>(string, nodeInfo, ":");
        if (!NodeId::TryParse(nodeInfo[0], testNode.nodeId_))
        {
            return false;
        }

        if (nodeInfo[1] == "B")
        {
            testNode.phase_ = NodePhase::Booting;
        }
        else if (nodeInfo[1] == "I")
        {
            testNode.phase_  = NodePhase::Inserting;
        }
        else if (nodeInfo[1] == "J")
        {
            testNode.phase_  = NodePhase::Joining;
        }
        else if (nodeInfo[1] == "R")
        {
            testNode.phase_  = NodePhase::Routing;
        }
        else if (nodeInfo[1] == "S")
        {
            testNode.phase_  = NodePhase::Shutdown;
        }
        else
        {
            return false;
        }
        return true;
    }

    std::string TestNodeInfo::ToString()
    {
        return formatString.L("{0}:{1}", nodeId_, phase_);
    }

    bool TestVoteInfo::Create(std::string string, __out TestVoteInfo & testVote)
    {
        StringCollection voteInfo;
        StringUtility::Split<std::string>(string, voteInfo, ":");
        if (!NodeId::TryParse(voteInfo[0], testVote.voteId_))
        {
            return false;
        }

        if (voteInfo.size() > 1)
        {
            if (voteInfo[1] == "T")
            {
                testVote.superTicket_ = Stopwatch::Now() + TimeSpan::FromMinutes(2);
            }
            else if (voteInfo[1] == "F")
            {
                testVote.superTicket_ = StopwatchTime::Zero;
            }
            else
            {
                return false;
            }
        }
        else
        {
            testVote.superTicket_ = Stopwatch::Now() + TimeSpan::FromMinutes(2);
        }

        if (voteInfo.size() > 2)
        {
            if (voteInfo[2] == "T")
            {
                testVote.globalTicket_ = Stopwatch::Now() + TimeSpan::FromMinutes(2);
            }
            else if (voteInfo[2] == "F")
            {
                testVote.globalTicket_ = StopwatchTime::Zero;
            }
            else
            {
                return false;
            }
        }
        else
        {
            testVote.globalTicket_ = Stopwatch::Now() + FederationConfig::GetConfig().GlobalTicketLeaseDuration;
        }
        return true;
    }

    std::string TestVoteInfo::ToString()
    {
        return formatString.L("{0}:{1}:{2}", voteId_, superTicket_, globalTicket_);
    }
}

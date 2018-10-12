// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace FederationUnitTests
{
    class SiteNodeHelper
    {
    public:
        static Federation::SiteNodeSPtr CreateSiteNode(Federation::NodeId nodeId, std::string host, std::string port);
        static Federation::SiteNodeSPtr CreateSiteNode(Federation::NodeId nodeId);
        static std::string GetLoopbackAddress();
        static std::string GetFederationPort();
        static std::string GetLeaseAgentPort();
        static std::string BuildAddress(std::string hostname, std::string port);
        static std::string GetTicketFilePath(Federation::NodeId nodeId);
        static void DeleteTicketFile(Federation::NodeId nodeId);
        static std::string GetWorkingDir();
    };
};


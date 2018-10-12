// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

#include "SiteNodeHelper.h"

using namespace Federation;
using namespace Transport;
using namespace Common;
using namespace std;
using namespace FederationUnitTests;

string SiteNodeHelper::GetLoopbackAddress()
{
    return "localhost";
}

string SiteNodeHelper::GetFederationPort()
{
    USHORT basePort = 0;
    TestPortHelper::GetPorts(1, basePort);

    string portString;
    StringWriter(portString).Write(basePort);
    return portString;
}

string SiteNodeHelper::GetLeaseAgentPort()
{
    USHORT basePort = 0;
    TestPortHelper::GetPorts(1, basePort);

    string portString;
    StringWriter(portString).Write(basePort);
    return portString;
}

string SiteNodeHelper::BuildAddress(string hostname, string port)
{
    string addr = hostname;
    addr.append(":");
    addr.append(port);

    return addr;
}

SiteNodeSPtr SiteNodeHelper::CreateSiteNode(NodeId nodeId, string host, string port)
{
    string addr = BuildAddress(host, port);
    string laAddr = BuildAddress(host, GetLeaseAgentPort());
    NodeConfig config = NodeConfig(nodeId, addr, laAddr, GetWorkingDir());
    return SiteNode::Create(config, FabricCodeVersion(), nullptr);
}

SiteNodeSPtr SiteNodeHelper::CreateSiteNode(NodeId nodeId)
{
    string  port = GetFederationPort();
    string host = GetLoopbackAddress();
    return SiteNodeHelper::CreateSiteNode(nodeId, host, port);
}

string SiteNodeHelper::GetWorkingDir()
{
    string workingDir = ".\\Federation.Test\\Data";
    if(!Directory::Exists(workingDir))
    {
        Directory::Create(workingDir);
    }

    return workingDir;
}

string SiteNodeHelper::GetTicketFilePath(Federation::NodeId nodeId)
{
    return Path::Combine(GetWorkingDir(), nodeId.ToString() + ".tkt");
}

void SiteNodeHelper::DeleteTicketFile(Federation::NodeId nodeId)
{
    File::Delete(GetTicketFilePath(nodeId), false);
}

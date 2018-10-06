// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include <iostream>

#include "Common/Common.h"
#include "Federation/Federation.h"

using namespace Common;
using namespace Federation;
using namespace std;
using namespace Transport;

int main(int argc, char* argv[])
{
    // TODO: read from arguments
    wstring nodeIdString = L"0";
    wstring nodeAddress = L"localhost:8080";
    wstring leaseAgentAddress = L"localhost:8081";
    wstring workingDirectory = L"./";

    NodeId nodeId;
    if(!NodeId::TryParse(nodeIdString, nodeId))
    {
        wcerr << L"Failed to parse '" << nodeIdString << L"' as NodeId" << endl;
        return 1;
    }

    ComponentRoot mockRoot;

    auto federation = std::make_shared<Federation::FederationSubsystem>(
        NodeConfig(nodeId, nodeAddress, leaseAgentAddress, workingDirectory),
        FabricCodeVersion(),
        Uri(),
        SecuritySettings(),
        mockRoot);

    wcout << L"Created FederationSubsystem" << endl;

    wcin.ignore();
}


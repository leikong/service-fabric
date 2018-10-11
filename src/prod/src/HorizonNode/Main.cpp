// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include <stdio.h>

#include "Common/Common.h"
#include "Federation/Federation.h"
#include "PyHost/dll/dll.h"

using namespace Common;
using namespace Federation;
using namespace Transport;
using namespace std;

StringLiteral const TraceComponent("Main");

const size_t MaxArgLen = 128;

void OnRoutingTokenChanged(shared_ptr<FederationSubsystem> const & fs)
{
    PyHost_OnRoutingTokenChanged(fs);
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {   
        printf("usage: %s -n <node ID> -p [port] -h [hostname/ip (default='localhost')] \n", argv[0]);
        printf("example: %s -n 10 -p 19000\n", argv[0]);
        return 1;
    }

    string nodeIdString = "0";
    string hostname = "localhost";
    unsigned int port = 19000;
    wstring workingDirectory = L"./";

    for (auto ix=0; ix<argc; ++ix)
    {
        if (ix+1 < argc)
        {
            if (strncmp(argv[ix], "-n", MaxArgLen) == 0)
            {
                nodeIdString = argv[++ix];
            }   
            else if (strncmp(argv[ix], "-p", MaxArgLen) == 0)
            {
                port = atoi(argv[++ix]);
            }
            else if (strncmp(argv[ix], "-h", MaxArgLen) == 0)
            {
                hostname = argv[++ix];
            }
        }
    }

    NodeId nodeId;
    if (!NodeId::TryParse(StringUtility::Utf8ToUtf16(nodeIdString), nodeId))
    {
        printf("Failed to parse '%s' as NodeId \n", nodeIdString.c_str());
        return 1;
    }

    string nodeAddress = hostname + ":" + to_string(port);
    string leaseAddress = hostname + ":" + to_string(port + 1);

    printf("Running node %s at %s...\n",
        nodeIdString.c_str(), 
        nodeAddress.c_str());

    ComponentRoot mockRoot;
    auto federation = make_shared<Federation::FederationSubsystem>(
        NodeConfig(
            nodeId, 
            StringUtility::Utf8ToUtf16(nodeAddress), 
            StringUtility::Utf8ToUtf16(leaseAddress), 
            workingDirectory),
        FabricCodeVersion(1, 0, 960, 0), // minimum acceptable version at Federation layer
        Uri(),
        SecuritySettings(),
        mockRoot);

    printf("Created FederationSubsystem \n");

    PyHost_Initialize();

    printf("Initialized Python host\n");

    auto unusedHandlerId = federation->RegisterRoutingTokenChangedEvent(
        [federation](EventArgs const &) { OnRoutingTokenChanged(federation); });

    AutoResetEvent event(false);

    auto operation = federation->BeginOpen(
        TimeSpan::MaxValue,
        [&event](AsyncOperationSPtr const &) { event.Set(); },
        AsyncOperationSPtr());

    event.WaitOne();

    auto error = federation->EndOpen(operation);

    printf("Opened FederationSubsystem: %s \n", StringUtility::Utf16ToUtf8(error.ErrorCodeValueToString()).c_str());

    if (error.IsSuccess())
    {
        cin.ignore();
    }
}

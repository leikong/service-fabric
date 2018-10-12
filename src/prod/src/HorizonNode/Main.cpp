// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include <stdio.h>

#include "Common/Common.h"
#include "Federation/Federation.h"
#include "Node.h"

using namespace Common;
using namespace Federation;
using namespace HorizonNode;

const size_t MaxArgLen = 128;

void PrintHelp(char* exe)
{
    printf("usage: %s [-n <node ID> -p <port> -h <hostname/ip (default='localhost')>]\n", exe);
    printf("example: %s -n 10 -p 19000\n", exe);
}

int main(int argc, char* argv[])
{
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

        if (strncmp(argv[ix], "-?", MaxArgLen) == 0)
        {
            PrintHelp(argv[0]);
            exit(1);
        }
    }

    NodeId nodeId;
    if (!NodeId::TryParse(StringUtility::Utf8ToUtf16(nodeIdString), nodeId))
    {
        printf("Failed to parse '%s' as NodeId\n", nodeIdString.c_str());
        return 1;
    }

    string nodeAddress = hostname + ":" + to_string(port);
    string leaseAddress = hostname + ":" + to_string(port + 1);

    printf("Starting node %s at %s...\n",
        nodeIdString.c_str(), 
        nodeAddress.c_str());

    Node node(NodeConfig(
        nodeId, 
        StringUtility::Utf8ToUtf16(nodeAddress), 
        StringUtility::Utf8ToUtf16(leaseAddress), 
        workingDirectory));

    auto error = node.Open();

    if (error.IsSuccess())
    {
        printf("Running %s...\n", argv[0]);

        cin.ignore();
    }
    else
    {
        printf("Failed to start node: %s\n", StringUtility::Utf16ToUtf8(error.ErrorCodeValueToString()).c_str());
    }
}

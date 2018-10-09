// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include <stdio.h>
#include <codecvt>
#include <locale>

#include "Common/Common.h"
#include "Federation/Federation.h"

using namespace Common;
using namespace Federation;
using namespace std;
using namespace Transport;

StringLiteral const TraceComponent("Main");

string ToString(wstring const & in)
{
    static wstring_convert<codecvt_utf8_utf16<wchar_t>, wchar_t> cvt;
    return cvt.to_bytes(in.c_str());
}

int main(int argc, char* argv[])
{
    // TODO: read from arguments
    wstring nodeIdString = L"0";
    wstring nodeAddress = L"localhost:8080";
    wstring leaseAgentAddress = L"localhost:8081";
    wstring workingDirectory = L"./";

    printf("Running at %s. Press any key to continue... \n", ToString(nodeAddress).c_str());

    getchar();

    Trace.WriteInfo(TraceComponent, "Running main...");

    NodeId nodeId(LargeInteger(0, 0));
    if (!NodeId::TryParse(nodeIdString, nodeId))
    {
        printf("Failed to parse '%s' as NodeId \n", ToString(nodeIdString).c_str());
        return 1;
    }

    ComponentRoot mockRoot;
    auto federation = std::make_shared<Federation::FederationSubsystem>(
        NodeConfig(
            nodeId, 
            nodeAddress, 
            leaseAgentAddress, 
            workingDirectory),
        FabricCodeVersion(),
        Uri(),
        SecuritySettings(),
        mockRoot);

    printf("Created FederationSubsystem \n");

    AutoResetEvent event(false);

    auto operation = federation->BeginOpen(
        TimeSpan::MaxValue,
        [&event](AsyncOperationSPtr const &) { event.Set(); },
        AsyncOperationSPtr());

    event.WaitOne();

    auto error = federation->EndOpen(operation);

    printf("Opened FederationSubsystem: %s \n", ToString(error.ErrorCodeValueToString()).c_str());

    if (error.IsSuccess())
    {
        cin.ignore();
    }
}

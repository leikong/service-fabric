// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

namespace Transport
{
    using namespace Common;
    using namespace std;

    StringLiteral const Constants::TcpTrace("Tcp");
    StringLiteral const Constants::MemoryTrace("Memory");
    StringLiteral const Constants::DemuxerTrace("Demuxer");
    StringLiteral const Constants::ConfigTrace("Config");
    Global<string> const Constants::ClaimsTokenError = make_global<string>("ClaimsTokenError");
    Global<string> const Constants::ConnectionAuth = make_global<string>("ConnectionAuth");
    StringLiteral const Constants::PartitionIdString("PartitionId");
    Global<string> const Constants::PartitionIdWString = make_global<string>("PartitionId");
    Global<string> const Constants::ClaimsMessageAction = make_global<string>("Claims");
    Global<string> const Constants::ReconnectAction = make_global<string>("Reconnect");
}

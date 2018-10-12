// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace std;
using namespace Common;
using namespace Federation;

GlobalString Constants::SeedNodeVoteType = make_global<string>("SeedNode");
GlobalString Constants::SqlServerVoteType = make_global<string>("SqlServer");
GlobalString Constants::WindowsAzureVoteType = make_global<string>("WindowsAzure");

GlobalString Constants::GlobalTimestampEpochName = make_global<string>("GlobalTimestampEpoch");

char const* Constants::RejectedTokenKey = "RejectedTokens";
char const* Constants::NeighborHeadersIgnoredKey = "NeighborHeadersIgnored";

StringLiteral Constants::VersionTraceType("Version");


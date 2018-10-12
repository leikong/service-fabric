// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "Common/Common.h"
#include "Federation/Federation.h"

#include "dll.h"
#include "../Host.h"
#include "../SingletonHost.h"

using namespace Common;
using namespace Federation;
using namespace PyHost;
using namespace std;
using namespace Transport;

ErrorCode PyHost_Initialize(shared_ptr<FederationSubsystem> const & fs)
{
    return SingletonHost::GetInstance().Initialize(fs);
}

ErrorCode PyHost_OnRoutingTokenChanged()
{
    return SingletonHost::GetInstance().OnRoutingTokenChanged();
}

ErrorCode PyHost_OnBroadcast(MessageUPtr & msg)
{
    return SingletonHost::GetInstance().OnBroadcast(msg);
}

// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "Common/Common.h"
#include "Federation/Federation.h"

#include "dll.h"
#include "../Host.h"
#include "../SingletonHost.h"

using namespace Federation;
using namespace PyHost;
using namespace std;

void PyHost_Initialize(shared_ptr<FederationSubsystem> const & fs)
{
    SingletonHost::GetInstance().Initialize(fs);
}

void PyHost_OnRoutingTokenChanged()
{
    SingletonHost::GetInstance().OnRoutingTokenChanged();
}

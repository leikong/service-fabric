// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

extern "C" 
{
    Common::ErrorCode PyHost_Initialize(std::shared_ptr<Federation::FederationSubsystem> const &);
    Common::ErrorCode PyHost_OnRoutingTokenChanged();
    Common::ErrorCode PyHost_OnBroadcast(Transport::MessageUPtr &);
}

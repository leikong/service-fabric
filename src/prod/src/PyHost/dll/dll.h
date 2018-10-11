// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

extern "C" 
{
    void PyHost_Initialize();
    void PyHost_OnRoutingTokenChanged(std::shared_ptr<Federation::FederationSubsystem> const &);
}

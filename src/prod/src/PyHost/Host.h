// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace PyHost
{
    class Host
    {
    public:
        Host();

        void Initialize();
        void OnRoutingTokenChanged(std::shared_ptr<Federation::FederationSubsystem> const & fs);

    private:
        class Impl;

        std::shared_ptr<Impl> impl_;
    };
}

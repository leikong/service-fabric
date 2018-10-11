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

        void Initialize(std::shared_ptr<Federation::FederationSubsystem> const &);
        void OnRoutingTokenChanged();

    private:
        class Impl;

        std::shared_ptr<Impl> impl_;
    };
}

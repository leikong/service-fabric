// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace PyHost
{
    class PyHostConfig : Common::ComponentConfig
    {
        DECLARE_SINGLETON_COMPONENT_CONFIG(PyHostConfig, "PyHost")

        // TODO: currently only for testing
        INTERNAL_CONFIG_ENTRY(std::string, "PyHost", ModuleNames, "", Common::ConfigEntryUpgradePolicy::Dynamic)
    };
}

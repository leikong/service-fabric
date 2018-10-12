// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class ContainerEnvironment
    {
    public:
        
        static bool IsContainerHost();
        static std::string GetContainerTracePath();
        static std::string GetContainerNetworkingMode();

        static GlobalString IsContainerHostEnvironmentVariableName;
        static GlobalString ContainerNetworkingModeEnvironmentVariable;
    private:
        static GlobalString ContainertracePathEnvironmentVariableName;        
    };
}

// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"
#include "Common/ContainerEnvironment.h"

Common::GlobalString Common::ContainerEnvironment::IsContainerHostEnvironmentVariableName = make_global<string>("Fabric_IsContainerHost");
Common::GlobalString Common::ContainerEnvironment::ContainertracePathEnvironmentVariableName = make_global<string>("FabricLogRoot");
Common::GlobalString Common::ContainerEnvironment::ContainerNetworkingModeEnvironmentVariable = make_global<string>("Fabric_NetworkingMode");

bool Common::ContainerEnvironment::IsContainerHost()
{
    string envValue;
    bool envVarFound = Environment::GetEnvironmentVariable(ContainerEnvironment::IsContainerHostEnvironmentVariableName, envValue, Common::NOTHROW());
    bool isContainerHost = false;
    if (envVarFound)
    {
        if (StringUtility::ParseBool::Try(envValue, isContainerHost))
        {
            return isContainerHost;
        }
    }

    return false;
}

std::string Common::ContainerEnvironment::GetContainerTracePath()
{
    string envValue = "";
    bool envVarFound = Environment::GetEnvironmentVariable(ContainerEnvironment::ContainertracePathEnvironmentVariableName, envValue, Common::NOTHROW());
    if (envVarFound)
    {
        return Path::Combine(envValue, "Traces");
    }

    return envValue;
}

std::string Common::ContainerEnvironment::GetContainerNetworkingMode()
{
    string envValue = "";
    Environment::GetEnvironmentVariable(ContainerEnvironment::ContainerNetworkingModeEnvironmentVariable, envValue, Common::NOTHROW());
    return envValue;
}

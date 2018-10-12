// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    namespace IpUtility
    {
        ErrorCode GetDnsServers(
            __out std::vector<std::string> & list
            );

        ErrorCode GetDnsServersPerAdapter(
            __out std::map<std::string, std::vector<std::string>> & map
            );

        ErrorCode GetIpAddressesPerAdapter(
            __out std::map<std::string, std::vector<Common::IPPrefix>> & map
            );

        ErrorCode GetAdapterAddressOnTheSameNetwork(
            __in std::string input,
            __out std::string& output
            );

        ErrorCode GetGatewaysPerAdapter(
            __out std::map<std::string, std::vector<std::string>> &map
            );

        ErrorCode GetFirstNonLoopbackAddress(
            std::map<std::string, std::vector<Common::IPPrefix>> addressesPerAdapter,
            __out string &nonLoopbackIp
            );
    }
}

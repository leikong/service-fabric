// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Transport
{
    // todo, leikong, remove ResolveOptions as it is a duplicate of Common::AddressFamily, need to move TryParse and WriteTo over
    namespace ResolveOptions
    {
        enum Enum
        {
            Invalid = -1,
            Unspecified = AF_UNSPEC,
            IPv4 = AF_INET,
            IPv6 = AF_INET6
        };

        _Success_(return)
        bool TryParse(std::string const & input, _Out_ Enum & output);
        void WriteToTextWriter(Common::TextWriter & w, Enum const & e);
    }

    class TcpTransportUtility : public Common::TextTraceComponent<Common::TraceTaskCodes::Transport>
    {
    public:
        static Common::ErrorCode GetLocalFqdn(std::string & hostname);

        static Common::ErrorCode ResolveToAddresses(
            std::string const & hostName,
            ResolveOptions::Enum options,
            _Out_ std::vector<Common::Endpoint> & resolved);

        static Common::ErrorCode GetLocalAddresses(
            _Out_ std::vector<Common::Endpoint> & localAddresses,
            ResolveOptions::Enum options = ResolveOptions::Unspecified);

        static Common::ErrorCode GetFirstLocalAddress(
            _Out_ Common::Endpoint & localAddress,
            ResolveOptions::Enum options = ResolveOptions::Unspecified);

        static bool IsValidEndpointString(std::string const & address);

        static Common::Endpoint ParseEndpointString(std::string const & address);

        static std::string ParseHostString(std::string const & address);

        static USHORT ParsePortString(std::string const & address);

        static Common::ErrorCode TryParseEndpointString(std::string const & address, Common::Endpoint & endpoint);

        static Common::ErrorCode TryParseHostPortString(std::string const & address, std::string & host, std::string & port);

        _Success_(return)
        static bool TryParsePortString(std::string const & portString, _Out_ USHORT & port);

        static bool IsLoopbackAddress(std::string const & address);

        // Note: local != loopback
        static bool IsLocalEndpoint(Common::Endpoint const & endpoint);

        _Success_(return)
        static bool TryParseHostNameAddress(
            std::string const & hostnameAddress,
            _Out_ std::string & hostnameResult,
            _Out_ USHORT & portResult);

        static Common::ErrorCode TryResolveHostNameAddress(
            std::string const & hostnameAddress,
            ResolveOptions::Enum addressType,
            _Out_ std::vector<Common::Endpoint> & endpoints);

        static void EnableTcpFastLoopbackIfNeeded(
            Common::Socket & socket,
            std::string const & traceId);

        static std::string TcpTransportUtility::ConstructAddressString(
            std::string const & ipAddressOrFQDN,
            uint port);
   };
}

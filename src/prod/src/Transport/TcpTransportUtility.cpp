// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace Transport;
using namespace Common;
using namespace std;

namespace Transport
{
    namespace ResolveOptions
    {
        _Use_decl_annotations_
            bool TryParse(string const & input, Enum & result)
        {
            if (StringUtility::AreEqualCaseInsensitive(input, "unspecified"))
            {
                result = Enum::Unspecified;
                return true;
            }

            if (StringUtility::AreEqualCaseInsensitive(input, "ipv4"))
            {
                result = Enum::IPv4;
                return true;
            }

            if (StringUtility::AreEqualCaseInsensitive(input, "ipv6"))
            {
                result = Enum::IPv6;
                return true;
            }

            return false;
        }

        void WriteToTextWriter(Common::TextWriter & w, Enum const & e)
        {
            switch (e)
            {
            case Unspecified: w << "Unspecified"; return;
            case IPv4: w << "IPv4"; return;
            case IPv6: w << "IPv6"; return;
            }

            w << "ResolveOptions(" << static_cast<int>(e) << ')';
        }
    }

    struct AddrInfoWDeleter
    {
        void operator()(PADDRINFOW ptr) const
        {
            FreeAddrInfoW(ptr);
        }
    };
}

bool TcpTransportUtility::IsLoopbackAddress(string const & address)
{
    Endpoint endpoint;
    if (TcpTransportUtility::TryParseEndpointString(address, endpoint).IsSuccess())
    {
        return endpoint.IsLoopback();
    }

    const string localhost = "localhost:";
    return StringUtility::StartsWithCaseInsensitive(address, localhost);
}

ErrorCode TcpTransportUtility::GetLocalFqdn(std::string & hostname)
{
    ADDRINFOW hint = {};
    hint.ai_flags = AI_CANONNAME;

    PADDRINFOW results = nullptr;

    int error = ::GetAddrInfoW(
        "",
        nullptr,
        &hint,
        &results);
    unique_ptr<ADDRINFOW, AddrInfoWDeleter> resultUptr(results);

    if (0 != error)
    {
        auto errorCode = ErrorCode::FromWin32Error(::WSAGetLastError());
        WriteError(
            Constants::TcpTrace, "GetLocalFqdn: GetAddrInfoW() failed with {0}",
            errorCode);
        return errorCode;
    }

    PADDRINFOW ptr = results;
    while(ptr)
    {
        if (results->ai_canonname)
        {
#ifdef PLATFORM_UNIX
            Utf8ToUtf16NotNeeded2(results->ai_canonname, hostname);
#else
            hostname = string(results->ai_canonname);
#endif
            return ErrorCodeValue::Success;
        }

        ptr = ptr->ai_next;
    }

    hostname.clear();
    return ErrorCodeValue::OperationFailed;
}

_Use_decl_annotations_
bool TcpTransportUtility::TryParseHostNameAddress(
    string const & hostnameAddress,
    string & hostnameResult,
    USHORT & portResult)
{
    size_t delimiterOffset = hostnameAddress.find(":");
    if (delimiterOffset == string::npos)
    {
        WriteError(
            Constants::TcpTrace,
            "TryParseHostNameAddress: cannot find port in address string '{0}'",
            hostnameAddress);
        return false;
    }

    string hostname = hostnameAddress.substr(0, delimiterOffset);
    string port = hostnameAddress.substr(delimiterOffset + 1);

    if (!TryParsePortString(port, portResult))
    {
        WriteError(
            Constants::TcpTrace,
            "TryParseHostNameAddress: cannot convert '{0}' in address '{1}' to tcp port",
            port, hostnameAddress);

        return false;
    }

    hostnameResult = std::move(hostname);
    return true;
}

_Use_decl_annotations_
ErrorCode TcpTransportUtility::TryResolveHostNameAddress(
    string const & hostnameAddress,
    ResolveOptions::Enum addressType,
    std::vector<Endpoint> & endpoints)
{
    string hostname;
    USHORT portValue;

    if (!TryParseHostNameAddress(hostnameAddress, hostname, portValue))
    {
        return ErrorCodeValue::InvalidAddress;
    }

    auto error = TcpTransportUtility::ResolveToAddresses(hostname, addressType, endpoints);
    if (!error.IsSuccess())
    {
        return error;
    }

    for (auto iter = endpoints.begin(); iter != endpoints.end(); ++ iter)
    {
        iter->Port = static_cast<unsigned short>(portValue);
    }

    return ErrorCode();
}

bool TcpTransportUtility::IsLocalEndpoint(Common::Endpoint const & endpoint)
{
    if (endpoint.IsLoopback())
    {
        return true;
    }

    // Is it one of the local addresses?
    vector<Endpoint> localAddresses;
    auto error = TcpTransportUtility::GetLocalAddresses(localAddresses, (ResolveOptions::Enum)endpoint.AddressFamily);
    if (!error.IsSuccess())
    {
        return false;
    }

    for (auto const & localAddress : localAddresses)
    {
        if (localAddress.EqualAddress(endpoint))
        {
            return true;
        }
    }

    return false;
}

_Use_decl_annotations_
ErrorCode TcpTransportUtility::ResolveToAddresses(string const & hostName, ResolveOptions::Enum options, vector<Endpoint> & addresses)
{
    addresses.clear();

    PADDRINFOW addressInfoList = nullptr;

    ADDRINFOW addressInfoHints = {0};
    addressInfoHints.ai_family = options;
    addressInfoHints.ai_socktype = SOCK_STREAM;
    addressInfoHints.ai_protocol = IPPROTO_TCP;

    // Call getaddrinfo(). If the call succeeds,
    // the aiList variable will hold a linked list
    // of addrinfo structures containing response
    // information about the host
    int error = GetAddrInfoW(hostName.c_str(), nullptr, &addressInfoHints, &addressInfoList);
    if (0 != error)
    {
        auto errNo =  errno;
        WriteError(
            Constants::TcpTrace,
            "ResolveToAddresses: GetAddrInfoW('{0}') failed: {1}",
            hostName,
            errNo);

        return ErrorCode::FromWin32Error(error);
    }

    unique_ptr<ADDRINFOW, AddrInfoWDeleter> addressInfoListUptr(addressInfoList);

    PADDRINFOW ptr = addressInfoList;

    while (ptr != nullptr)
    {
        Common::Endpoint endpoint(*ptr);
        auto duplicate = std::find(addresses.cbegin(), addresses.cend(), endpoint);
        if (duplicate == addresses.cend())
        {
            addresses.push_back(endpoint);
        }

        ptr = ptr->ai_next;
    }

    if (addresses.empty())
    {
        WriteError(
            Constants::TcpTrace,
            "ResolveToAddresses: GetAddrInfo returned success but no address was resolved from '{0}'",
            hostName);

        return ErrorCodeValue::OperationFailed;
    }

    return ErrorCode();
}

_Use_decl_annotations_ ErrorCode TcpTransportUtility::GetLocalAddresses(vector<Endpoint> & result, ResolveOptions::Enum options)
{
    WriteNoise(Constants::TcpTrace, "GetLocalAddresses({0})", options);
    result.clear();
    ErrorCode error;
    multimap<SCOPE_LEVEL, Endpoint, std::greater<SCOPE_LEVEL>> addrMap; //sort addresses by visibility

#ifdef PLATFORM_UNIX
    struct ifaddrs* addrStruct = NULL;
    if (getifaddrs(&addrStruct) < 0)
    {
        error = ErrorCode::FromErrno();
        WriteError(
            Constants::TcpTrace,
            "GetLocalAddresses: getifaddrs failed: {0}",
            error);

        return error;
    }

    KFinally([addrStruct] { freeifaddrs(addrStruct); });

    for(ifaddrs* iter = addrStruct; iter != NULL; iter = iter->ifa_next)
    {
        if (iter->ifa_addr == NULL) continue; 
        if ((iter->ifa_addr->sa_family != AF_INET) && (iter->ifa_addr->sa_family != AF_INET6)) continue;

        Endpoint ep(*(iter->ifa_addr));
        if (ep.IsIPv4() && (options != ResolveOptions::IPv6))
        {
            addrMap.emplace(make_pair(ep.IpScopeLevel(), ep));
            continue;
        }

        if (ep.IsIPv6() && (options != ResolveOptions::IPv4))
        {
            addrMap.emplace(make_pair(ep.IpScopeLevel(), ep));
            continue;
        }
    }
#else
    PMIB_UNICASTIPADDRESS_TABLE addressTable;
    NETIO_STATUS retval = GetUnicastIpAddressTable((ADDRESS_FAMILY)options, &addressTable);
    if (retval != NO_ERROR)
    {
        error = ErrorCode::FromWin32Error(retval);
        WriteError(
            Constants::TcpTrace,
            "GetLocalAddresses: GetUnicastIpAddressTable failed: {0}",
            error);

        return ErrorCode::FromWin32Error(retval);
    }

    KFinally([=] { FreeMibTable(addressTable); });

    for (ULONG i = 0; i < addressTable->NumEntries; ++i)
    {
        switch (options)
        {
        case ResolveOptions::Unspecified:
            if (addressTable->Table[i].Address.si_family == AF_INET)
            {
                Endpoint ep((sockaddr const &)addressTable->Table[i].Address.Ipv4);
                addrMap.emplace(make_pair(ep.IpScopeLevel(), ep));
            }
            else
            {
                KInvariant(addressTable->Table[i].Address.si_family == AF_INET6);
                Endpoint ep((sockaddr const &)addressTable->Table[i].Address.Ipv6);
                addrMap.emplace(make_pair(ep.IpScopeLevel(), ep));
            }
            break;

        case ResolveOptions::IPv4:
            if (addressTable->Table[i].Address.si_family == AF_INET)
            {
                Endpoint ep((sockaddr const &)addressTable->Table[i].Address.Ipv4);
                addrMap.emplace(make_pair(ep.IpScopeLevel(), ep));
            }
            break;

        case ResolveOptions::IPv6:
            if (addressTable->Table[i].Address.si_family == AF_INET6)
            {
                Endpoint ep((sockaddr const &)addressTable->Table[i].Address.Ipv6);
                addrMap.emplace(make_pair(ep.IpScopeLevel(), ep));
            }
            break;

        default:
            return ErrorCodeValue::InvalidArgument;
        }
    }
#endif

    // Addresses is sorted in the following order:
    // global -> site local -> link local -> loopback
    for (auto & addrEntry : addrMap)
    {
        WriteNoise(Constants::TcpTrace, "GetLocalAddresses: {0}, scopeLevel={1}", addrEntry.second, (uint)addrEntry.first);
        result.emplace_back(move(addrEntry.second));
    }

    return error;
}

_Use_decl_annotations_ ErrorCode TcpTransportUtility::GetFirstLocalAddress(Endpoint & localAddress, ResolveOptions::Enum options)
{
    vector<Endpoint> localAddresses;
    auto error = GetLocalAddresses(localAddresses, options);
    if (!error.IsSuccess()) { return error; }

    if (localAddresses.empty())
    {
        WriteWarning(Constants::TcpTrace, "No local addresses found");

        return ErrorCodeValue::InvalidAddress;
    }

    for (auto const & endpoint : localAddresses)
    {
        if (!endpoint.IsLoopback())
        {
            localAddress = endpoint;

            return ErrorCodeValue::Success;
        }
    }

    WriteWarning(Constants::TcpTrace, "No non-loopback addresses found");

    return ErrorCodeValue::InvalidAddress;
}

Endpoint TcpTransportUtility::ParseEndpointString(string const & address)
{
    Endpoint endpoint;

    if (TcpTransportUtility::TryParseEndpointString(address, endpoint).IsSuccess())
    {
        return endpoint;
    }

    Common::Assert::CodingError("Supplied ip address is invalid: {0}", address);
}

std::string TcpTransportUtility::ParseHostString(string const & address)
{
    string host;
    string port;
    if (TcpTransportUtility::TryParseHostPortString(address, host, port).IsSuccess())
    {
        return host;
    }

    Common::Assert::CodingError("Supplied ip address is invalid: {0}", address);
}

USHORT TcpTransportUtility::ParsePortString(string const & address)
{
    string host;
    string port;
    if (TcpTransportUtility::TryParseHostPortString(address, host, port).IsSuccess())
    { 
        USHORT clientConnectionPort = 0;
        if (TryParsePortString(port, clientConnectionPort))
        {
            return clientConnectionPort;
        }
    }

    Common::Assert::CodingError("Supplied ip address is invalid: {0}", address);
}

ErrorCode TcpTransportUtility::TryParseEndpointString(std::string const & address, Common::Endpoint & endpoint)
{
    string host;
    string port;

    ErrorCode error = TryParseHostPortString(address, host, port);
    if (!error.IsSuccess())
    {
        return error;
    }

    USHORT portLong;
    if (!TryParsePortString(port, portLong))
    {
        WriteError(
            Constants::TcpTrace, "TryParseEndpointString: cannot convert '{0}' in address '{1}' to tcp port",
            port, address);

        return ErrorCodeValue::InvalidAddress;
    }

    error  = Endpoint::TryParse(host, endpoint);
    if (!error.IsSuccess())
    {
        return error;
    }

    endpoint.Port = static_cast<USHORT>(portLong);
    return ErrorCodeValue::Success;;
}

ErrorCode TcpTransportUtility::TryParseHostPortString(std::string const & address, std::string & host, std::string & port)
{
    size_t startAddress = address.find("[");
    size_t endAddress;
    size_t portAddress;

    if (startAddress == string::npos) 
    {
        startAddress = 0;
        endAddress = address.find(":");
        portAddress = endAddress + 1;
    }
    else
    {
        ++startAddress;
        endAddress = address.find("]:");
        portAddress = endAddress + 2;
    }

    if (endAddress == string::npos)
    {
        return ErrorCodeValue::InvalidAddress;
    }

    host = address.substr(startAddress, endAddress - startAddress);
    port = address.substr(portAddress);

    return ErrorCodeValue::Success;;
}

_Use_decl_annotations_
bool TcpTransportUtility::TryParsePortString(std::string const & portString, USHORT & port)
{
    int portLong = 0;
    if (!(wistringstream(portString) >> portLong))
    {
        return false;
    }

    port = (USHORT)portLong;
    return (0 <= portLong) && (portLong <= numeric_limits<USHORT>::max());
}

bool TcpTransportUtility::IsValidEndpointString(std::string const & address)
{
    Endpoint endpoint;
    return TcpTransportUtility::TryParseEndpointString(address, endpoint).IsSuccess();
}

void TcpTransportUtility::EnableTcpFastLoopbackIfNeeded(
    Socket & socket,
    string const & traceId)
{
    if (!TransportConfig::GetConfig().TcpFastLoopbackEnabled)
    {
        WriteInfo(Constants::TcpTrace, traceId, "TCP fast loopback is disabled");
        return;
    }

    auto error = socket.EnableFastLoopback();
    WriteInfo(Constants::TcpTrace, traceId, "enable TCP fast loopback: {0}", error);
}

string TcpTransportUtility::ConstructAddressString(
    string const & ipAddressOrFQDN,
    uint port)
{
    Endpoint endpoint;
    ErrorCode error = Endpoint::TryParse(ipAddressOrFQDN, endpoint);
    if (!error.IsSuccess())
    {
        // domain is hostname
        string result;
        StringWriter(result).Write("{0}:{1}", ipAddressOrFQDN, port);
        return result;
    }

    endpoint.Port = (unsigned short) port;
    return endpoint.ToString();
}

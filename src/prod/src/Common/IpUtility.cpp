// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"
#if defined(PLATFORM_UNIX)
#include <ifaddrs.h>
#include <resolv.h>
#include <arpa/inet.h>
#endif

using namespace std;
using namespace Common;

ErrorCode IpUtility::GetDnsServers(
    __out std::vector<std::string> & list
    )
{
#if !defined(PLATFORM_UNIX)
    ByteBuffer buffer;
    ULONG bufferSize = 0;

    DWORD error = GetNetworkParams((FIXED_INFO*)buffer.data(), /*inout*/&bufferSize);
    if (error == ERROR_BUFFER_OVERFLOW)
    {
        buffer.resize(bufferSize);
        error = GetNetworkParams((FIXED_INFO*)buffer.data(), /*inout*/&bufferSize);
    }

    if (error != ERROR_SUCCESS)
    {
        return ErrorCode::FromWin32Error(error);
    }

    FIXED_INFO* pFixedInfo = (FIXED_INFO*)buffer.data();
    ASSERT_IFNOT(pFixedInfo != nullptr, "Expected a valid FIXED_INFO buffer");

    IP_ADDR_STRING* pIPAddr = &pFixedInfo->DnsServerList;
    while (pIPAddr != nullptr)
    {
        std::string address(pIPAddr->IpAddress.String);
        std::string addressW;
        Utf8ToUtf16NotNeeded2(address, addressW);

        list.push_back(addressW);

        pIPAddr = pIPAddr->Next;
    }
#else
    struct __res_state state;
    int error = res_ninit(/*out*/&state);
    if (error == -1)
    {
        return ErrorCode::FromWin32Error(errno);
    }

    for (int i = 0; i < state.nscount; i++)
    {
        SOCKADDR_IN& addr = state.nsaddr_list[i];

        char address[INET_ADDRSTRLEN];
        Invariant(inet_ntop(AF_INET, &addr.sin_addr, address, sizeof(address)));

        std::string addressW;
        Utf8ToUtf16NotNeeded2(address, addressW);

        list.push_back(addressW);
    }
#endif

    return ErrorCode::Success();
}

ErrorCode IpUtility::GetDnsServersPerAdapter(
    __out std::map<std::string, std::vector<std::string>> & map
    )
{
#if !defined(PLATFORM_UNIX)
    const DWORD flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_UNICAST;

    ByteBuffer buffer;
    ULONG bufferSize = 0;
    DWORD error = GetAdaptersAddresses(AF_INET, flags, NULL, (IP_ADAPTER_ADDRESSES*)buffer.data(), &bufferSize);
    if (error == ERROR_BUFFER_OVERFLOW)
    {
        buffer.resize(bufferSize);
        error = GetAdaptersAddresses(AF_INET, flags, NULL, (IP_ADAPTER_ADDRESSES*)buffer.data(), &bufferSize);
    }

    if (error != ERROR_SUCCESS)
    {
        return ErrorCode::FromWin32Error(error);
    }

    IP_ADAPTER_ADDRESSES* pCurrAddresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
    while (pCurrAddresses != NULL)
    {
        std::string adapterName = Utf8ToUtf16NotNeeded(pCurrAddresses->AdapterName);
        std::vector<std::string> addresses;

        IP_ADAPTER_DNS_SERVER_ADDRESS* pDnsServer = pCurrAddresses->FirstDnsServerAddress;
        while (pDnsServer != NULL)
        {
            SOCKADDR_IN& dns = (SOCKADDR_IN&)*pDnsServer->Address.lpSockaddr;

            CHAR address[INET_ADDRSTRLEN];
            InetNtop(AF_INET, &dns.sin_addr, address, ARRAYSIZE(address));
            addresses.push_back(address);

            pDnsServer = pDnsServer->Next;
        }

        if (!addresses.empty())
        {
            map.insert(std::make_pair(std::move(adapterName), std::move(addresses)));
        }

        pCurrAddresses = pCurrAddresses->Next;
    }
#endif
    return ErrorCode::Success();
}

ErrorCode IpUtility::GetIpAddressesPerAdapter(
    __out std::map<std::string, std::vector<Common::IPPrefix>> & map
    )
{
#if !defined(PLATFORM_UNIX)
    const DWORD flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;

    ByteBuffer buffer;
    ULONG bufferSize = 0;
    DWORD error = GetAdaptersAddresses(AF_INET, flags, NULL, (IP_ADAPTER_ADDRESSES*)buffer.data(), &bufferSize);
    if (error == ERROR_BUFFER_OVERFLOW)
    {
        buffer.resize(bufferSize);
        error = GetAdaptersAddresses(AF_INET, flags, NULL, (IP_ADAPTER_ADDRESSES*)buffer.data(), &bufferSize);
    }

    if (error != ERROR_SUCCESS)
    {
        return ErrorCode::FromWin32Error(error);
    }

    IP_ADAPTER_ADDRESSES* pCurrAddresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
    while (pCurrAddresses != NULL)
    {
        std::string adapterName = Utf8ToUtf16NotNeeded(pCurrAddresses->AdapterName);
        std::vector<IPPrefix> addresses;

        IP_ADAPTER_UNICAST_ADDRESS* pUnicast = pCurrAddresses->FirstUnicastAddress;
        while (pUnicast != NULL)
        {
            Endpoint ep(pUnicast->Address);
            IPPrefix ipPrefix(ep, pUnicast->OnLinkPrefixLength);

            addresses.push_back(ipPrefix);

            pUnicast = pUnicast->Next;
        }

        if (!addresses.empty())
        {
            map.insert(std::make_pair(std::move(adapterName), std::move(addresses)));
        }

        pCurrAddresses = pCurrAddresses->Next;
    }
#else
    struct ifaddrs *ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1)
    {
        return ErrorCode::FromWin32Error(errno);
    }

    ifa = ifaddr;
    while (ifa != nullptr)
    {
        if ((ifa->ifa_addr != nullptr) && (ifa->ifa_addr->sa_family == AF_INET) && (ifa->ifa_netmask != nullptr))
        {
            SOCKADDR_IN& subnetmask = (SOCKADDR_IN&)*ifa->ifa_netmask;
            ULONG mask = subnetmask.sin_addr.s_addr;
            int prefix = 0;
            // Convert subnet mask to prefix
            for (int i = 0; i < 32; i++)
            {
                if (mask & (1 << i))
                {
                    prefix++;
                }
            }

            std::vector<IPPrefix> addresses;

            Endpoint ep(*(ifa->ifa_addr));
            IPPrefix ipPrefix(ep, prefix);
            addresses.push_back(ipPrefix);

            std::string adapterName = Utf8ToUtf16NotNeeded(ifa->ifa_name);

            if (!addresses.empty())
            {
                map.insert(std::make_pair(std::move(adapterName), std::move(addresses)));
            }
        }

        ifa = ifa->ifa_next;
    }

    freeifaddrs(ifaddr);
#endif
    return ErrorCode::Success();
}

ErrorCode IpUtility::GetAdapterAddressOnTheSameNetwork(
    __in std::string input,
    __out std::string& output
    )
{
    Common::Endpoint inputEp;
    ErrorCode error = Common::Endpoint::TryParse(input, /*out*/inputEp);
    if (!error.IsSuccess())
    {
        return error;
    }

    typedef std::map<std::string, std::vector<IPPrefix>> IpMap;

    IpMap map;
    error = GetIpAddressesPerAdapter(map);
    if (!error.IsSuccess())
    {
        return error;
    }

    for (IpMap::iterator it = map.begin(); it != map.end(); ++it)
    {
        const std::vector<IPPrefix> & ipAddresses = it->second;

        for (int i = 0; i < ipAddresses.size(); i++)
        {
            const IPPrefix& ip = ipAddresses[i];
            if (inputEp.EqualPrefix(ip.GetAddress(), ip.GetPrefixLength()))
            {
                output = ip.GetAddress().GetIpString();
                return ErrorCode::Success();
            }
        }
    }

    return ErrorCode::FromWin32Error(ERROR_NOT_FOUND);
}

ErrorCode IpUtility::GetGatewaysPerAdapter(
    __out map<string, vector<string>> &map
    )
{
#if !defined(PLATFORM_UNIX)
    const DWORD flags = GAA_FLAG_INCLUDE_GATEWAYS;

    ByteBuffer buffer;
    ULONG bufferSize = 0;
    DWORD error = GetAdaptersAddresses(AF_INET, flags, NULL, (IP_ADAPTER_ADDRESSES*)buffer.data(), &bufferSize);
    if (error == ERROR_BUFFER_OVERFLOW)
    {
        buffer.resize(bufferSize);
        error = GetAdaptersAddresses(AF_INET, flags, NULL, (IP_ADAPTER_ADDRESSES*)buffer.data(), &bufferSize);
    }

    if (error != ERROR_SUCCESS)
    {
        return ErrorCode::FromWin32Error(error);
    }

    IP_ADAPTER_ADDRESSES* pCurrAddress = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
    while (pCurrAddress)
    {
        std::string adapterName = Utf8ToUtf16NotNeeded(pCurrAddress->AdapterName);
        std::vector<std::string> addresses;

        PIP_ADAPTER_GATEWAY_ADDRESS_LH gatewayAddress = pCurrAddress->FirstGatewayAddress;
        while (gatewayAddress)
        {
            SOCKADDR_IN& sockAddr = (SOCKADDR_IN&)*gatewayAddress->Address.lpSockaddr;
            CHAR ip[16];
            InetNtop(AF_INET, &sockAddr.sin_addr, ip, ARRAYSIZE(ip));
            addresses.push_back(ip);

            gatewayAddress = gatewayAddress->Next;
        }

        if (!addresses.empty())
        {
            map.insert(std::make_pair(std::move(adapterName), std::move(addresses)));
        }

        pCurrAddress = pCurrAddress->Next;
    }

    return ErrorCodeValue::Success;

#else if

    return ErrorCodeValue::NotImplemented;

#endif
}

ErrorCode IpUtility::GetFirstNonLoopbackAddress(
    std::map<std::string, std::vector<Common::IPPrefix>> addressesPerAdapter,
    __out string & nonLoopbackIp)
{
    std::regex loopbackaddrIPv4("(127.0.0.)(.*)", std::regex::ECMAScript);
    std::regex loopbackaddrIPv6("(0000:0000:0000:0000:0000:0000:0000)(.*)", std::regex::ECMAScript);

    for (map<string, vector<Common::IPPrefix>>::const_iterator it = addressesPerAdapter.begin(); it != addressesPerAdapter.end(); ++it)
    {
        for (vector<Common::IPPrefix>::const_iterator iit = it->second.begin(); iit != it->second.end(); ++iit)
        {
            auto address = iit->GetAddress().GetIpString();
            if (!regex_match(address, loopbackaddrIPv4) && !regex_match(address, loopbackaddrIPv6))
            {
                nonLoopbackIp = address;
                return ErrorCodeValue::Success;
            }
        }
    }

    return ErrorCodeValue::NotFound;
}

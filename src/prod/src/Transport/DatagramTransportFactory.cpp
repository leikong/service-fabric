// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace Transport;
using namespace Common;
using namespace std;

IDatagramTransportSPtr DatagramTransportFactory::CreateTcp(
    string const & address,
    string const & id,
    string const & owner)
{
    if (TransportConfig::GetConfig().InMemoryTransportEnabled)
    {
        return CreateMem(address, id);
    }

    return TcpDatagramTransport::Create(address, id, owner);
}

IDatagramTransportSPtr DatagramTransportFactory::CreateTcpClient(string const & id, string const & owner)
{
    if (TransportConfig::GetConfig().InMemoryTransportEnabled)
    {
        return CreateMem("", id);
    }

    return TcpDatagramTransport::CreateClient(id, owner);
}

IDatagramTransportSPtr DatagramTransportFactory::CreateMem(string const & name, string const & id)
{
    string address;
    if (StringUtility::StartsWith(name, "localhost"))
    {
        address = "127.0.0.1" + name.substr(9);
    }
    else
    {
        address = name;
    }

    return make_shared<MemoryTransport>(address, id.empty() ? address : id);
}

IDatagramTransportSPtr DatagramTransportFactory::CreateUnreliable(
    ComponentRoot const & root,
    IDatagramTransportSPtr const & innerTransport)
{
    return make_shared<UnreliableTransport>(root, innerTransport);
}

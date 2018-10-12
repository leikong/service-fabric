// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Transport
{
    class DatagramTransportFactory
    {
    public:
        static IDatagramTransportSPtr CreateTcp(
            std::string const & address,
            std::string const & id = "",
            std::string const & owner = "");

        static IDatagramTransportSPtr CreateTcpClient(
            std::string const & id = "",
            std::string const & owner = "");

        // Returns an empty pointer if the local address already exists
        static IDatagramTransportSPtr  CreateMem(
            std::string const & name,
            std::string const & id = "");

        static IDatagramTransportSPtr CreateUnreliable(
            Common::ComponentRoot const & root,
            IDatagramTransportSPtr const & innerTransport);
    };
}

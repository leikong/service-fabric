// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Transport
{
    struct NamedAddress
    {
        NamedAddress()
            :   Address(""),
                Name("")
        {
        }

        NamedAddress(std::string const & address)
            :   Address(address),
                Name("")
        {
        }

        NamedAddress(std::string const & address, std::string const & name)
            :   Address(address),
                Name(name)
        {
        }

        std::string Address;
        std::string Name;
    };
}

// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Transport
{
    class ClaimsMessage : public Serialization::FabricSerializable
    {
    public:
        ClaimsMessage();
        ClaimsMessage(std::string const & claims);

        static MessageUPtr CreateClaimsMessage(std::string const& token);

        std::string const & Claims() const;

        void WriteTo(Common::TextWriter & w, Common::FormatOptions const &) const;

        FABRIC_FIELDS_01(claims_);

    private:
        std::string claims_;
    };
}

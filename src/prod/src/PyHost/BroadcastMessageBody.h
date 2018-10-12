// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace PyHost
{
    class BroadcastMessageBody : public Serialization::FabricSerializable
    {
    private:
        BroadcastMessageBody();
        BroadcastMessageBody(std::string const &);

    public:
        BroadcastMessageBody(BroadcastMessageBody &&) = default;

        static Transport::MessageUPtr CreateMessage(std::string const &);
        static BroadcastMessageBody DeserializeBody(Transport::MessageUPtr &);

        std::string const & GetContents() const;

        FABRIC_FIELDS_01( contents_ )

    private:
        std::string contents_;
    };
}

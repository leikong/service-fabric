// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Transport
{
    class ActionHeader : public MessageHeader<MessageHeaderId::Action>, public Serialization::FabricSerializable
    {
        DEFAULT_COPY_ASSIGNMENT(ActionHeader)
    
    public:
        ActionHeader();
        ActionHeader(std::string const & action);
        ActionHeader(std::string && action);
        ActionHeader(ActionHeader const & rhs);
        ActionHeader(ActionHeader && rhs);

        __declspec(property(get=get_Action)) std::string const & Action;
        std::string const & get_Action() const;

        void WriteTo(Common::TextWriter & w, Common::FormatOptions const &) const;

        FABRIC_FIELDS_01(action_);

    private:
        std::string action_;
    };
}

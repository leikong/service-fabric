// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once 

namespace ServiceModel
{
    class Claim : public Serialization::FabricSerializable
    {
    public:
        Claim();
            
        Claim(
            std::string const& claimType,
            std::string const& issuer,
            std::string const& originalIssuer,
            std::string const& subject,
            std::string const& value,
            std::string const& valueType);

        __declspec(property(get=get_ClaimType, put=set_ClaimType)) std::string const & ClaimType;
        std::string const & get_ClaimType() const { return claimType_; }
        void set_ClaimType(std::string const & value) { claimType_ = value; }

        __declspec(property(get=get_Issuer, put=set_Issuer)) std::string const & Issuer;
        std::string const & get_Issuer() const { return issuer_; }
        void set_Issuer(std::string const & value) { issuer_ = value; }

        __declspec(property(get=get_OriginalIssuer, put=set_OriginalIssuer)) std::string const & OriginalIssuer;
        std::string const & get_OriginalIssuer() const { return originalIssuer_; }
        void set_OriginalIssuer(std::string const & value) { originalIssuer_ = value; }

        __declspec(property(get=get_Subject, put=set_Subject)) std::string const & Subject;
        std::string const & get_Subject() const { return subject_; }
        void set_Subject(std::string const & value) { claimType_ = value; }

        __declspec(property(get=get_Value, put=set_Value)) std::string const & Value;
        std::string const & get_Value() const { return value_; }
        void set_Value(std::string const & value) { value_ = value; }

        __declspec(property(get=get_ValueType, put=set_ValueType)) std::string const & ValueType;
        std::string const & get_ValueType() const { return valueType_; }
        void set_ValueType(std::string const & value) { valueType_ = value; }

        void WriteTo(Common::TextWriter & w, Common::FormatOptions const &) const;

        FABRIC_FIELDS_06(claimType_, issuer_, originalIssuer_, subject_, value_, valueType_);

    private:

        std::string claimType_;
        std::string issuer_;
        std::string originalIssuer_;
        std::string subject_;
        std::string value_;
        std::string valueType_;
    };
}

DEFINE_USER_ARRAY_UTILITY(ServiceModel::Claim);

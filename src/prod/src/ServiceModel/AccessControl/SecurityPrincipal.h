// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace AccessControl
{
    namespace SecurityPrincipalType
    {
        enum Enum
        {
            Invalid = FABRIC_SECURITY_PRINCIPAL_IDENTIFIER_KIND_INVALID,
            X509 = FABRIC_SECURITY_PRINCIPAL_IDENTIFIER_KIND_X509,
            Windows = FABRIC_SECURITY_PRINCIPAL_IDENTIFIER_KIND_WINDOWS,
            Claim = FABRIC_SECURITY_PRINCIPAL_IDENTIFIER_KIND_CLAIM,
            Role = FABRIC_SECURITY_PRINCIPAL_IDENTIFIER_KIND_ROLE
        };

        bool IsValid(Enum value);
        void WriteToTextWriter(Common::TextWriter & w, Enum const & e);

        DECLARE_ENUM_STRUCTURED_TRACE(SecurityPrincipalType);
    }

    class SecurityPrincipal
        : public Serialization::FabricSerializable
        , public Common::TextTraceComponent<Common::TraceTaskCodes::AccessControl>
    {
    public:
        typedef std::shared_ptr<SecurityPrincipal> SPtr;

        static Common::ErrorCode FromPublic(
            FABRIC_SECURITY_PRINCIPAL_IDENTIFIER const * input,
            _Out_ SPtr & result);

        virtual void ToPublic(
            Common::ScopedHeap & heap,
            _Out_ FABRIC_SECURITY_PRINCIPAL_IDENTIFIER & securityPrincipal) = 0;

        Common::ErrorCode Status() const;

        virtual bool CheckCommonName(std::string const & commonName) const;
        virtual bool CheckClaim(Transport::SecuritySettings::RoleClaims const & claims) const;
        virtual bool CheckTokenMembership(HANDLE token) const;

        virtual SecurityPrincipalType::Enum Type() const = 0;
        static Serialization::IFabricSerializable * FabricSerializerActivator(
            Serialization::FabricTypeInformation typeInformation);

        void WriteTo(Common::TextWriter & w, Common::FormatOptions const &) const;
        static std::string AddField(Common::TraceEvent & traceEvent, std::string const & name);
        void FillEventData(Common::TraceEventContext & context) const;

    protected:
        Common::ErrorCode error_;

    private:
        const static SecurityPrincipalType::Enum type_ = SecurityPrincipalType::Invalid;
    };

#define DECLARE_PRINCIPAL_IDENTIFIER_TYPE(TypeEnum) \
public: \
    void ToPublic( \
        Common::ScopedHeap & heap, \
        _Out_ FABRIC_SECURITY_PRINCIPAL_IDENTIFIER & securityPrincipal) override; \
    SecurityPrincipalType::Enum Type() const override; \
    NTSTATUS GetTypeInformation(_Out_ Serialization::FabricTypeInformation & typeInformation) const override; \
private: \
    const static SecurityPrincipalType::Enum type_ = TypeEnum;

#define DEFINE_PRINCIPAL_IDENTIFIER_TYPE(TypeName) \
    SecurityPrincipalType::Enum TypeName::Type() const { return type_; } \
    _Use_decl_annotations_ NTSTATUS TypeName::GetTypeInformation( \
        Serialization::FabricTypeInformation & typeInformation) const \
    { \
        typeInformation.buffer = reinterpret_cast<UCHAR const *>(&type_); \
        typeInformation.length = sizeof(type_); \
        return STATUS_SUCCESS; \
    }

    class SecurityPrincipalX509 : public SecurityPrincipal
    {
    public:
        SecurityPrincipalX509();
        SecurityPrincipalX509(std::string const & commonName);

        bool CheckCommonName(std::string const & commonName) const override;
        std::string const & CommonName() const;

        FABRIC_FIELDS_01(commonName_)
        DECLARE_PRINCIPAL_IDENTIFIER_TYPE(SecurityPrincipalType::X509)

    private:
        std::string commonName_;
    };

    class SecurityPrincipalClaim : public SecurityPrincipal
    {
    public:
        SecurityPrincipalClaim();
        SecurityPrincipalClaim(std::string const & claim);

        bool CheckClaim(Transport::SecuritySettings::RoleClaims const & claims) const;
        std::string Claim() const;

        FABRIC_FIELDS_02(claimType_, claimValue_)
        DECLARE_PRINCIPAL_IDENTIFIER_TYPE(SecurityPrincipalType::Claim)

    private:
        static Common::ErrorCode TryParseClaim(
            std::string const & claim,
            std::string & claimType,
            std::string & claimValue);

    private:
        std::string claimType_;
        std::string claimValue_;
    };

    class SecurityPrincipalWindows : public SecurityPrincipal
    {
    public:
        SecurityPrincipalWindows();
        SecurityPrincipalWindows(std::string const & accountName);

        bool CheckTokenMembership(HANDLE token) const override;
        Common::ErrorCode GetAccountName(_Out_ std::string & accountName) const;

        FABRIC_FIELDS_01(sid_);
        DECLARE_PRINCIPAL_IDENTIFIER_TYPE(SecurityPrincipalType::Windows)

    private:
        Common::ByteBuffer sid_;
    };
}

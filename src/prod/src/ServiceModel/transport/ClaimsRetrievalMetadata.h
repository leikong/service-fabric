// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Transport
{
    class ClaimsRetrievalMetadata : public Serialization::FabricSerializable, public Common::IFabricJsonSerializable
    {
        DENY_COPY(ClaimsRetrievalMetadata)

    public:

        static Common::Global<std::string> const InvalidMetadataValue;

        static Common::Global<std::string> const MetadataType_AAD;
        static Common::Global<std::string> const AAD_LoginUri;
        static Common::Global<std::string> const AAD_AuthorityUri;
        static Common::Global<std::string> const AAD_TenantId;
        static Common::Global<std::string> const AAD_ClusterApplication;
        static Common::Global<std::string> const AAD_ClientApplication;
        static Common::Global<std::string> const AAD_ClientRedirectUri;

    private:

        ClaimsRetrievalMetadata(std::string const & type);

    public:

        ClaimsRetrievalMetadata();

        ClaimsRetrievalMetadata(ClaimsRetrievalMetadata &&);

        virtual ~ClaimsRetrievalMetadata() { }

        static ClaimsRetrievalMetadata CreateForAAD();

        __declspec (property(get=get_IsValid)) bool IsValid;

        __declspec (property(get=get_IsAAD)) bool IsAAD;
        __declspec (property(get=get_AADLogin)) std::string const & AAD_Login;
        __declspec (property(get=get_AADAuthority)) std::string const & AAD_Authority;
        __declspec (property(get=get_AADTenant)) std::string const & AAD_Tenant;
        __declspec (property(get=get_AADCluster)) std::string const & AAD_Cluster;
        __declspec (property(get=get_AADClient)) std::string const & AAD_Client;
        __declspec (property(get=get_AADRedirect)) std::string const & AAD_Redirect;

        bool get_IsValid() const;
        bool get_IsAAD() const;
        std::string const & get_AADLogin() const;
        std::string const & get_AADAuthority() const;
        std::string const & get_AADTenant() const;
        std::string const & get_AADCluster() const;
        std::string const & get_AADClient() const;
        std::string const & get_AADRedirect() const;

        void WriteTo(Common::TextWriter&, Common::FormatOptions const&) const;

        void ToPublicApi(
            __in Common::ScopedHeap &,
            __out FABRIC_CLAIMS_RETRIEVAL_METADATA &) const;

        FABRIC_FIELDS_02(type_, metadata_);

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("type", type_)
            SERIALIZABLE_PROPERTY_SIMPLE_MAP("metadata", metadata_)
        END_JSON_SERIALIZABLE_PROPERTIES()

    private:

        void SetMetadataValue(std::string const & key, std::string const & value);
        std::string const & GetMetadataValue(std::string const & key) const;

        std::string type_;
        std::map<std::string, std::string> metadata_;
    };

    typedef std::shared_ptr<ClaimsRetrievalMetadata> ClaimsRetrievalMetadataSPtr;
}

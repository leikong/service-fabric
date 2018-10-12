// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Transport 
{
    namespace SecuritySettingsNames
    {
        std::string const CredentialType = "CredentialType";

        std::string const AllowedCommonNames = "AllowedCommonNames";
        std::string const IssuerThumbprints = "IssuerThumbprints";
        std::string const RemoteCertThumbprints = "RemoteCertThumbprints";
        std::string const FindType = "FindType";
        std::string const FindValue = "FindValue";
        std::string const FindValueSecondary = "FindValueSecondary";
        std::string const StoreLocation = "StoreLocation";
        std::string const StoreName = "StoreName";
        std::string const ProtectionLevel = "ProtectionLevel";

        // Kerberos transport security settings:
        std::string const ServicePrincipalName = "ServicePrincipalName"; // Service principal name of all replication listeners for this service group
        std::string const WindowsIdentities = "WindowsIdentities"; // Used to authorize incoming replication connections for this service group

        std::string const ApplicationIssuerStorePrefix = "ApplicationIssuerStore/";
    }

    class ComSecurityCredentialsResult
        : public Common::TextTraceComponent<Common::TraceTaskCodes::Transport>,
        public IFabricSecurityCredentialsResult,
        protected Common::ComUnknownBase
    {
        DENY_COPY(ComSecurityCredentialsResult)

        COM_INTERFACE_LIST1(
        ComSecurityCredentialsResult,
        IID_IFabricSecurityCredentialsResult,
        IFabricSecurityCredentialsResult)

    public:
        static HRESULT FromConfig(
            __in Common::ComPointer<IFabricCodePackageActivationContext> codePackageActivationContextCPtr,
            __in std::string const & configurationPackageName,
            __in std::string const & sectionName,
            __out IFabricSecurityCredentialsResult ** securityCredentialsResult);

        static HRESULT ClusterSecuritySettingsFromConfig(
            __out IFabricSecurityCredentialsResult ** securityCredentialsResult);
        
        ComSecurityCredentialsResult(SecuritySettings const & value);
        virtual ~ComSecurityCredentialsResult();
        const FABRIC_SECURITY_CREDENTIALS * STDMETHODCALLTYPE get_SecurityCredentials();

        static HRESULT STDMETHODCALLTYPE ReturnSecurityCredentialsResult(
            SecuritySettings && settings,
            IFabricSecurityCredentialsResult ** value);

    private:
        Common::ScopedHeap heap_;
        Common::ReferencePointer<FABRIC_SECURITY_CREDENTIALS> credentials_;
    };
}

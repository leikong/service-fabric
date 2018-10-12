// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    struct X509Default
    {
        static X509StoreLocation::Enum StoreLocation();
        static enum FABRIC_X509_STORE_LOCATION StoreLocation_Public();
        static string const & StoreName();
    };

    class CryptoUtility : TextTraceComponent<TraceTaskCodes::Common>
    {
    public:
        static ErrorCode GetCertificate(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            std::shared_ptr<X509FindValue> const & findValue,
            _Out_ CertContextUPtr & certContext);

        static ErrorCode GetCertificate(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            DWORD storeOpenFlag,
            std::shared_ptr<X509FindValue> const & findValue,
            _Out_ CertContextUPtr & certContext);

        static ErrorCode GetCertificate(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            std::shared_ptr<X509FindValue> const & findValue,
            _Out_ CertContexts & certContexts);

        static ErrorCode GetCertificate(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            DWORD storeOpenFlag,
            std::shared_ptr<X509FindValue> const & findValue,
            _Out_ CertContexts & certContexts);

        static ErrorCode GetCertificate(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            std::string const & findType,
            std::string const & findValue,
            _Out_ CertContextUPtr & certContext);

        static ErrorCode GetCertificate(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            std::string const & findType,
            std::string const & findValue,
            _Out_ CertContexts & certContexts);

        static ErrorCode GetCertificate(
            std::string const & certFilePath,
            _Out_ CertContextUPtr & certContext);

        static ErrorCode GetCertificateChain(
            PCCertContext certContext,
            _Out_ CertChainContextUPtr & certChain);

        static ErrorCode GetCertificateIssuerPubKey(
            PCCertContext certContext,
            _Out_ X509Identity::SPtr & issuerPubKey);

        static bool IsCertificateSelfSigned(PCCERT_CHAIN_CONTEXT certChain);

        static bool GetCertificateIssuerChainIndex(PCCERT_CHAIN_CONTEXT certChain, size_t & issuerCertIndex);
        static PCCertContext GetCertificateIssuer(PCCERT_CHAIN_CONTEXT certChain);

        static Common::ErrorCode GetCertificateExpiration(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            std::shared_ptr<X509FindValue> const & findValue,
            _Out_ Common::DateTime & expiration);

        static Common::ErrorCode GetCertificateExpiration(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            std::string const & findType,
            std::string const & findValue,
            _Out_ Common::DateTime & expiration);

        static Common::ErrorCode GetCertificateExpiration(
            CertContextUPtr const & cert,
            _Out_ Common::DateTime & expiration);

        static Common::ErrorCode GetCertificateCommonName(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            std::shared_ptr<X509FindValue> const & findValue,
            _Out_ std::string & commonName);

        static Common::ErrorCode GetCertificateCommonName(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            std::string const & findType,
            std::string const & findValue,
            _Out_ std::string & commonName);

        static Common::ErrorCode GetCertificateCommonName(
            PCCertContext certContext,
            _Out_ std::string & commonName);

        static Common::ErrorCode GetCertificatePrivateKeyFileName(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            std::shared_ptr<X509FindValue> const & findValue,
            std::string & privateKeyFileName);

        static Common::ErrorCode GetCertificatePrivateKeyFileName(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            std::shared_ptr<X509FindValue> const & findValue,
            std::vector<std::string> & privateKeyFileName);

        static Common::ErrorCode GetCertificateIssuerThumbprint(
            PCCERT_CONTEXT certContext,
            Thumbprint::SPtr & issuerThumbprint);

        static Common::ErrorCode CryptoUtility::GenerateExportableKey(
            string const & keyContainerName,
            _Out_ SecureString & key);

        static Common::ErrorCode CryptoUtility::CreateCertFromKey(
            ByteBuffer const & buffer,
            string const & key,
            _Out_ Common::CertContextUPtr & cert);

        static Common::ErrorCode CryptoUtility::ImportCertKey(
            string const& keyContainerName,
            string const& key);

        static Common::ErrorCode CreateSelfSignedCertificate(
            std::string const & subjectName,
            std::string const & keyContainerName,
            _Out_ Common::CertContextUPtr & cert);

        static Common::ErrorCode CreateSelfSignedCertificate(
            std::string const & subjectName,
            _Out_ Common::CertContextUPtr & cert);

        static Common::ErrorCode CreateSelfSignedCertificate(
            std::string const & subjectName,
            Common::DateTime expiration,
            _Out_ Common::CertContextUPtr & cert);

        static Common::ErrorCode CreateSelfSignedCertificate(
            std::string const & subjectName,
            const std::vector<std::string> *subjectAltNames,
            DateTime expiration,
            std::string const & keyContainerName,
            _Out_ CertContextUPtr & cert);

        static Common::ErrorCode CreateSelfSignedCertificate(
            std::string const & subjectName,
            Common::DateTime expiration,
            std::string const & keyContainerName,
            _Out_ Common::CertContextUPtr & cert);

        static Common::ErrorCode InstallCertificate(
            CertContextUPtr & certContext,
            X509StoreLocation::Enum certificateStoreLocation = X509Default::StoreLocation(),
            std::string const & certificateStoreName = X509Default::StoreName());

        static Common::ErrorCode UninstallCertificate(
            X509StoreLocation::Enum certStoreLocation,
            std::string const & certStoreName,
            std::shared_ptr<X509FindValue> const & findValue,
            std::string const & keyContainerFilePath = "");

        static Common::ErrorCode EncryptText(
            std::string const & plainText,
            std::string const & certThumbPrint,
            std::string const & certStoreName,
            X509StoreLocation::Enum certStoreLocation,
            _In_opt_ LPCSTR algorithmOid,
            _Out_ std::string & encryptedText);

        static Common::ErrorCode EncryptText(
            std::string const & plainText,
            std::string const & certFilePath,
            _In_opt_ LPCSTR algorithmOid,
            _Out_ std::string & encryptedText);

        static Common::ErrorCode EncryptText(
            std::string const & plainText,
            PCCertContext certContext,
            _In_opt_ LPCSTR algorithmOid,
            _Out_ std::string & encryptedText);

        static Common::ErrorCode DecryptText(
            std::string const & encryptedText,
            _Out_ SecureString & decryptedText);

        static Common::ErrorCode DecryptText(
            std::string const & encryptedText,
            X509StoreLocation::Enum certStoreLocation,
            _Out_ SecureString & decryptedText);

        static Common::ErrorCode SignMessage(
            BYTE const* inputBuffer,
            size_t inputSize,
            char const* algOID,
            CertContextUPtr const & certContext,
            _Out_ ByteBuffer & signedMessage);

        static Common::ErrorCode SignMessage(
            std::string const & message,
            CertContextUPtr const & certContext,
            _Out_ ByteBuffer & signedMessage);

        #if !defined(PLATFORM_UNIX)
        static Common::ErrorCode VerifyEmbeddedSignature(std::string const & fileName);
        #endif

        static Common::ErrorCode CreateAndACLPasswordFile(std::string const & passwordFilePath, SecureString & password, vector<string> const & machineAccountNamesForACL);
        static Common::ErrorCode GenerateRandomString(_Out_ SecureString & password);
        static void Base64StringToBytes(std::string const & encryptedValue, ByteBuffer & buffer);
        static std::string BytesToBase64String(BYTE const* bytesToEncode, unsigned int length);
        static std::string CertToBase64String(CertContextUPtr const & cert);

    private:
        static bool IsBase64(char c);
        static Common::ErrorCode GetKnownFolderName(int csidl, std::string const & relativePath, std::string & folderName);

        #if !defined(PLATFORM_UNIX)
        static Common::ErrorCode GetCertificatePrivateKeyFileName(
            CertContextUPtr const & certContext, 
            std::string & privateKeyFileName);
        #endif
        static Common::ErrorCode GetCertificateByExtension(
            HCERTSTORE certStore,
            std::shared_ptr<X509FindValue> const & findValue,
            PCCertContext pPrevCertContext,
            PCCertContext & pCertContext);


        static const std::string BASE64_CHARS;

#if defined(PLATFORM_UNIX)

        static const char alphanum[];

        static ErrorCode GetAllCertificates(
            X509StoreLocation::Enum certificateStoreLocation,
            std::string const & certificateStoreName,
            DWORD storeOpenFlag,
            _Out_ std::vector<std::string> & certFiles);

        static bool IsMatch(
            PCCertContext pcertContext,
            std::shared_ptr<X509FindValue> const & findValue);
#else
        static PCCertContext GetCertificateFromStore(
            HCERTSTORE certStore,
            std::shared_ptr<X509FindValue> const & findValue,
            PCCertContext pPrevCertContext);
#endif
    };

    class CrlOfflineErrCache : public enable_shared_from_this<CrlOfflineErrCache>
    {
    public:
        static CrlOfflineErrCache & GetDefault();
        CrlOfflineErrCache();

        static bool CrlCheckingEnabled(uint certChainFlags);

        uint MaskX509CertChainFlags(Thumbprint const & cert, uint flags, bool shouldIgnoreCrlOffline) const;
        void AddError(Thumbprint const & cert, CertContext const* certContext, uint certChainFlagsUsed);
        void TryRemoveError(Thumbprint const & cert, uint certChainFlagsUsed);
        void Update(Thumbprint const & cert, CertContext const* certContext, uint certChainFlagsUsed, bool onlyCrlOfflineEncountered, bool matched);

        using HealthReportCallback = std::function<void(size_t errCount, std::string const & description)>;
        void SetHealthReportCallback(HealthReportCallback const & callback);

        struct ErrorRecord
        {
            ErrorRecord(CertContext const * cert, uint certChainFlagsUsed, StopwatchTime time);

            std::shared_ptr<const CertContext> certContext_;
            uint certChainFlagsUsed_;
            StopwatchTime failureTime_;
        };

        using CacheMap = std::map<Thumbprint, ErrorRecord>;

        CacheMap Test_GetCacheCopy() const;
        void Test_Reset();
        void Test_CheckCrl(uint certChainFlags, uint & errorStatus);

    private:
        void PurgeExpired_LockHeld(StopwatchTime now);
        void GetHealthReport_LockHeld(size_t & errCount, std::string & description); 
        void ReportHealth_LockHeld(size_t errCount, std::string const & description);
        void CleanUp();
        void ScheduleCleanup();

        void RecheckCrl(Thumbprint const & thumbprint, ErrorRecord const & errorRecord);

        mutable RwLock lock_;
        CacheMap cache_;
        bool cleanupActive_ = false;
        HealthReportCallback healthReportCallback_;

        // Test only
        StopwatchTime testStartTime_;
        StopwatchTime testEndTime_;
    };

    class InstallTestCertInScope
    {
        DENY_COPY(InstallTestCertInScope)

    public:
        InstallTestCertInScope(
            std::string const & subjectName = "", // a guid common name will be generated
            const std::vector<std::string> *subjectAltNames = nullptr, // No Alt name extension added if it is null or empty.
            TimeSpan expiration = DefaultCertExpiry(),
            std::string const & storeName = X509Default::StoreName(),
            std::string const & keyContainerName = "",
            X509StoreLocation::Enum storeLocation = X509Default::StoreLocation());

        InstallTestCertInScope(
            bool install,
            std::string const & subjectName = "", // a guid common name will be generated
            const std::vector<std::string> *subjectAltNames = nullptr, // No Alt name extension added if it is null or empty.
            TimeSpan expiration = DefaultCertExpiry(),
            std::string const & storeName = X509Default::StoreName(),
            std::string const & keyContainerName = "",
            X509StoreLocation::Enum storeLocation = X509Default::StoreLocation());

        ~InstallTestCertInScope();

        ErrorCode Install();
        void Uninstall(bool deleteKeyContainer = true);
        void DisableUninstall();

        PCCertContext CertContext() const;

#ifdef PLATFORM_UNIX
        X509Context const & X509ContextRef() const;
#endif

        Thumbprint::SPtr const & Thumbprint() const;
        std::string const & SubjectName() const;
        std::string const & StoreName() const;
        X509StoreLocation::Enum StoreLocation() const;

        CertContextUPtr DetachCertContext();
        CertContextUPtr const & GetCertContextUPtr() const;

        static TimeSpan DefaultCertExpiry();

    private:
        std::string const subjectName_;
        std::string const storeName_;
        X509StoreLocation::Enum const storeLocation_;
        CertContextUPtr certContext_;
        std::string keyContainerFilePath_;
        Thumbprint::SPtr thumbprint_;
        bool uninstallOnDestruction_;
    };
}

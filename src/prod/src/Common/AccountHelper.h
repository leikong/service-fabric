// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class AccountHelper
    {
    public:
        static Common::ErrorCode GetDomainUserAccountName(std::string accountName, __out std::string & username, __out std::string & domain, __out std::string & dlnFormatName);
        static Common::ErrorCode GetServiceAccountName(std::string accountName, __out std::string & username, __out std::string & domain, __out std::string & dlnFormatName);

        static Common::GlobalString UPNDelimiter;
        static Common::GlobalString DLNDelimiter;

        static bool GroupAllowsMemberAddition(std::string const & groupName);

        // If canSkipAddCounter is true, the counter is not added if the name size is 20 characters.
        static std::string GetAccountName(std::string const & name, ULONG applicationPackageCounter, bool canSkipAddCounter = false);

        static ErrorCode GetAccountNamesWithCertificateCommonName(
            std::string const& commonName,
            X509StoreLocation::Enum storeLocation,
            std::string const & storeName,
            bool generateV1Account,
            __inout std::map<std::string, std::string> & accountNamesMap); // key: account name, value: thumbprint

        static ErrorCode GeneratePasswordForNTLMAuthenticatedUser(
            std::string const& accountName,
            Common::SecureString const & passwordSecret,
            __inout Common::SecureString & password);

        static ErrorCode GeneratePasswordForNTLMAuthenticatedUser(
            std::string const& accountName,
            Common::SecureString const & passwordSecret,
            X509StoreLocation::Enum storeLocation,
            std::string const & x509StoreName,
            std::string const & x509Thumbprint,
            __inout Common::SecureString & password);

        static std::string GenerateUserAcountNameV1(
            Thumbprint::SPtr const & thumbprint);

        static std::string GenerateUserAcountName(
            Thumbprint::SPtr const & thumbprint,
            DateTime const & expirationTime);

        static void ReplaceAccountInvalidChars(__inout std::string & accountName);

#if !defined(PLATFORM_UNIX)
        static bool GenerateRandomPassword(__out Common::SecureString & password);
#endif

    private:
        static Common::GlobalString PasswordPrefix;
        static DWORD const RandomDigitsInPassword;
        static size_t const MaxCharactersInAccountUsername;
        static int64 const ExpiryBaselineTicks;

        static Common::GlobalString GroupNameLOCAL;
    };
}

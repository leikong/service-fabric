// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

#include <boost/test/unit_test.hpp>
#include "Common/boost-taef.h"
#include <memory>
#include <thread>
#include "Common/TimeSpan.h"
#include "Common/ExpiringSet.h"
#include "Common/CryptoTest.h"

using namespace Common;
using namespace std;

string const TestCertCN = "WinFabricCryptoUtilityTestCertCN";

namespace Common
{
#define CryptoUtilityTestTraceType "CryptoUtilityTest"

    class CryptoUtilityTest
    {
    protected:
        void InstallTest();
        string GetNewCommonName();
        bool CreateAndVerfiyCertificateThumbprint(string const subjectName, X509FindValue::SPtr findValue);
        bool VerfiyCertificateThumbprint(
            X509FindValue::SPtr const & findValue,
            string const & expectedThumbprintStr,
            string const & x509Path = "");

        bool VerfiyCertificateThumbprint(CertContext* loadedCert, string const expectedThumbprintStr);

        SecurityTestSetup securityTestSetup_;
    };

    BOOST_FIXTURE_TEST_SUITE(CryptoSuite, CryptoUtilityTest);

#ifdef PLATFORM_UNIX

    BOOST_AUTO_TEST_CASE(InstallPem)
    {
        ENTER;

        auto& config = SecurityConfig::GetConfig();
        auto saved1 = config.X509InstallExtension;
        config.X509InstallExtension = ".pem";
        auto saved2 = config.PrivateKeyInstallExtension;
        config.PrivateKeyInstallExtension = ".pem";
        KFinally([&config, saved1, saved2] { config.X509InstallExtension = saved1; config.PrivateKeyInstallExtension = saved2; }); 

        InstallTest();

        LEAVE;
    }

    BOOST_AUTO_TEST_CASE(InstallCrtPrv)
    {
        ENTER;

        auto& config = SecurityConfig::GetConfig();
        auto saved1 = config.X509InstallExtension;
        config.X509InstallExtension = ".crt";
        auto saved2 = config.PrivateKeyInstallExtension;
        config.PrivateKeyInstallExtension = ".prv";
        KFinally([&config, saved1, saved2] { config.X509InstallExtension = saved1; config.PrivateKeyInstallExtension = saved2; }); 

        InstallTest();

        LEAVE;
    }

#endif

    BOOST_AUTO_TEST_CASE(PublicKey)
    {
        ENTER;

        InstallTestCertInScope cert1;
        InstallTestCertInScope cert2(
            "",
            nullptr,
            InstallTestCertInScope::DefaultCertExpiry(),
            X509Default::StoreName(),
            "CryptoSuite.PublicKey.second");

        X509PubKey key1(cert1.CertContext());
        Trace.WriteInfo(CryptoUtilityTestTraceType, "key1 long format = {0:l}", key1);
        Trace.WriteInfo(CryptoUtilityTestTraceType, "key1 = {0}", key1);

        X509PubKey key1dup(cert1.CertContext());
        Trace.WriteInfo(CryptoUtilityTestTraceType, "key1dup = {0}", key1dup);

        VERIFY_IS_TRUE(key1 == key1dup);
        VERIFY_IS_FALSE(key1 < key1dup);
        VERIFY_IS_FALSE(key1dup < key1);

        X509PubKey key2(cert2.CertContext());
        Trace.WriteInfo(CryptoUtilityTestTraceType, "key2 = {0}", key2);

        VERIFY_IS_FALSE(key1 == key2);
        if (key1 < key2) VERIFY_IS_FALSE(key2 < key1);
        if (key2 < key1) VERIFY_IS_FALSE(key1 < key2);

        LEAVE;
    }

    BOOST_AUTO_TEST_CASE(LoadCertificateByThumbprint)
    {
        string commonName = GetNewCommonName();
        string subjectName = ("CN=" + commonName);
        InstallTestCertInScope testCert(subjectName);

        auto thumbprintStr = testCert.Thumbprint()->ToStrings().first;
        X509FindValue::SPtr findValue;
        auto error = X509FindValue::Create(X509FindType::FindByThumbprint, thumbprintStr, findValue);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(VerfiyCertificateThumbprint(findValue, thumbprintStr));
    }

    BOOST_AUTO_TEST_CASE(LoadCertificateBySubjectName)
    {
        string commonName = GetNewCommonName();
        string subjectName = ("CN=" + commonName);

        X509FindValue::SPtr findValue;
        auto error = X509FindValue::Create(X509FindType::FindBySubjectName, subjectName, findValue);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(CreateAndVerfiyCertificateThumbprint(subjectName, findValue));
    }

    BOOST_AUTO_TEST_CASE(LoadCertificateByCommonName)
    {
        string commonName = GetNewCommonName();
        string subjectName = ("CN=" + commonName);

        X509FindValue::SPtr findValue;
        auto error = X509FindValue::Create(X509FindType::FindBySubjectName, commonName, findValue);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(CreateAndVerfiyCertificateThumbprint(subjectName, findValue));
    }

    BOOST_AUTO_TEST_CASE(MatchCountTest)
    {
        string commonName = "MatchCountTest." + Guid::NewGuid().ToString();
        string subjectName = ("CN=" + commonName);
        InstallTestCertInScope testCert(subjectName);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "FindBySubjectName with common name");
        {
            X509FindValue::SPtr findValue;
            auto error = X509FindValue::Create(X509FindType::FindBySubjectName, commonName, findValue);
            VERIFY_IS_TRUE(error.IsSuccess());

            // Load cert using given "X509FindValue" findValue
            CertContexts loadedCerts;
            error = CryptoUtility::GetCertificate(
                X509Default::StoreLocation(),
                X509Default::StoreName(),
                findValue,
                loadedCerts);

            VERIFY_IS_TRUE(error.IsSuccess(), "GetCertificate call");
            VERIFY_ARE_EQUAL2(loadedCerts.size(), 1);

        }

        Trace.WriteInfo(CryptoUtilityTestTraceType, "FindBySubjectName with subject name");
        {
            X509FindValue::SPtr findValue;
            auto error = X509FindValue::Create(X509FindType::FindBySubjectName, subjectName, findValue);
            VERIFY_IS_TRUE(error.IsSuccess());

            // Load cert using given "X509FindValue" findValue
            CertContexts loadedCerts;
            error = CryptoUtility::GetCertificate(
                X509Default::StoreLocation(),
                X509Default::StoreName(),
                findValue,
                loadedCerts);

            VERIFY_IS_TRUE(error.IsSuccess(), "GetCertificate call");
            VERIFY_ARE_EQUAL2(loadedCerts.size(), 1);
        }

        Trace.WriteInfo(CryptoUtilityTestTraceType, "FindByThumbprint");
        {
            X509FindValue::SPtr findValue;
            auto error = X509FindValue::Create(X509FindType::FindByThumbprint, testCert.Thumbprint()->PrimaryToString(), findValue);
            VERIFY_IS_TRUE(error.IsSuccess());

            // Load cert using given "X509FindValue" findValue
            CertContexts loadedCerts;
            error = CryptoUtility::GetCertificate(
                X509Default::StoreLocation(),
                X509Default::StoreName(),
                findValue,
                loadedCerts);

            VERIFY_IS_TRUE(error.IsSuccess(), "GetCertificate call");
            VERIFY_ARE_EQUAL2(loadedCerts.size(), 1);
        }
    }

    BOOST_AUTO_TEST_CASE(EncryptTestInternal) // Test with internal API
    {
        InstallTestCertInScope testCert;

        string plainText = "The lazy fox jumped";
        string cipherText;
        auto error = CryptoUtility::EncryptText(
            plainText,
            testCert.Thumbprint()->ToStrings().first,
            X509Default::StoreName(),
            X509Default::StoreLocation(),
            nullptr,
            cipherText);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "EncryptText returned {0}", error);
        VERIFY_ARE_EQUAL2(error.ReadValue(), ErrorCodeValue::Success);
        Trace.WriteInfo(CryptoUtilityTestTraceType, "ciphertext: {0}", cipherText);

        SecureString decrypted;
        error = CryptoUtility::DecryptText(cipherText, X509Default::StoreLocation(), decrypted);
        Trace.WriteInfo(CryptoUtilityTestTraceType, "DecryptText returned {0}", error);
        Trace.WriteInfo(CryptoUtilityTestTraceType, "decrypted: {0}", decrypted.GetPlaintext());
        VERIFY_ARE_EQUAL2(error.ReadValue(), ErrorCodeValue::Success);
        VERIFY_ARE_EQUAL2(decrypted.GetPlaintext(), plainText);
    }

    BOOST_AUTO_TEST_CASE(LoadCertificateByAltSubjectName)
    {
        vector<string> subjectAltNames;
        subjectAltNames.push_back("TestDns");
        subjectAltNames.push_back("TestDns.TestSubDomain");
        subjectAltNames.push_back("TestDns.TestSubDomain.Microsoft.Com");
        subjectAltNames.push_back("TestDns.TestSubDomain.Microsoft");

        InstallTestCertInScope testCert("", &subjectAltNames);

        auto thumbprintStr = testCert.Thumbprint()->ToStrings().first;

        for (int i = 0; i < subjectAltNames.size(); i++)
        {
            // Try searching search string as is, its lower case variant and upper case varient.
            for (int j = 0; j < 3; j++)
            {
                string searchString = subjectAltNames[i];
                switch (j)
                {
                case 1:
                    // Convert search string to lower case.
                    StringUtility::ToLower(searchString);
                    break;
                case 2:
                    // Convert search string to upper case.
                    StringUtility::ToUpper(searchString);
                    break;
                }

                X509FindValue::SPtr findValue;
                string findValueString = "2.5.29.17:3=" + searchString;
                auto error = X509FindValue::Create(X509FindType::FindByExtension, findValueString, findValue);
                VERIFY_IS_TRUE(error.IsSuccess());
                VERIFY_IS_TRUE(VerfiyCertificateThumbprint(findValue, thumbprintStr));
            }
        }
    }

#ifdef PLATFORM_UNIX

    BOOST_AUTO_TEST_CASE(ValidPeriod)
    {
        auto now = DateTime::Now();
        auto expectedExpiry = now + InstallTestCertInScope::DefaultCertExpiry() - TimeSpan::FromMinutes(60*24);

        InstallTestCertInScope testCert;

        LinuxCryptUtil cryptoUtil;
        DateTime notBefore, notAfter;
        cryptoUtil.GetCertificateNotBeforeDate(testCert.CertContext(), notBefore); 
        cryptoUtil.GetCertificateNotAfterDate(testCert.CertContext(), notAfter);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "NotBefore = {0}, NotAfter = {1}, expectedExpiry = {2}", notBefore, notAfter, expectedExpiry);
        BOOST_REQUIRE(notBefore <= now); 
        BOOST_REQUIRE(expectedExpiry <= notAfter);
    }

#endif

    BOOST_AUTO_TEST_CASE(CrlOfflineErrCacheBasic)
    {
        auto& cache = CrlOfflineErrCache::GetDefault();
        cache.Test_Reset();

        auto& config = SecurityConfig::GetConfig();
        auto saved = config.CrlDisablePeriod;
        config.CrlDisablePeriod = TimeSpan::FromSeconds(3);
        auto saved2 = config.CrlOfflineHealthReportTtl;
        config.CrlOfflineHealthReportTtl = TimeSpan::FromSeconds(3);
        KFinally([&]
        {
            config.CrlDisablePeriod = saved;
            config.CrlOfflineHealthReportTtl = saved2;
            cache.Test_Reset();
        });

        auto cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_IS_TRUE(cacheCopy.empty());

        auto offlineReported = make_shared<AutoResetEvent>(false);
        auto recoveryReported = make_shared<AutoResetEvent>(false);
        auto bothCertsAdded = make_shared<AutoResetEvent>(false);
        cache.SetHealthReportCallback([=] (size_t cacheSize, string const&)
        {
            if (cacheSize == 0)
            {
                recoveryReported->Set(); 
                return;
            }

            if (cacheSize == 1)
            {
                offlineReported->Set();
                return;
            }

            if (cacheSize == 2)
            {
                bothCertsAdded->Set();
                return;
            }
        });

        InstallTestCertInScope c1;
        InstallTestCertInScope c2;

        Trace.WriteInfo(CryptoUtilityTestTraceType, "test before AddError");
        uint testFlag = CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT;  
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true), testFlag);
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "test after AddError");
        cache.AddError(*(c1.Thumbprint()), c1.CertContext(), testFlag); 

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_ARE_EQUAL2(cacheCopy.size(), 1);
        auto iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());

        auto masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);

        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true), testFlag);
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, false), testFlag);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "test duplicate AddError");
        cache.AddError(*(c1.Thumbprint()), c1.CertContext(), testFlag); 

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_ARE_EQUAL2(cacheCopy.size(), 1);
        iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());

        masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);

        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true), testFlag);
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, false), testFlag);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "wait for offline report");
        VERIFY_IS_TRUE(offlineReported->WaitOne(TimeSpan::FromSeconds(10)));
        Trace.WriteInfo(CryptoUtilityTestTraceType, "wait for recovery report");
        VERIFY_IS_TRUE(recoveryReported->WaitOne(TimeSpan::FromSeconds(10)));

        Trace.WriteInfo(CryptoUtilityTestTraceType, "test after recovery report");
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true), testFlag);
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true), testFlag);
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, false), testFlag);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "test AddError on both c1 and c2");
        cache.AddError(*(c1.Thumbprint()), c1.CertContext(), testFlag); 
        cache.AddError(*(c2.Thumbprint()), c2.CertContext(), testFlag); 

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_ARE_EQUAL2(cacheCopy.size(), 2);
        iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c2.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());

        masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);

        masked = cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, false), testFlag);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "wait for offline report for both certs");
        VERIFY_IS_TRUE(bothCertsAdded->WaitOne(TimeSpan::FromSeconds(10)));
        Trace.WriteInfo(CryptoUtilityTestTraceType, "wait for recovery report");
        VERIFY_IS_TRUE(recoveryReported->WaitOne(TimeSpan::FromSeconds(10)));
    }

    BOOST_AUTO_TEST_CASE(CrlOfflineErrCacheOverflow)
    {
        auto& cache = CrlOfflineErrCache::GetDefault();
        cache.Test_Reset();

        auto& config = SecurityConfig::GetConfig();
        auto saved = config.CrlOfflineErrCacheCapacity;
        config.CrlOfflineErrCacheCapacity = 3;
        KFinally([&]
        {
            config.CrlOfflineErrCacheCapacity = saved;
            cache.Test_Reset();
        });

        auto cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_IS_TRUE(cacheCopy.empty());

        InstallTestCertInScope c1;
        InstallTestCertInScope c2;
        InstallTestCertInScope c3;
        InstallTestCertInScope c4;
        InstallTestCertInScope c5;

        uint testFlag = CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT;  
        cache.AddError(*(c1.Thumbprint()), c1.CertContext(), testFlag); 
        AutoResetEvent().WaitOne(TimeSpan::FromMilliseconds(1));
        cache.AddError(*(c2.Thumbprint()), c2.CertContext(), testFlag); 
        AutoResetEvent().WaitOne(TimeSpan::FromMilliseconds(1));
        cache.AddError(*(c3.Thumbprint()), c3.CertContext(), testFlag); 
        AutoResetEvent().WaitOne(TimeSpan::FromMilliseconds(1));
        cache.AddError(*(c4.Thumbprint()), c4.CertContext(), testFlag); 
        AutoResetEvent().WaitOne(TimeSpan::FromMilliseconds(1));
        cache.AddError(*(c5.Thumbprint()), c5.CertContext(), testFlag); 

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_IS_TRUE(cacheCopy.size() == config.CrlOfflineErrCacheCapacity);

        auto iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c2.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c3.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c4.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c5.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());

        auto masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        masked = cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        masked = cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        masked = cache.MaskX509CertChainFlags(*(c4.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        masked = cache.MaskX509CertChainFlags(*(c5.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
    }

    BOOST_AUTO_TEST_CASE(CrlOfflineErrCacheRemove)
    {
        auto& cache = CrlOfflineErrCache::GetDefault();
        cache.Test_Reset();
        KFinally([&] { cache.Test_Reset(); });

        auto cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_IS_TRUE(cacheCopy.empty());

        InstallTestCertInScope c1;
        InstallTestCertInScope c2;
        InstallTestCertInScope c3;

        Trace.WriteInfo(CryptoUtilityTestTraceType, "test before AddError");
        uint testFlag = CERT_CHAIN_REVOCATION_CHECK_CHAIN;  
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true), testFlag);
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true), testFlag);
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, false), testFlag);
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, true), testFlag);
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, false), testFlag);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "test after AddError");
        cache.AddError(*(c1.Thumbprint()), c1.CertContext(), testFlag); 
        cache.AddError(*(c2.Thumbprint()), c2.CertContext(), testFlag); 
        cache.AddError(*(c3.Thumbprint()), c3.CertContext(), testFlag); 

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_ARE_EQUAL2(cacheCopy.size(), 3);

        auto iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c2.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c3.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());

        auto masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, false), testFlag);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "remove with 0");
        cache.TryRemoveError(*(c1.Thumbprint()), 0); 
        cache.TryRemoveError(*(c2.Thumbprint()), 0); 
        cache.TryRemoveError(*(c3.Thumbprint()), 0); 

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_ARE_EQUAL2(cacheCopy.size(), 3);

        iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c2.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c3.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());

        masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, false), testFlag);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "remove c1");
        cache.TryRemoveError(*(c1.Thumbprint()), testFlag); 

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_ARE_EQUAL2(cacheCopy.size(), 2);

        iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c2.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c3.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());

        masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, false), testFlag);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "remove c3");
        cache.TryRemoveError(*(c3.Thumbprint()), testFlag); 

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_ARE_EQUAL2(cacheCopy.size(), 1);

        iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c2.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c3.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());

        masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, false), testFlag);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "add c1 again");
        cache.AddError(*(c1.Thumbprint()), c1.CertContext(), testFlag); 

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_ARE_EQUAL2(cacheCopy.size(), 2);

        iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c2.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c3.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());

        masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, false), testFlag);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "remove c1 c2 c3");
        cache.TryRemoveError(*(c1.Thumbprint()), testFlag); 
        cache.TryRemoveError(*(c2.Thumbprint()), testFlag); 
        cache.TryRemoveError(*(c3.Thumbprint()), testFlag); 

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_ARE_EQUAL2(cacheCopy.size(), 0);

        iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c2.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c3.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());

        masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, false), testFlag);

        Trace.WriteInfo(CryptoUtilityTestTraceType, "remove c1 c2 c3 again should not cause any issue");
        cache.TryRemoveError(*(c1.Thumbprint()), testFlag); 
        cache.TryRemoveError(*(c2.Thumbprint()), testFlag); 
        cache.TryRemoveError(*(c3.Thumbprint()), testFlag); 

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_ARE_EQUAL2(cacheCopy.size(), 0);

        iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c2.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c3.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());

        masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, false), testFlag);
        masked = cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        VERIFY_ARE_EQUAL2(cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, false), testFlag);
    }

    BOOST_AUTO_TEST_CASE(CrlOfflineErrCacheConcurrent)
    {
        auto& cache = CrlOfflineErrCache::GetDefault();
        cache.Test_Reset();
        KFinally([&] { cache.Test_Reset(); });

        auto cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_IS_TRUE(cacheCopy.empty());

        InstallTestCertInScope c1;
        InstallTestCertInScope c2;
        InstallTestCertInScope c3;
        InstallTestCertInScope c4;
        InstallTestCertInScope c5;

        Trace.WriteInfo(CryptoUtilityTestTraceType, "add in parallel"); 
        const uint testFlag = CERT_CHAIN_REVOCATION_CHECK_END_CERT;
        const size_t thrCount = 12;
        atomic_uint64 completedThreads(0);
        AutoResetEvent allThreadsCompleted(false);
        for (size_t i = 0; i < thrCount; ++i)
        {
            Threadpool::Post([&, i]
            {
                if (i%2)
                {
                    cache.AddError(*(c1.Thumbprint()), c1.CertContext(), testFlag);
                    cache.AddError(*(c2.Thumbprint()), c2.CertContext(), testFlag); 
                    cache.AddError(*(c3.Thumbprint()), c3.CertContext(), testFlag); 
                }
                else
                {
                    cache.AddError(*(c4.Thumbprint()), c4.CertContext(), testFlag); 
                    this_thread::yield();
                    cache.AddError(*(c5.Thumbprint()), c5.CertContext(), testFlag); 
                }

                if ((++completedThreads) == thrCount)
                {
                    allThreadsCompleted.Set();
                }
            });
        }

        Trace.WriteInfo(CryptoUtilityTestTraceType, "wait for all add to complete"); 
        VERIFY_IS_TRUE(allThreadsCompleted.WaitOne(TimeSpan::FromSeconds(6)));

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_ARE_EQUAL2(cacheCopy.size(), 5);

        auto iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c2.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c3.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c4.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c5.Thumbprint()));
        VERIFY_IS_FALSE(iter == cacheCopy.cend());

        auto masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        masked = cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        masked = cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        masked = cache.MaskX509CertChainFlags(*(c4.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        masked = cache.MaskX509CertChainFlags(*(c5.Thumbprint()), testFlag, true);
        VERIFY_IS_FALSE(CrlOfflineErrCache::CrlCheckingEnabled(masked));

        Trace.WriteInfo(CryptoUtilityTestTraceType, "remove in parallel"); 
        completedThreads.store(0);
        for (size_t i = 0; i < thrCount; ++i)
        {
            Threadpool::Post([&, i]
            {
                if (i%2)
                {
                    cache.TryRemoveError(*(c1.Thumbprint()), testFlag);
                    cache.TryRemoveError(*(c3.Thumbprint()), testFlag);
                    cache.TryRemoveError(*(c5.Thumbprint()), testFlag);
                }
                else
                {
                    cache.TryRemoveError(*(c2.Thumbprint()), testFlag);
                    this_thread::yield();
                    cache.TryRemoveError(*(c4.Thumbprint()), testFlag);
                }

                if ((++completedThreads) == thrCount)
                {
                    allThreadsCompleted.Set();
                }
            });
        }

        Trace.WriteInfo(CryptoUtilityTestTraceType, "wait for all remove to complete"); 
        VERIFY_IS_TRUE(allThreadsCompleted.WaitOne(TimeSpan::FromSeconds(6)));

        cacheCopy = cache.Test_GetCacheCopy();
        VERIFY_ARE_EQUAL2(cacheCopy.size(), 0);

        iter = cacheCopy.find(*(c1.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c2.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c3.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c4.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());
        iter = cacheCopy.find(*(c5.Thumbprint()));
        VERIFY_IS_TRUE(iter == cacheCopy.cend());

        masked = cache.MaskX509CertChainFlags(*(c1.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        masked = cache.MaskX509CertChainFlags(*(c2.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        masked = cache.MaskX509CertChainFlags(*(c3.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        masked = cache.MaskX509CertChainFlags(*(c4.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
        masked = cache.MaskX509CertChainFlags(*(c5.Thumbprint()), testFlag, true);
        VERIFY_IS_TRUE(CrlOfflineErrCache::CrlCheckingEnabled(masked));
    }

    BOOST_AUTO_TEST_CASE(CryptoBitBlobTest)
    {
        ENTER;

        CryptoBitBlob bb;
        VERIFY_IS_TRUE(bb.Data().size() == 0);
        VERIFY_IS_TRUE(bb.UnusedBits() == 0);
        VERIFY_IS_TRUE(bb == bb);

        CryptoBitBlob bb2;
        VERIFY_IS_TRUE(bb == bb2);

        CryptoBitBlob bb3;
        VERIFY_IS_TRUE(bb3.Initialize("11 22 33").IsSuccess());
        VERIFY_IS_TRUE(bb3.UnusedBits() == 0);
        VERIFY_IS_FALSE(bb3 == bb);

        CryptoBitBlob bb4;
        VERIFY_IS_TRUE(bb4.Initialize("112233").IsSuccess());
        VERIFY_IS_TRUE(bb4.UnusedBits() == 0);
        VERIFY_IS_TRUE(bb3 == bb4);

        CryptoBitBlob bb5;
        VERIFY_IS_TRUE(bb5.Initialize("2233").IsSuccess());
        VERIFY_IS_TRUE(bb5.UnusedBits() == 0);
        VERIFY_IS_FALSE(bb4 == bb5);
        VERIFY_IS_FALSE(bb5 == bb);

        CryptoBitBlob bb6;
        VERIFY_IS_TRUE(bb6.Initialize("112333").IsSuccess());
        VERIFY_IS_TRUE(bb5.UnusedBits() == 0);
        VERIFY_IS_FALSE(bb6 == bb4);
        VERIFY_IS_FALSE(bb6 == bb5);

        ByteBuffer buffer;
        VERIFY_IS_TRUE(FabricSerializer::Serialize(&bb6, buffer).IsSuccess());

        CryptoBitBlob bb6d;
        VERIFY_IS_TRUE(FabricSerializer::Deserialize(bb6d, buffer).IsSuccess());
        VERIFY_IS_TRUE(bb6d == bb6);

        ByteBuffer buffer2;
        VERIFY_IS_TRUE(FabricSerializer::Serialize(&bb6d, buffer2).IsSuccess());
        VERIFY_IS_TRUE(buffer == buffer2);

#ifndef PLATFORM_UNIX

        InstallTestCertInScope cert1("CN=cert1.CryptoBitBlobTest");
        CryptoBitBlob bb7(cert1.CertContext()->pCertInfo->SubjectPublicKeyInfo.PublicKey);
        VERIFY_IS_TRUE(bb7.Data().size() > 0);
        VERIFY_IS_FALSE(bb7 == bb6);

        ByteBuffer buffer3;
        VERIFY_IS_TRUE(FabricSerializer::Serialize(&bb7, buffer3).IsSuccess());

        CryptoBitBlob bb7d;
        VERIFY_IS_TRUE(FabricSerializer::Deserialize(bb7d, buffer3).IsSuccess());
        VERIFY_IS_TRUE(bb7d == bb7);

        ByteBuffer buffer4;
        VERIFY_IS_TRUE(FabricSerializer::Serialize(&bb7d, buffer4).IsSuccess());
        VERIFY_IS_TRUE(buffer3 == buffer4);

        InstallTestCertInScope cert2(
            "CN=cert2.CryptoBitBlobTest",
            nullptr,
            TimeSpan::FromMinutes(60),
            X509Default::StoreName(),
            "cert2.CryptoBitBlobTest" /*non-default key container to generate a different key pair*/);

        CryptoBitBlob bb8(cert2.CertContext()->pCertInfo->SubjectPublicKeyInfo.PublicKey);
        VERIFY_IS_TRUE(bb8 != bb7);

        CryptoBitBlob bb9(bb7);
        VERIFY_IS_TRUE(bb9 == bb7);

        CryptoBitBlob bb10(move(bb7));
        VERIFY_IS_TRUE(bb10.Data().size() > 0);
        VERIFY_IS_TRUE(bb10 != bb7);
        VERIFY_IS_TRUE(bb10 == bb9);

        VERIFY_IS_TRUE(bb9 != bb8);
        bb9 = bb8;
        VERIFY_IS_TRUE(bb9 == bb8);

        bb10 = move(bb8);
        VERIFY_IS_TRUE(bb10 == bb9);
        VERIFY_IS_TRUE(bb8 == bb);
        VERIFY_IS_TRUE(bb10 != bb8);

        CryptoBitBlob bb11, bb12, bb13;
        auto err = bb11.InitializeAsSignatureHash(cert1.CertContext());
        VERIFY_IS_TRUE(err.IsSuccess());
        Trace.WriteInfo(CryptoUtilityTestTraceType, "InitializeAsSignatureHash(cert1): {0}", bb11);
        err = bb12.InitializeAsSignatureHash(cert2.CertContext());
        VERIFY_IS_TRUE(err.IsSuccess());
        Trace.WriteInfo(CryptoUtilityTestTraceType, "InitializeAsSignatureHash(cert2): {0}", bb12);

        VERIFY_IS_TRUE(bb11 != bb12);

        err = bb13.InitializeAsSignatureHash(cert1.CertContext());
        VERIFY_IS_TRUE(err.IsSuccess());
        Trace.WriteInfo(CryptoUtilityTestTraceType, "InitializeAsSignatureHash(cert1): {0}", bb13);
        VERIFY_IS_TRUE(bb11 == bb13);

        X509FindValue::SPtr findValue;
        err = X509FindValue::Create(X509FindType::FindBySubjectName, "GlobalSign", findValue);
        if (!err.IsSuccess()) return;

        // Load cert using given "X509FindValue" findValue
        CertContextUPtr hostCert;
        err = CryptoUtility::GetCertificate(
            X509Default::StoreLocation(),
            "Root",
            findValue,
            hostCert);

        if (!err.IsSuccess()) return;

        CryptoBitBlob bb15;
        err = bb15.InitializeAsSignatureHash(hostCert.get());
        Trace.WriteInfo(CryptoUtilityTestTraceType, "InitializeAsSignatureHash(GlobalSign): {0}", bb15);
        VERIFY_IS_TRUE(bb15.Data().size() > 20); // SHA1 is deprecated, signature hash size should be larger
#endif

        LEAVE;
    }

    BOOST_AUTO_TEST_SUITE_END()

    bool CryptoUtilityTest::CreateAndVerfiyCertificateThumbprint(string const subjectName, X509FindValue::SPtr findValue)
    {
        // create cert with given subjectName
        InstallTestCertInScope testCert(subjectName);

        // Get thumbprint of new certificate
        auto thumbprintStr = testCert.Thumbprint()->ToStrings().first;

        // Load certificate by FindValue and verify it is the same newly created certificate
        return VerfiyCertificateThumbprint(findValue, thumbprintStr);
    }

    bool CryptoUtilityTest::VerfiyCertificateThumbprint(
        X509FindValue::SPtr const & findValue,
        const string & expectedThumbprintStr,
        const string & x509Path)
    {
        // Load cert using given "X509FindValue" findValue
        CertContextUPtr loadedCert;
        auto error = CryptoUtility::GetCertificate(
            X509Default::StoreLocation(),
            x509Path.empty()? X509Default::StoreName() : x509Path, 
            findValue,
            loadedCert);

        VERIFY_IS_TRUE(error.IsSuccess(), "GetCertificate call");
        VERIFY_IS_TRUE(loadedCert, "assert that loaded cert is not null");
        return VerfiyCertificateThumbprint((CertContext*)loadedCert.get(), expectedThumbprintStr);
    }

    bool CryptoUtilityTest::VerfiyCertificateThumbprint(CertContext* loadedCert, const string expectedThumbprintStr)
    {
        Thumbprint loadedThumbprint;
        auto error = loadedThumbprint.Initialize(loadedCert);
        VERIFY_IS_TRUE(error.IsSuccess());

        Thumbprint expectedThumbprint;
        error = expectedThumbprint.Initialize(expectedThumbprintStr);
        VERIFY_IS_TRUE(error.IsSuccess());

        // Verify the certificate thumbprint matches.
        VERIFY_ARE_EQUAL2(loadedThumbprint, expectedThumbprint);
        return true;
    }

    std::string CryptoUtilityTest::GetNewCommonName()
    {
        SYSTEMTIME st = DateTime::Now().GetSystemTime();
        std::string postfix;
        Common::StringWriter(postfix).Write("{0:04d}{1:02d}{2:02d}{3:02d}{4:02d}{5:02d}{6:03d}",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        string commonName = TestCertCN + postfix;
        return commonName;
    }

#ifdef PLATFORM_UNIX
    void CryptoUtilityTest::InstallTest()
    {
        auto commonName = GetNewCommonName();
        auto subjectName = ("CN=" + commonName);
        InstallTestCertInScope testCert(subjectName);

        auto thumbprintStr = testCert.Thumbprint()->PrimaryToString();
        X509FindValue::SPtr findValue;
        auto error = X509FindValue::Create(X509FindType::FindByThumbprint, thumbprintStr, findValue);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(VerfiyCertificateThumbprint(findValue, thumbprintStr));

        error = X509FindValue::Create(X509FindType::FindBySubjectName, subjectName, findValue);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(VerfiyCertificateThumbprint(findValue, thumbprintStr));

        PrivKeyContext privKey;
        
        LinuxCryptUtil cryptoUtil;
        auto corePfxPath = thumbprintStr;
        StringUtility::ToUpper(corePfxPath);
        long buflen = 16384;

        auto uid = geteuid();
        unique_ptr<char[]> buf(new char[buflen]);
        struct passwd pwbuf, *pwbufp;
        auto errorCode = getpwuid_r(uid, &pwbuf, buf.get(), buflen, &pwbufp);
        VERIFY_IS_TRUE(errorCode == 0);
        auto homeDirectory = string(pwbufp->pw_dir);
        auto corePfxPathw = Path::Combine(homeDirectory + "/.dotnet/corefx/cryptography/x509stores/my", corePfxPath + ".pfx");
        KFinally ([&corePfxPathw] { std::remove(corePfxPathw.c_str()); });
        error = cryptoUtil.InstallCoreFxCertificate(pwbufp->pw_name, testCert.X509ContextRef());
        VERIFY_IS_TRUE(error.IsSuccess());
        error = cryptoUtil.ReadPrivateKey(corePfxPathw, privKey);
        VERIFY_IS_TRUE(error.IsSuccess());

        auto x509Folder = SecurityConfig::GetConfig().X509Folder;
        SecurityConfig::GetConfig().X509Folder = "NoSuchFolder"; // Pollute default to test explicit loading path
        KFinally ([&x509Folder] { SecurityConfig::GetConfig().X509Folder = x509Folder; });

        auto const path = testCert.X509ContextRef().FilePath();
        string pathw = path;
        error = X509FindValue::Create(X509FindType::FindByThumbprint, "", findValue); //find value to match "anything"
        VERIFY_ARE_EQUAL2(error.ReadValue(), ErrorCodeValue::Success);
        VERIFY_IS_TRUE(VerfiyCertificateThumbprint(findValue, thumbprintStr, pathw));

        error = X509FindValue::Create(X509FindType::FindBySubjectName, nullptr, findValue); //find value to match "anything"
        VERIFY_ARE_EQUAL2(error.ReadValue(), ErrorCodeValue::Success);
        VERIFY_IS_TRUE(VerfiyCertificateThumbprint(findValue, thumbprintStr, pathw));

        auto const folderw = Path::GetDirectoryName(pathw);
        error = X509FindValue::Create(X509FindType::FindByThumbprint, thumbprintStr, findValue);
        VERIFY_IS_TRUE(error.IsSuccess());
        Trace.WriteInfo(CryptoUtilityTestTraceType, "test with explicit certificate folder {0} and find value {1}", folderw, *findValue);
        VERIFY_IS_TRUE(VerfiyCertificateThumbprint(findValue, thumbprintStr, folderw));

        error = X509FindValue::Create(X509FindType::FindBySubjectName, subjectName, findValue);
        VERIFY_IS_TRUE(error.IsSuccess());
        Trace.WriteInfo(CryptoUtilityTestTraceType, "test with explicit certificate folder {0} and find value {1}", folderw, *findValue);
        VERIFY_IS_TRUE(VerfiyCertificateThumbprint(findValue, thumbprintStr, folderw));
        Trace.WriteInfo(CryptoUtilityTestTraceType, "InstallTest completed");
    }
#endif
}

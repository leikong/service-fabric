// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

#include <thread>
#include <boost/test/unit_test.hpp>
#include "Common/boost-taef.h"
#include "TestCommon.h"

using namespace Transport;
using namespace Common;
using namespace std;

void TTestUtil::EnterTest(char const * testName)
{
    Trace.WriteInfo(BoostTestTrace, "Enter test: {0}", testName);

    TcpConnection::Test_ResetConnectionFailureCount();
    AcceptThrottle::GetThrottle()->Test_Reset();

    bool w1 = TTestUtil::WaitUntil([] { return TcpConnection::GetObjCount() == 0;}, TimeSpan::FromSeconds(30));
    bool w2 = TTestUtil::WaitUntil([] { return TcpSendTarget::GetObjCount() == 0; }, TimeSpan::FromSeconds(20));
    bool w3 = TTestUtil::WaitUntil([] { return TcpDatagramTransport::GetObjCount() == 0; }, TimeSpan::FromSeconds(10));

    Trace.WriteTrace(
        (w1 && w2 && w3)? LogLevel::Info : LogLevel::Error,
        BoostTestTrace,
        "Enter test: transportCount = {0}, targetCount = {1}, connectionCount = {2}",
        TcpDatagramTransport::GetObjCount(),
        TcpSendTarget::GetObjCount(),
        TcpConnection::GetObjCount());

    VERIFY_IS_TRUE(w1);
    VERIFY_IS_TRUE(w2);
    VERIFY_IS_TRUE(w3);

#ifdef PLATFORM_UNIX
    bool w4 = TTestUtil::WaitUntil([] { return X509Context::ObjCount() == 0; }, TimeSpan::FromSeconds(10));
    Trace.WriteTrace(
        w4 ? LogLevel::Info : LogLevel::Error,
        BoostTestTrace,
        "x509 count = {0}",
        X509Context::ObjCount());

    VERIFY_IS_TRUE(w4);
#endif
}

void TTestUtil::LeaveTest(char const * testName)
{
    Trace.WriteInfo(BoostTestTrace, "Leave test: {0}", testName);

    bool w1 = TTestUtil::WaitUntil([] { return TcpConnection::GetObjCount() == 0; }, TimeSpan::FromSeconds(30));
    bool w2 = TTestUtil::WaitUntil([] { return TcpSendTarget::GetObjCount() == 0; }, TimeSpan::FromSeconds(20));
    bool w3 = TTestUtil::WaitUntil([] { return TcpDatagramTransport::GetObjCount() == 0; }, TimeSpan::FromSeconds(10));

    Trace.WriteInfo(
        BoostTestTrace,
        "Leave test: transportCount = {0}, targetCount = {1}, connectionCount = {2}",
        TcpDatagramTransport::GetObjCount(),
        TcpSendTarget::GetObjCount(),
        TcpConnection::GetObjCount());

    VERIFY_IS_TRUE(w1);
    VERIFY_IS_TRUE(w2);
    VERIFY_IS_TRUE(w3);

#ifdef PLATFORM_UNIX
    bool w4 = TTestUtil::WaitUntil([] { return X509Context::ObjCount() == 0; }, TimeSpan::FromSeconds(10));
    Trace.WriteInfo(BoostTestTrace, "x509 count = {0}", X509Context::ObjCount());
    VERIFY_IS_TRUE(w4);
#endif

    TcpConnection::Test_ResetConnectionFailureCount();
    AcceptThrottle::GetThrottle()->Test_Reset();
}

string TTestUtil::GetListenAddress(ListenAddressType::Enum listenAddressType)
{
    switch (listenAddressType)
    {
    case ListenAddressType::LoopbackIpv4:
        return GetListenAddress("127.0.0.1");

    case ListenAddressType::LoopbackIpv6:
        return GetListenAddress("[::1]");

    case ListenAddressType::Localhost:
        return GetListenAddress("localhost");

    case ListenAddressType::Fqdn:
    {
        string localFqdn;
        auto error = TcpTransportUtility::GetLocalFqdn(localFqdn);
        ASSERT_IFNOT(error.IsSuccess(), "TcpTransportUtility::GetLocalFqdn failed: {0}", error);
        return GetListenAddress(localFqdn);
    }

    default:
        Assert::CodingError("Invalid ListenAddressType: {0}", (int)listenAddressType);
    }
}

string TTestUtil::GetListenAddress(string const & addressWithoutPort)
{
    USHORT listenPort = 0;
    for(int i = 0; i < 600; ++i)
    {
        TestPortHelper::GetPorts(1, listenPort);
        auto portInUse = TestPortHelper::GetPortsInUse();
        if (portInUse.find(listenPort) != portInUse.cend())
        {
            Trace.WriteInfo(BoostTestTrace, "listen port {0} is already in use", listenPort);
            this_thread::yield();
            continue;
        }

        return formatString.L("{0}:{1}", addressWithoutPort, listenPort);
    }

    Assert::CodingError("failed to get a listen port");
}

string TTestUtil::GetGuidAction()
{
    return Guid::NewGuid().ToString();
}

void TTestUtil::SetMessageHandler(
    IDatagramTransportSPtr transport,
    string const & action,
    IDatagramTransport::MessageHandler const & handler)
{
    transport->SetMessageHandler(
    [action, handler](MessageUPtr & message, ISendTarget::SPtr const & target)
    {
        if (message->Action != action)
        {
            Trace.WriteError(BoostTestTrace, "dropping unexpected message received from {0}: {1}", target->Address(), *message);
            return;
        }

        handler(message, target);
    });
}

bool TTestUtil::WaitUntil(function<bool(void)> const & becomeTrue, TimeSpan timeout, TimeSpan interval)
{
    auto timeElapsed = TimeSpan::Zero;
    while (!becomeTrue() && (timeElapsed < timeout))
    {
        Trace.WriteInfo(BoostTestTrace, "WaitUntil: sleep for {0} before next check", interval);
        ::Sleep((DWORD)interval.TotalMilliseconds());
        timeElapsed = timeElapsed + interval;
    }

    return (timeElapsed < timeout);
}

SecuritySettings TTestUtil::CreateTestSecuritySettings(SecurityProvider::Enum provider, ProtectionLevel::Enum protectionLevel)
{
    SecuritySettings securitySettings;
    if (provider == SecurityProvider::Ssl)
    {
        SecurityConfig::X509NameMap remoteNames;
        remoteNames.Add("WinFabricRing.Rings.WinFabricTestDomain.com", "b3 44 9b 01 8d 0f 68 39 a2 c5 d6 2b 5b 6c 6a c8 22 b6 f6 62");
        return CreateX509Settings("CN=WinFabric-Test-SAN1-Bob", remoteNames);
    }

    securitySettings.Test_SetRawValues(provider, protectionLevel);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateSvrX509Settings_ClaimsTest(string const & svrCertThumbprint, string const & clientCertThumbprints)
{
    SecuritySettings securitySettings;
    auto error = SecuritySettings::FromConfiguration(
        "X509",
        X509Default::StoreName(),
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindByThumbprint),
        svrCertThumbprint,
        "",
        formatString(ProtectionLevel::EncryptAndSign),
        clientCertThumbprints,
        SecurityConfig::X509NameMap(),
        SecurityConfig::IssuerStoreKeyValueMap(),
        "",
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509Settings(
    string const & subjectNameFindValue,
    SecurityConfig::X509NameMap const & remoteX509Names)
{
    SecuritySettings securitySettings;
    auto error = SecuritySettings::FromConfiguration(
        "X509",
        X509Default::StoreName(),
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindBySubjectName),
        subjectNameFindValue,
        "",
        formatString(ProtectionLevel::EncryptAndSign),
        "",
        remoteX509Names,
        SecurityConfig::IssuerStoreKeyValueMap(),
        "",
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509Settings2(
    string const & thumbprintFindValue,
    SecurityConfig::X509NameMap const & remoteX509Names)
{
    SecuritySettings securitySettings;
    auto error = SecuritySettings::FromConfiguration(
        "X509",
        X509Default::StoreName(),
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindByThumbprint),
        thumbprintFindValue,
        "",
        formatString(ProtectionLevel::EncryptAndSign),
        "",
        remoteX509Names,
        SecurityConfig::IssuerStoreKeyValueMap(),
        "",
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509Settings(
    string const & subjectNameFindValue,
    SecurityConfig::X509NameMap const & remoteX509Names,
    SecurityConfig::IssuerStoreKeyValueMap const & issuerStores)
{
    SecuritySettings securitySettings;
    auto error = SecuritySettings::FromConfiguration(
        "X509",
        X509Default::StoreName(),
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindBySubjectName),
        subjectNameFindValue,
        "",
        formatString(ProtectionLevel::EncryptAndSign),
        "",
        remoteX509Names,
        issuerStores,
        "",
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509Settings(
    string const & subjectNameFindValue,
    string const & subjectNameFindValueSecondary,
    string const & remoteCommonNames,
    string const & certIssuerThumbprints)
{
    SecurityConfig::X509NameMap remoteNames;
    auto error = remoteNames.AddNames(remoteCommonNames, certIssuerThumbprints);
    ASSERT_IFNOT(error.IsSuccess(), "X509NameMap::AddNames() failed: {0}", error);

    SecuritySettings securitySettings;
    error = SecuritySettings::FromConfiguration(
        "X509",
        X509Default::StoreName(),
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindBySubjectName),
        subjectNameFindValue,
        subjectNameFindValueSecondary,
        formatString(ProtectionLevel::EncryptAndSign),
        "",
        remoteNames,
        SecurityConfig::IssuerStoreKeyValueMap(),
        "",
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509Settings(
    string const & subjectNameFindValue,
    string const & remoteCommonNames,
    string const & certIssuerThumbprints)
{
    return CreateX509Settings(subjectNameFindValue, "", remoteCommonNames, certIssuerThumbprints);
}

SecuritySettings TTestUtil::CreateX509Settings(
    string const & subjectNameFindValue,
    string const & remoteCommonNames,
    SecurityConfig::IssuerStoreKeyValueMap const & certIssuerStores)
{
    SecurityConfig::X509NameMap remoteNames;
    auto error = remoteNames.AddNames(remoteCommonNames, "");
    ASSERT_IFNOT(error.IsSuccess(), "X509NameMap::AddNames() failed: {0}", error);

    SecuritySettings securitySettings;
    error = SecuritySettings::FromConfiguration(
        "X509",
        X509Default::StoreName(),
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindBySubjectName),
        subjectNameFindValue,
        "",
        formatString(ProtectionLevel::EncryptAndSign),
        "",
        remoteNames,
        certIssuerStores,
        "",
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509Settings3(
    string const & findBySubjectAltName,
    string const & remoteCommonNames,
    string const & certIssuerThumbprints)
{
    SecurityConfig::X509NameMap remoteNames;
    auto error = remoteNames.AddNames(remoteCommonNames, certIssuerThumbprints);
    ASSERT_IFNOT(error.IsSuccess(), "X509NameMap::AddNames() failed: {0}", error);

    SecuritySettings securitySettings;
    error = SecuritySettings::FromConfiguration(
        "X509",
        X509Default::StoreName(),
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindByExtension),
        findBySubjectAltName,
        "",
        formatString(ProtectionLevel::EncryptAndSign),
        "",
        remoteNames,
        SecurityConfig::IssuerStoreKeyValueMap(),
        "",
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509Settings2(
    string const & thumbprintFindValue,
    string const & thumbprintFindValueSecondary,
    string const & remoteCommonNames,
    string const & certIssuerThumbprints)
{
    SecurityConfig::X509NameMap remoteNames;
    auto error = remoteNames.AddNames(remoteCommonNames, certIssuerThumbprints);
    ASSERT_IFNOT(error.IsSuccess(), "X509NameMap::AddNames() failed: {0}", error);

    SecuritySettings securitySettings;
    error = SecuritySettings::FromConfiguration(
        "X509",
        X509Default::StoreName(),
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindByThumbprint),
        thumbprintFindValue,
        thumbprintFindValueSecondary,
        formatString(ProtectionLevel::EncryptAndSign),
        "",
        remoteNames,
        SecurityConfig::IssuerStoreKeyValueMap(),
        "",
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509SettingsBySan(
    string const & storeName,
    string const & localDnsNameInSan,
    string const & remoteDnsNameInSan)
{
    SecurityConfig::X509NameMap remoteNames;
    auto error = remoteNames.AddNames(remoteDnsNameInSan, "");
    ASSERT_IFNOT(error.IsSuccess(), "X509NameMap::AddNames() failed: {0}", error);

    SecuritySettings securitySettings;
    error = SecuritySettings::FromConfiguration(
        "X509",
        storeName,
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindByExtension),
        "2.5.29.17:3=" + localDnsNameInSan,
        "",
        formatString(ProtectionLevel::EncryptAndSign),
        "",
        remoteNames,
        SecurityConfig::IssuerStoreKeyValueMap(),
        "",
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509Settings2(
    string const & thumbprintFindValue,
    string const & remoteCommonNames,
    string const & certIssuerThumbprints)
{
    return CreateX509Settings2(thumbprintFindValue, "", remoteCommonNames, certIssuerThumbprints);
}

SecuritySettings TTestUtil::CreateX509SettingsForRoundTripTest(
    string const & certIssuerThumbprints,
    string const & remoteCertThumbprints,
    string const & findValueSecondary,
    X509FindType::Enum findType)
{
    SecuritySettings securitySettings;
    auto error = SecuritySettings::FromConfiguration(
        "X509",
        X509Default::StoreName(),
        formatString(X509Default::StoreLocation()),
        formatString(findType),
        (findType == X509FindType::FindByThumbprint) ? "00 ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 00" : "CN=SomeName",
        findValueSecondary,
        formatString(ProtectionLevel::EncryptAndSign),
        remoteCertThumbprints,
        SecurityConfig::X509NameMap(),
        SecurityConfig::IssuerStoreKeyValueMap(),
        "RemoteName",
        certIssuerThumbprints,
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509SettingsTp(
    string const & localCertThumbprint,
    string const & localCertThumbprint2,
    string const & remoteCertThumbprint,
    string const & remoteCertThumbprint2)
{
    auto remoteTpList = remoteCertThumbprint2.empty() ? remoteCertThumbprint : formatString.L("{0},{1}", remoteCertThumbprint, remoteCertThumbprint2);

    SecuritySettings securitySettings;
    auto error = SecuritySettings::FromConfiguration(
        "X509",
        X509Default::StoreName(),
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindByThumbprint),
        localCertThumbprint,
        localCertThumbprint2,
        formatString(ProtectionLevel::EncryptAndSign),
        remoteTpList,
        SecurityConfig::X509NameMap(),
        SecurityConfig::IssuerStoreKeyValueMap(),
        "",
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509Settings_LoadByName_AuthByThumbprint(
    string const & localCertCommonName,
    string const & remoteCertThumbprint)
{
    SecuritySettings securitySettings;
    auto error = SecuritySettings::FromConfiguration(
        "X509",
        X509Default::StoreName(),
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindBySubjectName),
        localCertCommonName,
        "",
        formatString(ProtectionLevel::EncryptAndSign),
        remoteCertThumbprint,
        SecurityConfig::X509NameMap(),
        SecurityConfig::IssuerStoreKeyValueMap(),
        "",
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509Settings_CertRefresh_IssuerStore(
    string const & localCertCommonName,
    SecurityConfig::X509NameMap remoteNames,
    SecurityConfig::IssuerStoreKeyValueMap issuerStores)
{
    SecuritySettings securitySettings;

    auto error = SecuritySettings::FromConfiguration(
        "X509",
        "Root",
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindBySubjectName),
        localCertCommonName,
        "",
        formatString(ProtectionLevel::EncryptAndSign),
        "",
        remoteNames,
        issuerStores,
        "",
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateX509SettingsExpired(string const & remoteCommonNames)
{
    SecuritySettings securitySettings;
    auto error = SecuritySettings::FromConfiguration(
        "X509",
        "Root",
        formatString(X509Default::StoreLocation()),
        formatString(X509FindType::FindBySubjectName),
        "CN=WinFabric-Test-Expired",
        "",
        formatString(ProtectionLevel::EncryptAndSign),
        "",
        SecurityConfig::X509NameMap(),
        SecurityConfig::IssuerStoreKeyValueMap(),
        remoteCommonNames,
        "",
        "",
        "",
        securitySettings);

    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::FromConfiguration failed: {0}", error);
    return securitySettings;
}

SecuritySettings TTestUtil::CreateKerbSettings()
{
    SecuritySettings securitySettings;
    auto error = SecuritySettings::CreateKerberos("", "", formatString(ProtectionLevel::EncryptAndSign), securitySettings);
    ASSERT_IFNOT(error.IsSuccess(), "SecuritySettings::CreateKerberos failed: {0}", error);
    return securitySettings;
}

void TTestUtil::CopyFile(std::string const & file, std::string const & srcDir, std::string & destDir)
{
    string srcPath = Path::Combine(srcDir, Path::GetFileName(file));
    string destPath = Path::Combine(destDir, Path::GetFileName(file));
    auto error = File::Copy(srcPath, destPath, true);
    Invariant(error.IsSuccess());
}

void TTestUtil::CopyFiles(std::vector<std::string> const & files, std::string const & srcDir, std::string & destDir)
{
    for (auto const & file : files)
    {
        TTestUtil::CopyFile(file, srcDir, destDir);
    }
}

void TTestUtil::ReduceTracingInFreBuild()
{
#ifndef DBG
    TraceProvider::GetSingleton()->SetDefaultLevel(TraceSinkType::TextFile, LogLevel::Enum::Warning);
#endif
}

void TTestUtil::RecoverTracingInFreBuild()
{
#ifndef DBG
    TraceProvider::GetSingleton()->SetDefaultLevel(TraceSinkType::TextFile, LogLevel::Enum::Noise);
#endif
}

void TTestUtil::DisableSendThrottling()
{
    TransportConfig::GetConfig().DefaultSendQueueSizeLimit = 0;
    TransportConfig::GetConfig().DefaultOutgoingMessageExpiration = TimeSpan::Zero;
}

#ifndef PLATFORM_UNIX

void TTestUtil::CreateRemoteProcessWithPsExec(
    string const & computer, 
    string const & cmdline,
    string const & workDir,
    DWORD flags)
{
    //vector<string> tokens;
    //StringUtility::Split<string>(cmdline, tokens, " ");
    //string outputFile = formatString.L("{0}.out.txt", Path::GetFileName(tokens.front()));

    //string cmd = formatString.L("psexec -c -w {0} {1} {2} > {3}", workDir, computer, cmdline, outputFile);
    auto cmd = formatString.L("psexec -c -f -w {0} {1} {2}", workDir, computer, cmdline);
    wcout << "TTestUtil::CreateRemoteProcess: " << cmd << endl;
    HandleUPtr processHandle, threadHandle;
    vector<char> environmentBlock;
    auto error = ProcessUtility::CreateProcess(
        cmd,
        workDir,
        environmentBlock,
        flags,
        processHandle,
        threadHandle);

    Invariant(error.IsSuccess());
}

static BOOL CALLBACK ComSecInitConfigFunction(PINIT_ONCE, PVOID, PVOID *)
{
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    Invariant(SUCCEEDED(hr));

    hr = CoInitializeSecurity(
        NULL,
        -1,                          // COM negotiates service
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );

    Invariant(SUCCEEDED(hr));
    return TRUE;
}

static INIT_ONCE comSecInitOnceFlag = INIT_ONCE_STATIC_INIT; // CoInitializeSecurity needs to be called exactly once

void TTestUtil::CreateRemoteProcess(
    string const & computer,
    string const & cmdline,
    string const & workDir,
    DWORD/*flags*/)
{
    PVOID lpContext = nullptr;;
    Invariant(::InitOnceExecuteOnce(&comSecInitOnceFlag, ComSecInitConfigFunction, nullptr, &lpContext));

    ComPointer<IWbemLocator> locator;
    HRESULT hr = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        (LPVOID *)(locator.InitializationAddress()));

    Invariant(SUCCEEDED(hr));

    ComPointer<IWbemServices> services;

    // Connect to remote root\default namespace with the current user.
    string remoteNamespace = computer + "\\root\\cimv2";
    wcout << "remote namespace = " << remoteNamespace << endl;
    BSTR rns = SysAllocString(remoteNamespace.c_str());
    KFinally([=] { SysFreeString(rns); });

    hr = locator->ConnectServer(
        rns,
        NULL,       // User name 
        NULL,       // User password
        0,         // Locale 
        NULL,     // Security flags
        0,         // Authority 
        0,        // Context object 
        services.InitializationAddress());   // IWbemServices proxy

    Invariant(SUCCEEDED(hr));

    // Set the proxy so that impersonation of the client occurs.
    hr = CoSetProxyBlanket(
        services.GetRawPointer(),
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE);

    Invariant(SUCCEEDED(hr));

    BSTR className = SysAllocString("Win32_Process");
    KFinally([=] { SysFreeString(className); });
    BSTR methodName = SysAllocString("Create");
    KFinally([=] { SysFreeString(methodName); });

    ComPointer<IWbemClassObject> classCPtr;
    hr = services->GetObject(className, 0, NULL, classCPtr.InitializationAddress(), NULL);
    Invariant(SUCCEEDED(hr));

    ComPointer<IWbemClassObject> paramsDefinition;
    hr = classCPtr->GetMethod(methodName, 0, paramsDefinition.InitializationAddress(), NULL);
    Invariant(SUCCEEDED(hr));

    ComPointer<IWbemClassObject> classInstance;
    hr = paramsDefinition->SpawnInstance(0, classInstance.InitializationAddress());
    Invariant(SUCCEEDED(hr));

    // Create the values for the in parameters
    VARIANT varCommand;
    varCommand.vt = VT_BSTR;
    varCommand.bstrVal = _bstr_t(cmdline.c_str());

    // Store the value for the in parameters
    hr = classInstance->Put("CommandLine", 0, &varCommand, 0);
    Invariant(SUCCEEDED(hr));

    VARIANT varCommand2;
    varCommand2.vt = VT_BSTR;
    varCommand2.bstrVal = _bstr_t(workDir.c_str());

    hr = classInstance->Put("CurrentDirectory", 0, &varCommand2, 0);
    Invariant(SUCCEEDED(hr));

    // Execute Method
    ComPointer<IWbemClassObject> output;
    hr = services->ExecMethod(
        className,
        methodName,
        0,
        NULL,
        classInstance.GetRawPointer(),
        output.InitializationAddress(),
        NULL);

    Invariant(SUCCEEDED(hr));
}

#endif


// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{

    class FabricEnvironment
    {

    public:
        static Common::GlobalString FabricDataRootEnvironmentVariable;
        static Common::GlobalString FabricRootEnvironmentVariable;
        static Common::GlobalString FabricBinRootEnvironmentVariable;
        static Common::GlobalString FabricLogRootEnvironmentVariable;
        static Common::GlobalString EnableCircularTraceSessionEnvironmentVariable;
        static Common::GlobalString FabricCodePathEnvironmentVariable;
        static Common::GlobalString HostedServiceNameEnvironmentVariable;
        static Common::GlobalString FileConfigStoreEnvironmentVariable;
        static Common::GlobalString PackageConfigStoreEnvironmentVariable;
        static Common::GlobalString SettingsConfigStoreEnvironmentVariable;
        static Common::GlobalString RemoveNodeConfigurationEnvironmentVariable;

        static Common::GlobalString LogDirectoryRelativeToDataRoot;
        static Common::GlobalString WindowsFabricRemoveNodeConfigurationRegValue;

        static Common::GlobalString FabricDnsServerIPAddressEnvironmentVariable;

        static Common::ErrorCode GetStoreTypeAndLocation(HMODULE dllModule,
            __out ConfigStoreType::Enum & storeType,
            __out std::string & storeLocation);

        static Common::ErrorCode GetInfrastructureManifest(
            std::string const &,
            __out std::string &);

        static Common::ErrorCode GetFabricRoot(
            __out std::string &);

        static Common::ErrorCode GetFabricRoot(
            LPCSTR machineName, __out std::string &);

        static Common::ErrorCode GetFabricBinRoot(
            __out std::string &);

        static Common::ErrorCode GetFabricBinRoot(
            LPCSTR machineName, __out std::string &);

        static Common::ErrorCode GetFabricCodePath(
            __out std::string &);

        static Common::ErrorCode GetFabricCodePath(
            LPCSTR machineName, __out std::string &);

        static Common::ErrorCode GetFabricCodePath(
            HMODULE dllModule, __out std::string & fabricCodePath);

        static Common::ErrorCode GetFabricCodePath(
            HMODULE dllModule, LPCSTR machineName, __out std::string & fabricCodePath);

        static Common::ErrorCode GetFabricDataRoot(
            __out std::string &);

        static Common::ErrorCode GetFabricDataRoot(
            LPCSTR machineName, __out std::string &);

        static Common::ErrorCode GetFabricLogRoot(
            __out std::string &);

        static Common::ErrorCode GetFabricLogRoot(
            LPCSTR machineName, __out std::string &);

        static Common::ErrorCode GetEnableCircularTraceSession(
            __out BOOLEAN &);

        static Common::ErrorCode GetEnableCircularTraceSession(
            LPCSTR machineName, __out BOOLEAN &);

        static Common::ErrorCode GetFabricDnsServerIPAddress(
            __out std::string &);

        static Common::ErrorCode GetFabricDnsServerIPAddress(
            LPCSTR machineName, __out std::string &);

        static Common::ErrorCode SetFabricRoot(
            const std::string &);

        static Common::ErrorCode SetFabricRoot(
            const std::string &, LPCSTR machineName);

        static Common::ErrorCode SetFabricBinRoot(
            const std::string &);

        static Common::ErrorCode SetFabricBinRoot(
            const std::string &, LPCSTR machineName);

        static Common::ErrorCode SetFabricCodePath(
            const std::string &);

        static Common::ErrorCode SetFabricCodePath(
            const std::string &, LPCSTR machineName);

        static Common::ErrorCode SetFabricDataRoot(
            const std::string &);

        static Common::ErrorCode SetFabricDataRoot(
            const std::string &, LPCSTR machineName);

        static Common::ErrorCode SetFabricLogRoot(
            const std::string &);

        static Common::ErrorCode SetFabricLogRoot(
            const std::string &, LPCSTR machineName);

        static Common::ErrorCode SetEnableCircularTraceSession(
            BOOLEAN, LPCSTR machineName);

        static Common::ErrorCode SetEnableUnsupportedPreviewFeatures(
            BOOLEAN, LPCSTR machineName);

        static Common::ErrorCode GetEnableUnsupportedPreviewFeatures(
            __out bool &);

        static Common::ErrorCode SetIsSFVolumeDiskServiceEnabled(
            BOOLEAN, LPCSTR machineName);

        static Common::ErrorCode GetIsSFVolumeDiskServiceEnabled(
            __out bool &);

        static void GetFabricTracesTestKeyword(
            __out std::string &);

        static Common::ErrorCode GetRemoveNodeConfigurationValue(__out std::string &);

        static Common::ErrorCode GetFabricVersion(__out std::string &);

#if defined(PLATFORM_UNIX)
        static Common::ErrorCode GetLinuxPackageManagerType(__out Common::LinuxPackageManagerType::Enum &);
#endif

    private:
        static Common::ErrorCode GetRegistryKeyHelper(
            const std::string &, const std::string &, ErrorCode, LPCSTR, __out std::string &);

        static Common::ErrorCode GetRegistryKeyHelper(
            const std::string &, const std::string &, ErrorCode, LPCSTR, __out DWORD &);

        static Common::ErrorCode GetRegistryKeyHelper(
            RegistryKey &, const std::string &, ErrorCode, __out std::string &);

        static Common::ErrorCode GetRegistryKeyHelper(
            RegistryKey &, const std::string &, ErrorCode, __out DWORD &);

        static Common::ErrorCode SetRegistryKeyHelper(
            const std::string &, const std::string &, LPCSTR);

        static Common::ErrorCode SetRegistryKeyHelper(
            const std::string &, DWORD, LPCSTR);

        static Common::ErrorCode SetRegistryKeyHelper(
            RegistryKey &, const std::string &, const std::string &);

        static Common::ErrorCode SetRegistryKeyHelper(
            RegistryKey &, const std::string &, DWORD);

        static Common::ErrorCode GetEtcConfigValue(
            const std::string &, ErrorCode, __out std::string &);

        static Common::ErrorCode GetEtcConfigValue(
            const std::string &, ErrorCode, __out DWORD &);

        static Common::ErrorCode SetEtcConfigValue(
            const std::string &, const std::string &);

        static Common::ErrorCode SetEtcConfigValue(
            const std::string &, DWORD);

    private:
#if defined(PLATFORM_UNIX)
        static Common::LinuxPackageManagerType::Enum packageManagerType_;
#endif
    };
}

// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class FabricConstants
    {
    public:
        static Common::GlobalString WindowsFabricAllowedUsersGroupName;
        static Common::GlobalString WindowsFabricAllowedUsersGroupComment;
        static Common::GlobalString WindowsFabricAdministratorsGroupName;
        static Common::GlobalString WindowsFabricAdministratorsGroupComment;
        static Common::GlobalString CurrentClusterManifestFileName;
        static Common::GlobalString InfrastructureManifestFileName;
        static Common::GlobalString TargetInformationFileName;
        static Common::GlobalString FabricFolderName;
        static Common::GlobalString FabricDataFolderName;
        static Common::GlobalString DefaultFileConfigStoreLocation;
        static Common::GlobalString DefaultPackageConfigStoreLocation;
        static Common::GlobalString DefaultSettingsConfigStoreLocation;
        static Common::GlobalString DefaultClientSettingsConfigStoreLocation;
        static Common::GlobalString FabricGatewayName;
        static Common::GlobalString FabricApplicationGatewayName;
        static Common::GlobalString AppsFolderName;

        static Common::GlobalString FabricRegistryKeyPath;
        static Common::GlobalString FabricEtcConfigPath;
        static Common::GlobalString FabricRootRegKeyName;
        static Common::GlobalString FabricBinRootRegKeyName;
        static Common::GlobalString FabricCodePathRegKeyName;
        static Common::GlobalString FabricDataRootRegKeyName;
        static Common::GlobalString FabricLogRootRegKeyName;
        static Common::GlobalString EnableCircularTraceSessionRegKeyName;
        static Common::GlobalString EnableUnsupportedPreviewFeaturesRegKeyName;
        static Common::GlobalString IsSFVolumeDiskServiceEnabledRegKeyName;
        static Common::GlobalString UseFabricInstallerSvcKeyName;

        static Common::GlobalString FabricDnsServerIPAddressRegKeyName;

        static DWORD MaxFileSize;
    };
}

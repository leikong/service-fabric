// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace Common;
using namespace std;

GlobalString FabricConstants::WindowsFabricAllowedUsersGroupName = make_global<string>("ServiceFabricAllowedUsers");
GlobalString FabricConstants::WindowsFabricAllowedUsersGroupComment = make_global<string>("Members of this group are allowed to communicate with Service Fabric. Service Fabric service host processes must run under an account that is a member of this group.");
GlobalString FabricConstants::WindowsFabricAdministratorsGroupName = make_global<string>("ServiceFabricAdministrators");
GlobalString FabricConstants::WindowsFabricAdministratorsGroupComment = make_global<string>("Members of this group are allowed to administer a Service Fabric cluster.");


GlobalString FabricConstants::FabricRegistryKeyPath = make_global<string>("Software\\Microsoft\\Service Fabric");
GlobalString FabricConstants::FabricEtcConfigPath = make_global<string>("/etc/servicefabric/");
GlobalString FabricConstants::FabricRootRegKeyName = make_global<string>("FabricRoot");
GlobalString FabricConstants::FabricBinRootRegKeyName = make_global<string>("FabricBinRoot");
GlobalString FabricConstants::FabricCodePathRegKeyName = make_global<string>("FabricCodePath");
GlobalString FabricConstants::FabricDataRootRegKeyName = make_global<string>("FabricDataRoot");
GlobalString FabricConstants::FabricLogRootRegKeyName = make_global<string>("FabricLogRoot");
GlobalString FabricConstants::EnableCircularTraceSessionRegKeyName = make_global<string>("EnableCircularTraceSession");
GlobalString FabricConstants::EnableUnsupportedPreviewFeaturesRegKeyName = make_global<string>("EnableUnsupportedPreviewFeatures");
GlobalString FabricConstants::IsSFVolumeDiskServiceEnabledRegKeyName = make_global<string>("IsSFVolumeDiskServiceEnabled");
GlobalString FabricConstants::UseFabricInstallerSvcKeyName = make_global<string>("UseFabricInstallerSvc");

GlobalString FabricConstants::AppsFolderName = make_global<string>("_App");
// Current cluster manifest name
GlobalString FabricConstants::CurrentClusterManifestFileName = make_global<string>("ClusterManifest.current.xml");

GlobalString FabricConstants::InfrastructureManifestFileName = make_global<string>("InfrastructureManifest.xml");
GlobalString FabricConstants::TargetInformationFileName = make_global<string>("TargetInformation.xml");
GlobalString FabricConstants::FabricFolderName = make_global<string>("Fabric");
GlobalString FabricConstants::FabricDataFolderName = make_global<string>("Fabric.Data");
GlobalString FabricConstants::FabricGatewayName = make_global<string>("FabricGateway.exe");;
GlobalString FabricConstants::FabricApplicationGatewayName = make_global<string>("FabricApplicationGateway.exe");;

// Default Store locations
GlobalString FabricConstants::DefaultFileConfigStoreLocation = make_global<string>("FabricConfig.cfg");
GlobalString FabricConstants::DefaultPackageConfigStoreLocation = make_global<string>("Fabric.Package.Current.xml");
GlobalString FabricConstants::DefaultSettingsConfigStoreLocation = make_global<string>("FabricHostSettings.xml");
GlobalString FabricConstants::DefaultClientSettingsConfigStoreLocation = make_global<string>("FabricClientSettings.xml");

// Async File Read Constants
DWORD FabricConstants::MaxFileSize = 1073741824;

// DnsService related constants
GlobalString FabricConstants::FabricDnsServerIPAddressRegKeyName = make_global<string>("FabricDnsServerIPAddress");

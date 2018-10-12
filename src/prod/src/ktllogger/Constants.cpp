// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

//
// Do not change application shared log location or name to preserve backwards compability with old V2 managed stack
//
std::string const KtlLogger::Constants::DefaultApplicationSharedLogIdString("3CA2CCDA-DD0F-49c8-A741-62AAC0D4EB62");
Common::Guid const KtlLogger::Constants::DefaultApplicationSharedLogId(DefaultApplicationSharedLogIdString);
std::string const KtlLogger::Constants::DefaultApplicationSharedLogName("replicatorshared.log");
std::string const KtlLogger::Constants::DefaultApplicationSharedLogSubdirectory("ReplicatorLog");

std::string const KtlLogger::Constants::DefaultSystemServicesSharedLogIdString("15B07384-83C5-411D-9942-1655520C77C6");
Common::Guid const KtlLogger::Constants::DefaultSystemServicesSharedLogId(DefaultSystemServicesSharedLogIdString);
std::string const KtlLogger::Constants::DefaultSystemServicesSharedLogName("sysshared.log");
std::string const KtlLogger::Constants::DefaultSystemServicesSharedLogSubdirectory("SysLog");
std::string const KtlLogger::Constants::NullGuidString("00000000-0000-0000-0000-000000000000");

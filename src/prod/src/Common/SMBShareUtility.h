// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{    
    class SMBShareUtility
    {
    public:
        static GlobalString WindowsFabricSMBShareRemark;        
        
        static GlobalString NullSessionSharesRegistryValue;
        static GlobalString LanmanServerParametersRegistryPath;

        static GlobalString LsaRegistryPath;
        static GlobalString EveryoneIncludesAnonymousRegistryValue;

        static ErrorCode EnsureServerServiceIsRunning();

        static ErrorCode CreateShare(
            std::string const & localPath, 
            std::string const & shareName, 
            SecurityDescriptorSPtr const & securityDescriptor);
        
        static ErrorCode DeleteShare(std::string const & shareName);
        
        static std::set<std::string> DeleteOrphanShares();

        static ErrorCode EnableAnonymousAccess(
            std::string const & localPath,
            std::string const & shareName,
            DWORD accessMask,
            SidSPtr const & anonymousSid,
			TimeSpan const & timeout);

        static ErrorCode DisableAnonymousAccess(std::string const & localPath, std::string const & shareName, TimeSpan const & timeout);
    };
}

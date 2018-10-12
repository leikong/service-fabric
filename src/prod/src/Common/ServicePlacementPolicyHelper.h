// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    struct ServicePlacementPolicyHelper
    {
    public:

        static HRESULT PolicyDescriptionToPlacementConstraints(
            __in const FABRIC_PLACEMENT_POLICY_TYPE & policyType, 
            __in const std::string & domainName,
            __out std::string & placementConstraints
            );

        static HRESULT PolicyDescriptionToDomainName(
            __in const FABRIC_SERVICE_PLACEMENT_POLICY_DESCRIPTION & placementPolicyDescription,
            __out std::string & domainName
            );

        static bool IsValidPlacementPolicyType(const FABRIC_PLACEMENT_POLICY_TYPE & policyType);

    };
}

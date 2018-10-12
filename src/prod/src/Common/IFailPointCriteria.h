// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class IFailPointCriteria
    {
    public:
        virtual bool Match(FailPointContext & context) =0; 
        virtual std::string ToString()=0;
    };
}

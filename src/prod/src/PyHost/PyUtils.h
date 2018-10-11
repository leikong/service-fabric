// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace PyHost
{
    class PyUtils
    {
    public:
        static void ThrowOnFailure(
            std::string const & moduleName, 
            std::string const & funcName,
            std::string const & operation);

    private:
        static void CheckPyErrAndThrow(std::string && debugTag);
    };

}

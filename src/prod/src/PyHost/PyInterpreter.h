// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace PyHost
{
    class PyInterpreter
    {
    public:
        //
        // C++ -> Python
        //
        Common::ErrorCode Execute(
            std::wstring const & moduleName, 
            std::wstring const & funcName, 
            std::vector<std::wstring> const & args);

    public:
        //
        // Python -> C++
        //
        using SetNodeIdOwnershipCallback = std::function<Common::ErrorCode(std::wstring const &, std::wstring const &)>;

        void Register_SetNodeIdOwnership(SetNodeIdOwnershipCallback const &);
    };
}

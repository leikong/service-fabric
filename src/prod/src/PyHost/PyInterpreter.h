// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace PyHost
{

#define DECLARE_PY_CALLBACK( Name, ... ) \
    using Name##Callback = std::function<Common::ErrorCode( __VA_ARGS__ )>; \
    void Register_##Name(Name##Callback const &); \

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

        Common::ErrorCode ExecuteIfFound(
            std::wstring const & moduleName, 
            std::wstring const & funcName, 
            std::vector<std::wstring> const & args);

    public:
        //
        // Python -> C++
        //

        DECLARE_PY_CALLBACK( SetNodeIdOwnership, std::wstring const &, std::wstring const & )
        DECLARE_PY_CALLBACK( Broadcast, std::wstring const & )
        DECLARE_PY_CALLBACK( Query, std::wstring const &, std::wstring & )
    };
}

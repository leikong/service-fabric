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
            std::string const & moduleName, 
            std::string const & funcName, 
            std::vector<std::string> const & args);

        Common::ErrorCode ExecuteIfFound(
            std::string const & moduleName, 
            std::string const & funcName, 
            std::vector<std::string> const & args);

    public:
        //
        // Python -> C++
        //

        DECLARE_PY_CALLBACK( SetNodeIdOwnership, std::string const &, std::string const & )
        DECLARE_PY_CALLBACK( Broadcast, std::string const & )
        DECLARE_PY_CALLBACK( Query, std::string const &, std::string & )
    };
}

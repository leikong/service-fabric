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

#define TRY_PARSE_PY_STRING( Input ) \
        wstring parsed_##Input; \
        { \
            auto parseError = StringUtility::LpcwstrToWstring2( Input, false, parsed_##Input ); \
            if (!parseError.IsSuccess()) \
            { \
                auto msg = wformatString("Function '{0}': Parse({1}, __out {2}) failed: {3}", __FUNCTION__, #Input, parsed_##Input, parseError); \
                Trace.WriteWarning("PyUtils", "{0}", msg); \
                PyErr_SetString(PyExc_RuntimeError, StringUtility::Utf16ToUtf8(msg).c_str()); \
                return NULL; \
            } \
        } \

}

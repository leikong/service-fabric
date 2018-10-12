//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#pragma once
#include <dbghelp.h>

namespace Common
{
    class SymbolPathInitializer
    {
    public:
        std::string InitializeSymbolPaths()
        {
            std::string symPathOut;

#ifndef PLATFORM_UNIX
            string symPath;
            Environment::GetEnvironmentVariable("_NT_SYMBOL_PATH", symPath, NOTHROW());
            string symPathAlt;
            Environment::GetEnvironmentVariable("_NT_ALTERNATE_SYMBOL_PATH", symPathAlt, NOTHROW());
            auto symPathsW = formatString.L("{0};{1};{2};{3}", Environment::GetExecutablePath(), Directory::GetCurrentDirectory(), symPath, symPathAlt);
            Utf16ToUtf8NotNeeded2(symPathsW, symPathOut);

            SymInitialize(GetCurrentProcess(), symPathOut.c_str(), TRUE);
#endif

            return symPathOut;
        }
    };
    
}
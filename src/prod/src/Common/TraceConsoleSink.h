// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class TraceConsoleSink
    {
        DENY_COPY(TraceConsoleSink);

    public:
        static void Write(LogLevel::Enum level, std::string const & data);

        static void Write(DWORD color, std::string const & data);

    private:
        static TraceConsoleSink* Singleton;

        TraceConsoleSink();

        void PrivateWrite(std::string const & data);
        void PrivateWrite(DWORD color, std::string const & data);

        RealConsole console_;
        DWORD color_;
        RwLock lock_;
    };
}

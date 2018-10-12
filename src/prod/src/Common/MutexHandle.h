// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class MutexHandle;
    typedef std::unique_ptr<MutexHandle> MutexHandleUPtr;
    typedef std::shared_ptr<MutexHandle> MutexHandleSPtr;

    class MutexHandle : TextTraceComponent<TraceTaskCodes::Common>
    {
        DENY_COPY(MutexHandle)

    public:
        //name must be non-empty, otherwise, RwLock should be used
        static MutexHandleSPtr CreateSPtr(std::string const & name);
        static MutexHandleUPtr CreateUPtr(std::string const & name);

        explicit MutexHandle(std::string const & name);
        ~MutexHandle();

        ErrorCode WaitOne(TimeSpan timeout = TimeSpan::MaxValue);
        void Release();

    private:
        std::string const id_;

#ifdef PLATFORM_UNIX
        static std::string NormalizeName(std::string const & name);
        static std::string GetMutexPath(std::string const & name);

        ExclusiveFile handle_;
#else
        ErrorCode createStatus_;
        HANDLE handle_;
#endif
    };
}

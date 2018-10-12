// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    template <bool IsReaderLock>
    class FileLock : public TextTraceComponent<TraceTaskCodes::Common>
    {
    public:
        FileLock(std::string const & path);
        virtual ~FileLock();

        virtual ErrorCode Acquire(TimeSpan timeout = TimeSpan::Zero);
        virtual bool Release();

    protected:

#ifdef PLATFORM_UNIX

        std::string const id_;
        ErrorCode openStatus_;
        std::string path_;
        int file_;

#else

        static std::string GetReaderLockPath(std::string const & path);
        static std::string GetWriterLockPath(std::string const & path);        
        static const std::string ReaderLockExtension;
        static const std::string WriterLockExtension;

        ErrorCode AcquireImpl();

        std::unique_ptr<File> readFile_;
        std::unique_ptr<File> writeFile_;
        std::string path_;
        std::string operation_;

#endif
    };

}

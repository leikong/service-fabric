// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class ErrorCode;

    class Directory
    {
        DENY_COPY(Directory);
    public:
        static inline void CreateDirectory(
            std::string const & path)
        {
            Directory::Create(path);
        }

        static void Create(
            std::string const & path);

        static ErrorCode Create2(std::string const & path);

        static ErrorCode Delete(
            std::string const & path,
            bool recursive);

        static ErrorCode Delete(
            std::string const & path,
            bool recursive,
            bool deleteReadOnlyFiles);

        static ErrorCode Delete_WithRetry(
            std::string const & path, 
            bool recursive, 
            bool deleteReadOnlyFiles);

        static bool Exists(
            std::string const & path);

        static ErrorCode Exists(
            std::string const & path,
            __out bool& exists);

        static std::string GetCurrentDirectory();

        static void SetCurrentDirectory(
            std::string const & path);

        static ErrorCode Copy(std::string const & src, std::string const & dest, bool overWrite);

        static ErrorCode SafeCopy(
            std::string const & src,
            std::string const & dest,
            bool overWrite,
            Common::TimeSpan const timeout = TimeSpan::Zero);

        static ErrorCode Rename(std::string const & src, std::string const & dest, bool const overwrite = false);

        static ErrorCode Rename_WithRetry(std::string const & src, std::string const & dest, bool const overwrite = false);

        static std::vector<std::string> GetSubDirectories(std::string const & path);
        static std::vector<std::string> GetSubDirectories(std::string const & path, std::string const & pattern, bool fullPath, bool topDirectoryOnly);

        static std::vector<std::string> GetFiles(std::string const & path);
        static std::vector<std::string> GetFiles(std::string const & path, std::string const & pattern, bool fullPath, bool topDirectoryOnly);

        static std::vector<std::string> Find(std::string const & path, std::string const & filePattern, uint expectedFileCount, bool findRecursively);
        static std::map<std::string, WIN32_FIND_DATA> FindWithFileProperties(std::string const & path, std::string const & filePattern, uint expectedFileCount, bool findRecursively);

        static ErrorCode GetLastWriteTime(std::string const & path, __out DateTime & lastWriteTime);

        static ErrorCode Echo(std::string const & src, std::string const & dest, Common::TimeSpan const timeout = TimeSpan::Zero);

        static bool IsSymbolicLink(std::string const & path);

        static int64 GetSize(std::string const & path);

        static ErrorCode CreateArchive(std::string const & src, std::string const & dest);
        static ErrorCode ExtractArchive(std::string const & src, std::string const & dest);

    private:
        Directory() { } // deny construction

#if defined(PLATFORM_UNIX)
        static int TryZipDirectory(char const * srcDirectoryPath, char const * destZipFileName, char * errorMessageBuffer, int errorMessageBufferSize);
        static int TryUnzipDirectory(char const * srcFileName, char const * destDirectoryPath, char * errorMessageBuffer, int errorMessageBufferSize);
#endif

        static ErrorCode DeleteInternal(
            std::string const & path,
            bool recursive,
            bool deleteReadOnlyFiles);

        static ErrorCode MoveFile(const std::string & src, const std::string & dest, bool overwrite);

        static bool HasFileLockExtension(std::string const & fileName);

        static std::vector<std::string> GetSubDirectoriesInternal(std::string const & path, std::string const & pattern, bool fullPath);
        static std::vector<std::string> GetFilesInternal(std::string const & path, std::string const & pattern, bool fullPath);

        static void GetSubDirectoriesHelper(std::string const & path, std::string const & pattern, bool fullPath, std::vector<std::string> & result);
        static void GetFilesHelper(std::string const & path, std::string const & pattern, bool fullPath, std::vector<std::string> & result);

        static const int DirectoryOperationMaxRetryCount;
        static const int DirectoryOperationRetryIntervalInMillis;
    };
}

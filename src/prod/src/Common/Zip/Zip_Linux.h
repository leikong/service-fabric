// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#if defined(PLATFORM_UNIX)
#pragma once

namespace Common
{
    class Zip
    {
        DENY_COPY(Zip)
    public:
        static int TryZipDirectory(char const * srcDirectoryPath, char const * destZipFileName, char * errorMessageBuffer, int errorMessageBufferSize);
        static int TryUnzipDirectory(char const * srcFileName, char const * destDirectoryPath, char * errorMessageBuffer, int errorMessageBufferSize);
        
    private:
        static bool IsLargeFile(std::istream & inputStream);
        static int ArchiveEntry(zipFile const & zipFile, std::string const & filename, std::string const & filenameFullPath, char * errorMessageBuffer, int const errorMessageBufferSize);
        static int ExtractCurrentEntry(unzFile const & zipFile, char const * destDirectoryPath, char * errorMessageBuffer, int const errorMessageBufferSize);
        static void GetFileDate(std::string filename, tm_zip & tmzip);
        static void ChangeFileDate(char const * filename, tm_unz tmuDate);
        static void CopyFromString(std::string const & str, char * out, int outSize);
    };
}
#endif

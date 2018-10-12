// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

#include <boost/test/unit_test.hpp>
#include "Common/boost-taef.h"

#include "Path.h"

namespace Common
{
    class TestPaths
    {
    };

    BOOST_FIXTURE_TEST_SUITE(TestPathsSuite,TestPaths)

    BOOST_AUTO_TEST_CASE(TestEscapedCombinePath)
    {
#if !defined(PLATFORM_UNIX)
        std::string path1 = "c:\\Some directory location here\\logDir";
        std::string path2 = "MyFileName.pid";

        std::string combinedPath = Path::Combine(path1, path2, true /*escapePath*/);
        VERIFY_IS_TRUE(combinedPath == "\"c:\\Some directory location here\\logDir\\MyFileName.pid\"");

        combinedPath = Path::Combine(path1, path2, false /*escapePath*/);
        VERIFY_IS_TRUE(combinedPath == "c:\\Some directory location here\\logDir\\MyFileName.pid");

        std::string path1MultiByte = "c:\\Some directory location here\\logDir";
        std::string path2MultiByte = "MyFileName.pid";

        std::string combinedPathMultiByte = Path::Combine(path1MultiByte, path2MultiByte, true /*escapePath*/);
        VERIFY_IS_TRUE(combinedPathMultiByte == "\"c:\\Some directory location here\\logDir\\MyFileName.pid\"");

        combinedPathMultiByte = Path::Combine(path1MultiByte, path2MultiByte, false /*escapePath*/);
        VERIFY_IS_TRUE(combinedPathMultiByte == "c:\\Some directory location here\\logDir\\MyFileName.pid");

        path1 = "";
        path2 = "c:\\Some directory location here\\logDir";
        combinedPath = Path::Combine(path1, path2, true /*escapePath*/);
        VERIFY_IS_TRUE(combinedPath == "\"c:\\Some directory location here\\logDir\"");

        path1 = "c:\\Some directory location here\\logDir";
        path2 = "";
        combinedPath = Path::Combine(path1, path2, true /*escapePath*/);
        VERIFY_IS_TRUE(combinedPath == "\"c:\\Some directory location here\\logDir\"");

        path1 = "";
        path2 = "";
        combinedPath = Path::Combine(path1, path2, true /*escapePath*/);
        VERIFY_IS_TRUE(combinedPath == "");
#else

        std::string path1 = "//Some_directory_location_here//logDir";
        std::string path2 = "MyFileName.pid";

        std::string combinedPath = Path::Combine(path1, path2, true /*escapePath*/);
        VERIFY_IS_TRUE(combinedPath == "\"//Some_directory_location_here//logDir/MyFileName.pid\"");

        combinedPath = Path::Combine(path1, path2, false /*escapePath*/);
        VERIFY_IS_TRUE(combinedPath == "//Some_directory_location_here//logDir/MyFileName.pid");

        std::string path1MultiByte = "//Some_directory_location_here//logDir";
        std::string path2MultiByte = "MyFileName.pid";

        std::string combinedPathMultiByte = Path::Combine(path1MultiByte, path2MultiByte, true /*escapePath*/);
        VERIFY_IS_TRUE(combinedPathMultiByte == "\"//Some_directory_location_here//logDir/MyFileName.pid\"");

        combinedPathMultiByte = Path::Combine(path1MultiByte, path2MultiByte, false /*escapePath*/);
        VERIFY_IS_TRUE(combinedPathMultiByte == "//Some_directory_location_here//logDir/MyFileName.pid");

        path1 = "";
        path2 = "//Some_directory_location_here//logDir";
        combinedPath = Path::Combine(path1, path2, true /*escapePath*/);
        VERIFY_IS_TRUE(combinedPath == "\"//Some_directory_location_here//logDir\"");

        path1 = "//Some_directory_location_here//logDir";
        path2 = "";
        combinedPath = Path::Combine(path1, path2, true /*escapePath*/);
        VERIFY_IS_TRUE(combinedPath == "\"//Some_directory_location_here//logDir\"");

        path1 = "";
        path2 = "";
        combinedPath = Path::Combine(path1, path2, true /*escapePath*/);
        VERIFY_IS_TRUE(combinedPath == "");

#endif
    }

    BOOST_AUTO_TEST_CASE(ChangeExtensionTest)
    {
        std::string path;

        path = "c:\\asdfsd\\pr.exe";
        Path::ChangeExtension(path, ".com");
        VERIFY_IS_TRUE(path == "c:\\asdfsd\\pr.com");

        path = "c:\\asdfsd\\pr.exe";
        Path::ChangeExtension(path, "com");
        VERIFY_IS_TRUE(path == "c:\\asdfsd\\pr.com");
    }

    BOOST_AUTO_TEST_CASE(GetDirectoryName)
    {
#if !defined(PLATFORM_UNIX)
        VERIFY_IS_TRUE(Path::GetDirectoryName("c:\\asdfs\\pr.exe") == "c:\\asdfs");
        VERIFY_IS_TRUE(Path::GetDirectoryName("\\\\asdfs\\share\\pr.exe") == "\\\\asdfs\\share");
#else
        VERIFY_IS_TRUE(Path::GetDirectoryName("/asdfs/pr.exe") == "/asdfs");
        VERIFY_IS_TRUE(Path::GetDirectoryName("//asdfs/share/pr.exe") == "//asdfs/share");
#endif
    }

    BOOST_AUTO_TEST_CASE(GetFileName)
    {
#if !defined(PLATFORM_UNIX)
        VERIFY_IS_TRUE(Path::GetFileName("c:\\asdfs\\pr.exe") == "pr.exe");
        VERIFY_IS_TRUE(Path::GetFileName("\\\\asdfs\\share\\pr.exe") == "pr.exe");

        VERIFY_IS_TRUE(Path::GetFileNameWithoutExtension("c:\\asdfs\\pr.exe") == "pr");
        VERIFY_IS_TRUE(Path::GetFileNameWithoutExtension("\\\\asdfs\\share\\pr.exe") == "pr");
#else
        VERIFY_IS_TRUE(Path::GetFileName("/asdfs/pr.exe") == "pr.exe");
        VERIFY_IS_TRUE(Path::GetFileName("//asdfs/share/pr.exe") == "pr.exe");

        VERIFY_IS_TRUE(Path::GetFileNameWithoutExtension("/asdfs/pr.exe") == "pr");
        VERIFY_IS_TRUE(Path::GetFileNameWithoutExtension("//asdfs/share/pr.exe") == "pr");
#endif
    }

    BOOST_AUTO_TEST_CASE(GetModuleFileName)
    {
        std::string path = Path::GetModuleLocation(NULL);

        char buffer[1024];
        VERIFY_IS_TRUE(::GetModuleFileName(NULL, buffer, ARRAYSIZE(buffer)) != 0);
        VERIFY_IS_TRUE(path == buffer);
    }

    BOOST_AUTO_TEST_CASE(GetFilePathInModuleLocation)
    {
        const std::string filename = "hello.txt";
        std::string path = Path::GetModuleLocation(NULL);

        char buffer[1024];
        VERIFY_IS_TRUE(::GetModuleFileName(NULL, buffer, ARRAYSIZE(buffer)) != 0);
        ::PathRemoveFileSpec(buffer);

        char bufferCombined[1024];
        ::PathCombine(bufferCombined, buffer, filename.c_str());

        std::string pathInModuleLocation = Path::GetFilePathInModuleLocation(NULL, filename);
        VERIFY_IS_TRUE(pathInModuleLocation == bufferCombined);
    }

    BOOST_AUTO_TEST_CASE(ConvertToNtPathTest)
    {
#if !defined(PLATFORM_UNIX)
        VERIFY_IS_TRUE(Path::ConvertToNtPath("D:\\path") == "\\\\?\\D:\\path");
        VERIFY_IS_TRUE(Path::ConvertToNtPath("\\\\server\\share") == "\\\\?\\UNC\\server\\share");
        VERIFY_IS_TRUE(Path::ConvertToNtPath("D:\\path\\\\abc") == "\\\\?\\D:\\path\\abc");
        VERIFY_IS_TRUE(Path::ConvertToNtPath("\\\\?\\D:\\path") == "\\\\?\\D:\\path");
        VERIFY_IS_TRUE(Path::ConvertToNtPath("path") == "path");
        VERIFY_IS_TRUE(Path::ConvertToNtPath("D:\\path\\..\\abc") == "D:\\path\\..\\abc");
#else
        VERIFY_IS_TRUE(Path::ConvertToNtPath("//server/share") == "//server/share");
        VERIFY_IS_TRUE(Path::ConvertToNtPath("/path/abc") == "/path/abc");
        VERIFY_IS_TRUE(Path::ConvertToNtPath("path/abc") == "path/abc");
        VERIFY_IS_TRUE(Path::ConvertToNtPath("path") == "path");
#endif
    }

    BOOST_AUTO_TEST_CASE(GetFullPathTest)
    {
        std::string workingDirectory = "test";
        std::string fullPath = Path::Combine(Directory::GetCurrentDirectory(), workingDirectory);
        VERIFY_IS_TRUE(Path::GetFullPath(workingDirectory) == fullPath);

#if !defined(PLATFORM_UNIX)
        VERIFY_IS_TRUE(Path::GetFullPath("c:\\test\\") == "c:\\test");
        VERIFY_IS_TRUE(Path::GetFullPath("c:\\test\\\\") == "c:\\test");
#else
        VERIFY_IS_TRUE(Path::GetFullPath("./test") == fullPath);
        VERIFY_IS_TRUE(Path::GetFullPath("test") == fullPath);
        VERIFY_IS_TRUE(Path::GetFullPath("/home/test") == "/home/test");
#endif
    }

// Tests for apis implemented only on Windows
#if !defined(PLATFORM_UNIX)
    BOOST_AUTO_TEST_CASE(GetPathRootTest)
    {
        VERIFY_IS_TRUE(Path::GetPathRoot("c:") == "c:");
        VERIFY_IS_TRUE(Path::GetPathRoot("d:\\") == "d:");
        VERIFY_IS_TRUE(Path::GetPathRoot("e:\\test") == "e:");
        VERIFY_IS_TRUE(Path::GetPathRoot("F:\\test\\") == "F:");
    }

    BOOST_AUTO_TEST_CASE(GetDriveLetterTest)
    {
        VERIFY_IS_TRUE(Path::GetDriveLetter("c:") == 'c');
        VERIFY_IS_TRUE(Path::GetDriveLetter("d:\\") == 'd');
        VERIFY_IS_TRUE(Path::GetDriveLetter("e:\\test") == 'e');
        VERIFY_IS_TRUE(Path::GetDriveLetter("F:\\test\\") == 'F');
        VERIFY_IS_TRUE(Path::GetDriveLetter("\\") == NULL);
        VERIFY_IS_TRUE(Path::GetDriveLetter("") == NULL);
    }
#endif

    BOOST_AUTO_TEST_SUITE_END()
} // end namespace Common

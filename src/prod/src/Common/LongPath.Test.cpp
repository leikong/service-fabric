// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

#include <boost/test/unit_test.hpp>
#include "Common/boost-taef.h"

using namespace std;
Common::StringLiteral const TraceType = "LongPathTest";
#define LONG_PATH_LENGTH 500

#if !defined(PLATFORM_UNIX)
#define PATH_SEPARATOR_C '\\'
#define PATH_SEPARATOR_S "\\"
#else
#define PATH_SEPARATOR_C '/'
#define PATH_SEPARATOR_S "/"
#endif

namespace Common
{
    using std::string;

    // Tests LongPath
    class TestLongPath
    {
    protected:
        void VerifyPath(const string & childPath, const string & root);
        static string TestLongPath::BuildLongPath(string & basePath, char charToFill = 'x', int dirSize = 100);
        string ConvertToRemotePath(const string & path);
        void VerifyGetFiles(const string & path, bool recursive, int exepectResultSize);
        void VerifyGetSubDirectories(const string & path, bool recursive, int expectResultSize);
    };

    BOOST_FIXTURE_TEST_SUITE(TestLongPathSuite, TestLongPath)

    BOOST_AUTO_TEST_CASE(VerifyRegularPaths)
    {
        string absolutePath = Path::Combine(Directory::GetCurrentDirectoryW(), "LongPath.test");
        VerifyPath(absolutePath, absolutePath);

        string relativePath = "LongPath.test";
        VerifyPath(relativePath, relativePath);

#if !defined(PLATFORM_UNIX)
// Note: Does not make much sense to verify creation of folder: /LongPath.test
        string relativePathWithALeadingSlash = PATH_SEPARATOR_S "LongPath.test";
        VerifyPath(relativePathWithALeadingSlash, relativePathWithALeadingSlash);
#endif
    }

    BOOST_AUTO_TEST_CASE(VerifyLocalLongPath)
    {
        string basePath;
        string longPath = TestLongPath::BuildLongPath(basePath);
        TestLongPath::VerifyPath(longPath, basePath);
    }

    BOOST_AUTO_TEST_CASE(VerifyRemoteLongPath)
    {
#if !defined(PLATFORM_UNIX)
// Note: Remote path is Windows only feature
        string basePath;
        string longPath = TestLongPath::BuildLongPath(basePath);
        TestLongPath::VerifyPath(TestLongPath::ConvertToRemotePath(longPath),
            TestLongPath::ConvertToRemotePath((basePath)));
#endif
    }

    BOOST_AUTO_TEST_SUITE_END()

    void TestLongPath::VerifyPath(const string & childPath, const string & root)
    {
        string longFilePath = Path::Combine(childPath, "abc.txt");
        string copyFilePath = Path::Combine(childPath, "abc_copy.txt");
        string moveFilePath = Path::Combine(childPath, "abc_moved.txt");

        VERIFY_IS_TRUE(Directory::Create2(childPath).IsSuccess());
        VERIFY_IS_TRUE(Directory::Exists(childPath));
        if (StringUtility::CompareCaseInsensitive(childPath, root) != 0)
        {
            VerifyGetSubDirectories(root, false, 1);
            VerifyGetSubDirectories(root, true, 4);
        }
        else
        {
            VerifyGetSubDirectories(root, false, 0);
            VerifyGetSubDirectories(root, true, 0);
        }

        std::unique_ptr<File> file = Common::make_unique<File>();
        VERIFY_IS_TRUE(file->TryOpen(longFilePath, FileMode::CreateNew, FileAccess::ReadWrite).IsSuccess());
        VERIFY_IS_TRUE(File::Exists(longFilePath));
        VERIFY_IS_TRUE(file->Close2().IsSuccess());

        if (StringUtility::CompareCaseInsensitive(childPath, root) != 0)
        {
            VerifyGetFiles(root, false, 0);
        }
        else
        {
            VerifyGetFiles(root, false, 1);
        }

        VerifyGetFiles(childPath, false, 1);
        VerifyGetFiles(root, true, 1);

        VERIFY_IS_TRUE(File::Copy(longFilePath, copyFilePath, true).IsSuccess());
        VERIFY_IS_TRUE(File::Exists(copyFilePath));
        VerifyGetFiles(childPath, false, 2);
        VerifyGetFiles(root, true, 2);

        File::Move(longFilePath, moveFilePath);
        VERIFY_IS_TRUE(File::Exists(moveFilePath));
        VERIFY_IS_TRUE(!File::Exists(longFilePath));
        VerifyGetFiles(childPath, false, 2);
        VerifyGetFiles(root, true, 2);

        VERIFY_IS_TRUE(File::Delete2(moveFilePath).IsSuccess());
        VERIFY_IS_TRUE(!File::Exists(moveFilePath));
        VerifyGetFiles(childPath, false, 1);
        VerifyGetFiles(root, true, 1);

        VERIFY_IS_TRUE(Directory::Delete(root, true).IsSuccess());
        VERIFY_IS_TRUE(!Directory::Exists(childPath));
        VerifyGetSubDirectories(root, false, 0);
        VerifyGetSubDirectories(root, true, 0);
    }

    void TestLongPath::VerifyGetFiles(const string & path, bool recursive, int exepectResultSize)
    {
        vector<string> result;
        if (recursive)
        {
            result = Directory::GetFiles(path, "*", true, false);
        }
        else
        {
            result = Directory::GetFiles(path);
        }

#if !defined(PLATFORM_UNIX)
// Note: file find not implemented on Linux
        VERIFY_ARE_EQUAL(exepectResultSize, result.size());
#endif
    }

    void TestLongPath::VerifyGetSubDirectories(const string & path, bool recursive, int exepectResultSize)
    {
        vector<string> result;
        if (recursive)
        {
            result = Directory::GetSubDirectories(path, "*", true, false);
        }
        else
        {
            result = Directory::GetSubDirectories(path);
        }

#if !defined(PLATFORM_UNIX)
// Note: file find not implemented on Linux
        VERIFY_ARE_EQUAL(exepectResultSize, result.size());
#endif
    }

    string TestLongPath::BuildLongPath(string & basePath, char charToFill, int dirSize)
    {
        string path(LONG_PATH_LENGTH + 1, charToFill);
        // each individual directory name is still limited to 255
        for (int i = dirSize; i < LONG_PATH_LENGTH; i = i + dirSize)
        {
            path[i] = PATH_SEPARATOR_C;
        }
        std::string::size_type index = path.find_first_of(PATH_SEPARATOR_S);
        basePath = Path::Combine(Directory::GetCurrentDirectoryW(), path.substr(0, index));
        return Path::Combine(Directory::GetCurrentDirectoryW(), path);
    }

    string TestLongPath::ConvertToRemotePath(const string & path)
    {
        string remotePath(path);
        // Converting to a remote path i.e. if the local path is d:\abc, 
        // corresponding remote path would be \\localhost\d$\abc
        remotePath.insert(0, "\\\\localhost\\");
        StringUtility::Replace<std::string>(remotePath, ":", "$");
        return remotePath;
    }
}

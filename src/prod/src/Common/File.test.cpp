// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include "Common/boost-taef.h"

namespace Common
{
    using namespace std;

    static string const fprefix = "FileTest-";

    class FileTests
    {
    };

    BOOST_FIXTURE_TEST_SUITE(FileTestsSuite, FileTests)

#if defined(PLATFORM_UNIX)
    BOOST_AUTO_TEST_CASE(CreateHardLink)
    {
            string fname = fprefix + Guid::NewGuid().ToString();
            File linkFrom;
            auto err = linkFrom.TryOpen(fname, FileMode::CreateNew);
            BOOST_REQUIRE_EQUAL(err.ReadValue(), ErrorCodeValue::Success);

            string linkName = fprefix + Guid::NewGuid().ToString();
            auto linked = File::CreateHardLink(linkName, fname);
            BOOST_REQUIRE(linked);

            auto linkedAgain = File::CreateHardLink(linkName, fname);
            BOOST_REQUIRE(!linkedAgain);

            BOOST_REQUIRE(File::Exists(linkName));

            BOOST_REQUIRE_EQUAL(File::Delete2(fname).ReadValue(), ErrorCodeValue::Success);
            BOOST_REQUIRE(!File::Exists(fname));
            BOOST_REQUIRE(File::Exists(linkName));

            BOOST_REQUIRE_EQUAL(File::Delete2(linkName).ReadValue(), ErrorCodeValue::Success);
            BOOST_REQUIRE(!File::Exists(linkName));
    }

    BOOST_AUTO_TEST_CASE(FileAccessDiagnostics)
    {
        auto dstr = File::GetFileAccessDiagnostics("/tmp");
        RealConsole c;
        c.WriteLine("file access diagnostistics: {0}", dstr);
        BOOST_REQUIRE(!dstr.empty());
    }

#endif // defined(PLATFORM_UNIX)

    BOOST_AUTO_TEST_CASE(ReplaceFileNoBackup)
    {
        string guid = Guid::NewGuid().ToString();
        string original = fprefix + "-original-" + guid;
        {
            File originalFile;
            auto err = originalFile.TryOpen(original, FileMode::CreateNew);
            BOOST_REQUIRE_EQUAL(err.ReadValue(), ErrorCodeValue::Success);
            BOOST_REQUIRE_EQUAL(originalFile.size(), (int64)0);
        }

        string replacement = fprefix + "-replacement-" + guid;
        char c = 'a';
        size_t dataSize = sizeof(c);
        {
            File replacementFile;
            auto err = replacementFile.TryOpen(replacement, FileMode::CreateNew, FileAccess::Write);
            BOOST_REQUIRE_EQUAL(err.ReadValue(), ErrorCodeValue::Success);

            replacementFile.Write(&c, (int)dataSize);
            BOOST_REQUIRE_EQUAL(replacementFile.size(), (int64)dataSize);
        }

        auto err = File::Replace(original, replacement, "", true);
        BOOST_REQUIRE_EQUAL(err.ReadValue(), ErrorCodeValue::Success);

        BOOST_REQUIRE(!File::Exists(replacement));

        int64 newSize = 0;
        err = File::GetSize(original, newSize);
        BOOST_REQUIRE_EQUAL(err.ReadValue(), ErrorCodeValue::Success);
        BOOST_REQUIRE_EQUAL(newSize, (int64)dataSize);

        BOOST_REQUIRE_EQUAL(File::Delete2(original).ReadValue(), ErrorCodeValue::Success);
    }

    BOOST_AUTO_TEST_CASE(ReplaceFileWithBackup)
    {
        string guid = Guid::NewGuid().ToString();
        string original = fprefix + "-original-" + guid;
        int64 originalFileSize = 0;
        {
            File originalFile;
            auto err = originalFile.TryOpen(original, FileMode::CreateNew);
            BOOST_REQUIRE_EQUAL(err.ReadValue(), ErrorCodeValue::Success);
            BOOST_REQUIRE_EQUAL(originalFile.size(), originalFileSize);
        }

        string replacement = fprefix + "-replacement-" + guid;
        char c = 'a';
        size_t dataSize = sizeof(c);
        {
            File replacementFile;
            auto err = replacementFile.TryOpen(replacement, FileMode::CreateNew, FileAccess::Write);
            BOOST_REQUIRE_EQUAL(err.ReadValue(), ErrorCodeValue::Success);

            replacementFile.Write(&c, (int)dataSize);
            BOOST_REQUIRE_EQUAL(replacementFile.size(), (int64)dataSize);
        }

        string backup = fprefix + "-backup-" + guid;
        auto err = File::Replace(original, replacement, backup, true);
        BOOST_REQUIRE_EQUAL(err.ReadValue(), ErrorCodeValue::Success);

        BOOST_REQUIRE(!File::Exists(replacement));

        int64 newSize = 0;
        err = File::GetSize(original, newSize);
        BOOST_REQUIRE_EQUAL(err.ReadValue(), ErrorCodeValue::Success);
        BOOST_REQUIRE_EQUAL(newSize, (int64)dataSize);

        BOOST_REQUIRE(File::Exists(backup));
        int64 backupFileSize = -1;
        err = File::GetSize(backup, backupFileSize);
        BOOST_REQUIRE_EQUAL(backupFileSize, originalFileSize);

        BOOST_REQUIRE_EQUAL(File::Delete2(original).ReadValue(), ErrorCodeValue::Success);
        BOOST_REQUIRE_EQUAL(File::Delete2(backup).ReadValue(), ErrorCodeValue::Success);
    }

#if !defined(PLATFORM_UNIX)
    BOOST_AUTO_TEST_CASE(FlushFileVolume)
    {
        // Find valid system drive
        char drive = NULL;
        string rootPath("");
        for (char testDrive = 'a'; testDrive <= 'z'; testDrive++)
        {
            rootPath = testDrive;
            rootPath.append(":\\");
            if (Directory::Exists(rootPath))
            {
                drive = testDrive;
                break;
            }
        }
        VERIFY_IS_TRUE(drive != NULL);

        // Flush
        ErrorCode error(ErrorCodeValue::Success);
        error = File::FlushVolume(drive);
        VERIFY_IS_TRUE(error.IsSuccess(), formatString.L("FlushVolume failed against {0} drive with error {1}", drive, error).c_str());

        error = File::FlushVolume(NULL);
        VERIFY_IS_TRUE(error.IsError(ErrorCodeValue::ArgumentNull), formatString.L("FlushVolume did not receive correct errorcode {0} for null drive.", drive, error).c_str());

        error = File::FlushVolume('.');
        VERIFY_IS_TRUE(error.IsWin32Error(ERROR_FILE_NOT_FOUND), formatString.L("FlushVolume did not receive correct errorcode {0} for incorrect volume.", drive, error).c_str());
    }
#endif

    BOOST_AUTO_TEST_SUITE_END()
} // end namespace Common

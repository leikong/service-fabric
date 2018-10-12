// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"
#include <sys/stat.h>

#include <boost/test/unit_test.hpp>
#include "Common/boost-taef.h"

using namespace std;
using namespace Common;

#define CertDirMonitorTraceType "CertDirMonitorTraceType"

#define FAIL_TEST( fmt, ...) \
    { \
        string tmp; \
        StringWriter writer(tmp); \
        writer.Write(fmt, __VA_ARGS__); \
        Trace.WriteError(CertDirMonitorTraceType, "{0}", tmp); \
        VERIFY_IS_TRUE( false, tmp.c_str() ); \
    } \

class CertDirMonitorTest
{
protected:
    bool SetupTestCase();
    bool CleanupTestCase();
    void WriteToFile(string const & filepath, string const & filetext);

    string testCaseFolder_;
    string testSrcPath_;
    string testDestPath_;

    static string TestDirectoryName;
    static string TestSrcDirectoryName;
    static string TestDestDirectoryName;
    static string TestFileContents;
};

string CertDirMonitorTest::TestDirectoryName = "CertDirMonitorTest.Data";
string CertDirMonitorTest::TestSrcDirectoryName = "waagent";
string CertDirMonitorTest::TestDestDirectoryName = "waagent_mirror";

BOOST_FIXTURE_TEST_SUITE(CertDirMonitorSuite, CertDirMonitorTest)

BOOST_AUTO_TEST_CASE(CertMonitorTest1)
{
    ENTER;
    BOOST_REQUIRE(SetupTestCase());
    KFinally([this] { BOOST_REQUIRE(CleanupTestCase()); });

    CertDirMonitor &monitor = CertDirMonitor::GetSingleton();

    int i = 0;

    // prepare pre-existing files
    string preExistsFileNameBase = "preExists";
    string preExistsFileContent = "preExists Cert Content";
    for (i = 0; i < CertDirMonitor::NrCertFileExts; i++)
    {
        string preFilePath = Path::Combine(testSrcPath_, preExistsFileNameBase + "." + CertDirMonitor::CertFileExtsW[i]);
        WriteToFile(preFilePath, preExistsFileContent);
    }

    // start monitoring
    ErrorCode error = monitor.StartWatch(Utf16ToUtf8NotNeeded(testSrcPath_), Utf16ToUtf8NotNeeded(testDestPath_));
    if (!error.IsSuccess())
    {
        FAIL_TEST("CertDirMonitor StartWatch Failed with error {0}. Directories: '{1}' to '{}'", error, testSrcPath_, testDestPath_);
    }

    // verify the copying of pre-existing files
    for (i = 0; i < CertDirMonitor::NrCertFileExts; i++)
    {
        string preFilePath = Path::Combine(testDestPath_, preExistsFileNameBase + "." + CertDirMonitor::CertFileExtsW[i]);
        if (!File::Exists(preFilePath))
        {
            FAIL_TEST("verification failed: CertDirMonitor copying of pre-existing file {0}", preFilePath);
        }
    }

    // put in some new files
    string newFileNameBase = "onWatch";
    string newFileContent = "onWatch Cert Content";

    for (i = 0; i < CertDirMonitor::NrCertFileExts; i++)
    {
        string newFilePath = Path::Combine(testSrcPath_, newFileNameBase + "." + CertDirMonitor::CertFileExtsW[i]);
        WriteToFile(newFilePath, newFileContent);
    }

    // add some noise on non-cert files, sub folders
    string noiseFileNameBase = "noise";
    string noiseFileContent = "noise Cert Content";
    string noiseDirPath = Path::Combine(testSrcPath_, Guid::NewGuid().ToString());
    string noiseFilePath = Path::Combine(noiseDirPath, noiseFileNameBase + "." + CertDirMonitor::CertFileExtsW[0]);
    Directory::Create(noiseDirPath);
    WriteToFile(noiseFilePath, noiseFileContent);

    string noiseDirectFilePath = Path::Combine(testSrcPath_, Guid::NewGuid().ToString() + ".noise");
    WriteToFile(noiseDirectFilePath, noiseFileContent);

    // give it some time
    Sleep(1000);

    // verify
    for (i = 0; i < CertDirMonitor::NrCertFileExts; i++)
    {
        string newFilePath = Path::Combine(testDestPath_, newFileNameBase + "." + CertDirMonitor::CertFileExtsW[i]);
        if (!File::Exists(newFilePath))
        {
            FAIL_TEST("verification failed: CertDirMonitor syncing of new file {0}", newFilePath);
        }
        struct stat newFileStat;
        string newFilePathA = Utf16ToUtf8NotNeeded(newFilePath);
        if (stat(newFilePathA.c_str(), &newFileStat) || (newFileStat.st_mode & (S_IWOTH))) {
            FAIL_TEST("verification failed: CertDirMonitor syncing of new file with mode check of {0}", newFilePath);
        }
    }

    // delete new files
    for (i = 0; i < CertDirMonitor::NrCertFileExts; i++)
    {
        string newFilePath = Path::Combine(testSrcPath_, newFileNameBase + "." + CertDirMonitor::CertFileExtsW[i]);
        File::Delete(newFilePath);
    }

    Sleep(1000);

    // verify
    for (i = 0; i < CertDirMonitor::NrCertFileExts; i++)
    {
        string newFilePath = Path::Combine(testDestPath_, newFileNameBase + "." + CertDirMonitor::CertFileExtsW[i]);
        if (File::Exists(newFilePath))
        {
            FAIL_TEST("verification failed: CertDirMonitor syncing of deleted file {0}", newFilePath);
        }
    }

    LEAVE;
}

BOOST_AUTO_TEST_SUITE_END()

bool CertDirMonitorTest::SetupTestCase()
{
    testCaseFolder_ = Path::Combine(TestDirectoryName, Guid::NewGuid().ToString());
    Trace.WriteInfo(CertDirMonitorTraceType, "creating test folder {0}", testCaseFolder_);
    auto error = Directory::Create2(testCaseFolder_);
    if (!error.IsSuccess())
    {
        FAIL_TEST("Failed to create test directory {0}: {1}", testCaseFolder_, error);
    }

    testSrcPath_ = Path::Combine(testCaseFolder_, TestSrcDirectoryName);
    error = Directory::Create2(testSrcPath_);
    if (!error.IsSuccess())
    {
        FAIL_TEST("Failed to create test src directory {0}: {1}", testSrcPath_, error);
    }

    testDestPath_ = Path::Combine(testCaseFolder_, TestDestDirectoryName);
    return true;
}

bool CertDirMonitorTest::CleanupTestCase()
{
    return true;
}

void CertDirMonitorTest::WriteToFile(string const & filePath, string const & fileText)
{
    string directoryName = Path::GetDirectoryName(filePath);
    string tempFileName = Guid::NewGuid().ToString();
    string tempFilePath = Path::Combine(directoryName, tempFileName);

    File file;
    auto error = file.TryOpen(tempFilePath, FileMode::Create, FileAccess::Write, FileShare::ReadWrite);
    if (!error.IsSuccess())
    {
        File::Delete(tempFilePath, Common::NOTHROW());
        FAIL_TEST("Failed to open '{0}' due to {1}", filePath, error);
    }


    int size = static_cast<int>(fileText.length() * sizeof(char));
    int written = file.TryWrite((void*)fileText.c_str(), size);
    if (!error.IsSuccess())
    {
        File::Delete(tempFilePath, Common::NOTHROW());
        FAIL_TEST("Failed write file '{0}': size = {1} written = {2}", filePath, size, written);
    }

    file.Flush();
    file.Close();

    File::Copy(tempFilePath, filePath, true);
    File::Delete(tempFilePath, Common::NOTHROW());
}

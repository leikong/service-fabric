// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    struct ProcessUtility : TextTraceComponent<Common::TraceTaskCodes::Common>
    {
    public:
        static Common::ErrorCode OpenProcess(
            DWORD dwDesiredAccess,
            BOOL bInheritHandle,
            DWORD dwProcessId,
            __out Common::HandleUPtr & processHandle);

        static Common::ErrorCode CreateProcess(
            std::string const & commandLine,
            std::string const & workingDirectory,
            std::vector<char> & environmentBlock,
            DWORD dwCreationFlags,
            __out Common::HandleUPtr & processHandle,
            __out Common::HandleUPtr & threadHandle,
            pid_t & processId);

        static Common::ErrorCode CreateProcess(
            std::string const & commandLine,
            std::string const & workingDirectory,
            std::vector<char> & environmentBlock,
            DWORD dwCreationFlags,
            __out Common::HandleUPtr & processHandle,
            __out Common::HandleUPtr & threadHandle);

        static pid_t GetProcessId();

#if defined(PLATFORM_UNIX)
        static Common::ErrorCode GetStatusOutput(std::string const & cmdline, std::string & output);
        static Common::ErrorCode GetStatusOutput(std::string const & cmdline, std::vector<std::string> & outputLines);
#else
        static Common::ErrorCode GetProcessImageName(DWORD processId, _Out_ std::string & imageName);

        static Common::ErrorCode ExecuteCommandLine(std::string const & commandLine, DWORD timeoutInMilliseconds = INFINITE);

        static Common::ErrorCode CreateAnnonymousJob(
            bool allowBreakaway,
            __out Common::HandleUPtr & jobHandle);
       
        static Common::ErrorCode AssociateProcessToJob(
            Common::HandleUPtr const & jobHandle,
            Common::HandleUPtr const & processHandle);

        static Common::ErrorCode ResumeProcess(
            Common::HandleUPtr const & processHandle,
            Common::HandleUPtr const & threadHandle);
#endif

        // performs a hard kill on the process, this is used as a last 
        // resort to clean up the process 
        static void TerminateProcess(
            Common::HandleUPtr const & processHandle);
        
        static DWORD CreationFlags_SuspendedProcessWithJobBreakaway;
        static DWORD CreationFlags_SuspendedProcessWithJobBreakawayNoWindow;

        static Common::ErrorCode ParseCommandLine(std::string const & commandLine, std::string & exePath, std::string & arguments);
        static std::string GetCommandLine(std::string const & exePath, std::string const & arguments);

#if !defined(PLATFORM_UNIX)
        static Common::ErrorCode GetProcessExitCode(
            Common::Handle const & processHandle,
            __out DWORD & exitCode);

        // enables synchronize access on the current process to all the users
        static Common::ErrorCode EnableSynchronizeAccess();

        // enables synchronize access on the specified process to the specified well known user
        static Common::ErrorCode EnableSynchronizeAccess(ProcessHandle const & processHandle, WELL_KNOWN_SID_TYPE wellKnownSidType);

        static Common::ErrorCode GetProcessSidString(__out std::string & stringSid);

        static Common::ErrorCode GetProcessSidString(ProcessHandle const & processHandle, __out std::string & stringSid);
#endif

        // creates default environment block that can be used by FabricNode to fork child processes
        // the environment block contains information about the default configuration store loaded by FabricNode
        static Common::ErrorCode CreateDefaultEnvironmentBlock(__out std::vector<char> & envBlock);

    private:

         static void TerminateProcess(HANDLE processHandle);

         static void CleanupProcess(
            Common::HandleUPtr const & processHandle,
            LPPROCESS_INFORMATION lpProcessInformation);

         static void CleanupCreateProcessHandles(
            Common::HandleUPtr & processHandle,
            Common::HandleUPtr & threadHandle,
            LPPROCESS_INFORMATION lpProcessInformation);
    };
}

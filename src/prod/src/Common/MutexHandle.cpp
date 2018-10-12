// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace Common;
using namespace std;

static const StringLiteral TraceType("MutexHandle");

MutexHandleSPtr MutexHandle::CreateSPtr(string const & name)
{
    return make_shared<MutexHandle>(name);
}

MutexHandleUPtr MutexHandle::CreateUPtr(string const & name)
{
    return make_unique<MutexHandle>(name);
}

#ifdef PLATFORM_UNIX

MutexHandle::MutexHandle(string const & name)
    : id_(formatString.L("{0}", TextTraceThis))
    , handle_(GetMutexPath(name))
{
    WriteInfo(TraceType, id_, "name='{0}', path='{1}'", name, GetMutexPath(name));
}

string MutexHandle::NormalizeName(std::string const & name)
{
    string normalizedName;
    normalizedName.reserve(name.size());

    for(auto ch : name)
    {
        // Linux file path actually allows all characters except null terminator,
        // the following converts non-standard ones for portability to other unix systems
        if (isdigit(ch) || isalpha(ch) || (ch == '.') || (ch == '-') || (ch == '_') || (ch == '/'))
        {
            normalizedName.push_back(ch);
            continue;
        }

        if (ch == '\\')
        {
            normalizedName.push_back('/');
            continue;
        }

        const FormatOptions formatString(sizeof(char)*2, true, "x");
        StringWriter(normalizedName).WriteNumber(ch, formatString, false); 
    }

    Invariant(normalizedName.size() <= NAME_MAX);
    return normalizedName;
}

string MutexHandle::GetMutexPath(string const & name)
{
    ASSERT_IF(name.empty(), "empty name not supported, use RwLock instead");

    auto pathw = Environment::GetObjectFolder();
    Path::CombineInPlace(pathw, "mutex");
    return Path::Combine(pathw, NormalizeName(name));
}

MutexHandle::~MutexHandle()
{
    Release();
}

ErrorCode MutexHandle::WaitOne(TimeSpan timeout)
{
    return handle_.Acquire(timeout);
}

void MutexHandle::Release()
{
    handle_.Release();
}

#else

MutexHandle::MutexHandle(string const & name)
    : handle_(nullptr), id_(formatString.L("{0}", TextTraceThis))
{
    ASSERT_IF(name.empty(), "empty name not supported, use Rwlock instead");

    handle_ = CreateMutex(NULL, FALSE, name.c_str());
    if (handle_ == NULL)
    {
        createStatus_ = ErrorCode::FromWin32Error();
        WriteWarning(
            TraceType,
            id_,
            "CreateMutex({0}) failed: ErrorCode={1}",
            name,
            createStatus_);
    }
}

MutexHandle::~MutexHandle()
{
    Release();

    auto success = ::CloseHandle(handle_);
    handle_ = NULL; 
    if (success == 0)
    {
        WriteWarning(
            TraceType,
            id_,
            "CloseHandle failed: ErrorCode={0}",
            ErrorCode::FromWin32Error());
    }
}

void MutexHandle::Release()
{
    ::ReleaseMutex(handle_);
    WriteNoise(TraceType, id_, "released");
}

ErrorCode MutexHandle::WaitOne(TimeSpan timeout)
{
    if (!createStatus_.IsSuccess()) return createStatus_;

    DWORD timeoutValue = (timeout == TimeSpan::MaxValue)? INFINITE : (DWORD)timeout.TotalPositiveMilliseconds();
    DWORD result = WaitForSingleObject(handle_, timeoutValue);
    if (result == WAIT_OBJECT_0)
    {
        WriteNoise(TraceType, id_, "acquired");
        return ErrorCodeValue::Success;
    }

    if (result == WAIT_TIMEOUT)
    {
        return ErrorCodeValue::Timeout;
    }

    return ErrorCode::FromWin32Error(result);
}

#endif


// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace std;
using namespace Common;

// Should always be written and read through FabricGlobals
//
// Cleanup happens when the ErrorCode object that set the 
// message destructs.
//
ThreadErrorMessages::ThreadErrorMessages()
    : tidMessages_()
    , lock_()
{
}

DWORD ThreadErrorMessages::SetMessage(string const & msg)
{
    auto tid = ::GetCurrentThreadId();

    AcquireWriteLock lock(lock_);

    tidMessages_[tid] = msg;

    return tid;
}

string ThreadErrorMessages::GetMessage()
{
    auto tid = ::GetCurrentThreadId();

    AcquireReadLock lock(lock_);

    auto it = tidMessages_.find(tid);
    if (it != tidMessages_.end())
    {
        return it->second;
    }

    return "";
}

void ThreadErrorMessages::ClearMessage(DWORD tid)
{
    AcquireWriteLock lock(lock_);

    tidMessages_.erase(tid);
}

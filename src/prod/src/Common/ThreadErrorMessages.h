// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class ThreadErrorMessages
    {
    public:
        ThreadErrorMessages();

        DWORD SetMessage(std::string const &);
        std::string GetMessage();
        void ClearMessage(DWORD tid);

    private:
        std::map<DWORD, std::string> tidMessages_;
        RwLock lock_;
    };
}

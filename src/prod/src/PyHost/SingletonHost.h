// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace PyHost
{
    class SingletonHost
    {
    public:
        static Host & GetInstance()
        {
            static Host instance;
            return instance;
        }

    private:
        SingletonHost(SingletonHost const &) = delete;
        void operator=(SingletonHost const &) = delete;
    };
}

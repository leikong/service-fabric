// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include <Python.h>

#pragma once

namespace PyHost
{
    class ScopedGilState
    {
    public:
        ScopedGilState() : state_(PyGILState_Ensure())
        {
        }

        ~ScopedGilState()
        {
            PyGILState_Release(state_);
        }

    private:
        PyGILState_STATE state_;
    };
}

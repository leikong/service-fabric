// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

#include "internal/pal_common.h"

/*
src/prod/src/Common/lib/../../Common/Parse.h:46:45: error: too few arguments provided to function-like macro invocation
        if (std::numeric_limits<int32>::min() <= result && result <= std::numeric_limits<int32>::max()) {

#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
*/

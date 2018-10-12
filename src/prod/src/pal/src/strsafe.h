// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

#include "internal/pal_common.h"

#include "internal/rt/strsafe.h"

#ifdef __cplusplus
extern "C" {
#endif

// 153
typedef _Null_terminated_ char* STRSAFE_LPSTR;
typedef _Null_terminated_ const char* STRSAFE_LPCSTR;
typedef _Null_terminated_ char* STRSAFE_LPSTR;
typedef _Null_terminated_ const char* STRSAFE_LPCSTR;

// 165
typedef  const char* STRSAFE_PCNZCH;
typedef  const char* STRSAFE_PCNZWCH;

#ifdef __cplusplus
}
#endif

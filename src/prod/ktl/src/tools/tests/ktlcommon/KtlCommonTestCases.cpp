    /*++

Copyright (c) Microsoft Corporation

Module Name:

    KtlCommonTestCases.cpp

Abstract:

    This file contains the test case table.

    To add a new unit test case:
    1. Declare your test case routine in KtlCommonTests.h.
    2. Add an entry to the gs_KuTestCases table in KtlCommonTestCases.cpp.
    3. Implement your test case routine. The implementation can be in
    this file or any other file.

    --*/
#pragma once
#include "KtlCommonTests.h"

//
// A table containing information of all test cases.
//

const KU_TEST_ENTRY gs_KuTestCases[] =
{
    {   "KtlCommonTest",            KtlCommonTest,         "BVT", "A few common KTL tests"},
#if !defined(PLATFORM_UNIX)
    {   "KtlTextTest",              KtlTextTest,           "BVT", "KTextFile object tests\n\t Optional: -Path [Full path name]"},
    {   "KNtMemoryMapTest",         KNtMemoryMapTest,      "BVT", "Tests for memory mapped file relevant functions in KNt"}
#endif
};

const ULONG gs_KuTestCasesSize = ARRAYSIZE(gs_KuTestCases);

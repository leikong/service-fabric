    /*++

Copyright (c) Microsoft Corporation

Module Name:

    KTaskTestCases.cpp

Abstract:

    This file contains the test case table.

    To add a new unit test case:
    1. Declare your test case routine in KTaskTests.h.
    2. Add an entry to the gs_KuTestCases table in KTaskTestCases.cpp.
    3. Implement your test case routine. The implementation can be in
    this file or any other file.

    --*/
#pragma once
#include "KTaskTests.h"

//
// A table containing information of all test cases.
//

const KU_TEST_ENTRY gs_KuTestCases[] =
{
    {   "KTaskTest",              KTaskTest,         "BVT", "Template for first test"}
};

const ULONG gs_KuTestCasesSize = ARRAYSIZE(gs_KuTestCases);

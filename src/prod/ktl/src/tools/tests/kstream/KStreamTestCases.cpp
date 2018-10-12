#pragma once
#include "KStreamTests.h"

//
// A table containing information of all test cases.
//
const KU_TEST_ENTRY gs_KuTestCases[] =
{
    {   "KStreamTest",            KStreamTest,         "BVT", "KStream implementation tests"},
};

const ULONG gs_KuTestCasesSize = ARRAYSIZE(gs_KuTestCases);

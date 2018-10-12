// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"
#include "Common/StackTrace.h"


#include <boost/test/unit_test.hpp>
#include "Common/boost-taef.h"

using namespace std;

namespace Common
{
    BOOST_AUTO_TEST_SUITE2(StackTraceTests)

    BOOST_AUTO_TEST_CASE(DirectStackTraceTest)
    {
        StackTrace stackTrace;
        stackTrace.CaptureCurrentPosition();
        string result = stackTrace.ToString();
        Trace.WriteInfo(TraceType, "{0}", result);
        for(int i = 0; i < result.length(); i++)
        {
            cout << (char) result[i];
        }
        cout << "\n";

        BOOST_REQUIRE(result.find("DirectStackTraceTest") != string::npos);
    }

    static CallStackRanking callerRanking;

    void __attribute__ ((noinline)) TestCallee()
    {
        callerRanking.Track();
    }

    void __attribute__ ((noinline)) Caller0()
    {
        TestCallee();
    }

    void __attribute__ ((noinline)) Caller1()
    {
        TestCallee();
    }

    void __attribute__ ((noinline)) Caller2()
    {
        TestCallee();
    }

    void __attribute__ ((noinline)) Caller3()
    {
        TestCallee();
    }

    BOOST_AUTO_TEST_CASE(CallerRanking)
    {
        Caller1();

        for(int i = 0; i < 2; ++i)
        {
            Caller2();
        }

        for(int i = 0; i < 3; ++i)
        {
            Caller3();
        }

        callerRanking.ResolveSymbols();

        Trace.WriteInfo(TraceType, "{0}", callerRanking);
        string ranking = formatString.L("{0}", callerRanking);
        string rankingA;
        Utf16ToUtf8NotNeeded2(ranking, rankingA);
        cout << rankingA << endl;

        auto results = callerRanking.Callstacks();
        int count = 1;
        for(auto const & r : results)
        {
            auto callstack = formatString.L("{0}", r.first);
            cout << "callstack: " << callstack << endl;
            auto expectedFunc = formatString.L("Caller{0}", r.second);
            cout << "expectedFunc: " << expectedFunc << endl;
            VERIFY_IS_TRUE(StringUtility::Contains(callstack, expectedFunc));
            ++count;
        }
    }

#endif

    BOOST_AUTO_TEST_SUITE_END()
}

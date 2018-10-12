// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

#include <boost/test/unit_test.hpp>
#include "Common/boost-taef.h"
#include "Common/StringUtility.h"

using namespace Common;
using namespace std;

BOOST_AUTO_TEST_SUITE2(StringUtilityTests)


BOOST_AUTO_TEST_CASE(TrimExtraCharacters)
{
    string trimmedString1 = StringUtility::LimitNumberOfCharacters(string("2+3-4+5-6+7"), '+', 2);
    VERIFY_IS_TRUE(trimmedString1 == string("2+3-4+5-6"));

    string trimmedString2 = StringUtility::LimitNumberOfCharacters(string("aabb"), 'b', 0);
    VERIFY_IS_TRUE(trimmedString2 == string("aa"));

    string trimmedString3 = StringUtility::LimitNumberOfCharacters(string("babb"), 'b', 0);
    VERIFY_IS_TRUE(trimmedString3 == string(""));

    string trimmedString4 = StringUtility::LimitNumberOfCharacters(string("bacb"), 'c', 1);
    VERIFY_IS_TRUE(trimmedString4 == string("bacb"));
}

BOOST_AUTO_TEST_CASE(RemoveDotAndGetDouble)
{
    string trimmedString1 = StringUtility::LimitNumberOfCharacters(string("17.03.1-ee-3"), '.', 1);
    double trimmedDouble1 = 0.0;
    VERIFY_IS_TRUE(trimmedString1 == string("17.03"));
    VERIFY_IS_TRUE(StringUtility::TryFromWString<double>(trimmedString1, trimmedDouble1));
    VERIFY_IS_TRUE(trimmedDouble1 == 17.03);

    string trimmedString2 = StringUtility::LimitNumberOfCharacters(string("17.06.2-ce-4"), '.', 1);
    double trimmedDouble2 = 0.0;
    VERIFY_IS_TRUE(trimmedString2 == string("17.06"));
    VERIFY_IS_TRUE(StringUtility::TryFromWString<double>(trimmedString2, trimmedDouble2));
    VERIFY_IS_TRUE(trimmedDouble2 == 17.06);
}

BOOST_AUTO_TEST_CASE(SmokeAreEqualCaseInsensitive)
{
    VERIFY_IS_TRUE(StringUtility::AreEqualCaseInsensitive("Test UPPER/lower", "TEST upper/LOWER"));
    VERIFY_IS_FALSE(StringUtility::AreEqualCaseInsensitive("aaa", " Aaa"));
    VERIFY_IS_FALSE(StringUtility::AreEqualCaseInsensitive("aaa", "aAb"));
}

BOOST_AUTO_TEST_CASE(SmokeAreEqualPrefixPartCaseInsensitive)
{
    VERIFY_IS_TRUE(StringUtility::AreEqualPrefixPartCaseInsensitive(string("servicefabric:/test"), string("ServiceFabric:/test"), ':'));
    VERIFY_IS_TRUE(StringUtility::AreEqualPrefixPartCaseInsensitive(string("servicefabric:/test"), string("ServiceFabric:/test"), ':'));
    VERIFY_IS_TRUE(StringUtility::AreEqualPrefixPartCaseInsensitive(string("serviceFabric:/test"), string("ServiceFabric:/test"), ':'));
    VERIFY_IS_TRUE(StringUtility::AreEqualPrefixPartCaseInsensitive(string("Random1_Run"), string("random1_Run"), '_'));
    VERIFY_IS_TRUE(StringUtility::AreEqualPrefixPartCaseInsensitive(string("Random1:Run"), string("random1:Run"), '_'));
    VERIFY_IS_FALSE(StringUtility::AreEqualPrefixPartCaseInsensitive(string("random1"), string("random2"), ','));
    VERIFY_IS_FALSE(StringUtility::AreEqualPrefixPartCaseInsensitive(string("random1:x"), string("random1:y"), ':'));
    VERIFY_IS_FALSE(StringUtility::AreEqualPrefixPartCaseInsensitive(string("random1:"), string("random1:y"), ':'));
    VERIFY_IS_FALSE(StringUtility::AreEqualPrefixPartCaseInsensitive(string("random1:Run"), string("random1:run"), ':'));
    VERIFY_IS_FALSE(StringUtility::AreEqualPrefixPartCaseInsensitive(string("Random1:Run"), string("random1:run"), ':'));
}

BOOST_AUTO_TEST_CASE(SmokeIsLessCaseInsensitive)
{
    VERIFY_IS_TRUE(StringUtility::IsLessCaseInsensitive("zzza", "zzzb"));
    VERIFY_IS_FALSE(StringUtility::IsLessCaseInsensitive("denial", "deniAl"));
    VERIFY_IS_TRUE(StringUtility::IsLessCaseInsensitive("mArket", "mbA"));
    VERIFY_IS_FALSE(StringUtility::IsLessCaseInsensitive("cast", "Bast"));
    VERIFY_IS_TRUE(StringUtility::IsLessCaseInsensitive("b", "C"));
    VERIFY_IS_TRUE(StringUtility::IsLessCaseInsensitive("alt", "Blt"));
}

BOOST_AUTO_TEST_CASE(SmokeContains1)
{
    string big = "Section one-two";
    VERIFY_IS_TRUE(StringUtility::Contains(big, string("one")));
    VERIFY_IS_TRUE(StringUtility::ContainsCaseInsensitive(big, string("OnE")));
    VERIFY_IS_FALSE(StringUtility::Contains(big, string("OnE")));
}

BOOST_AUTO_TEST_CASE(SmokeContains2)
{
    string big = "Section one-two";
    VERIFY_IS_TRUE(StringUtility::Contains(big, string("one")));
    VERIFY_IS_TRUE(StringUtility::ContainsCaseInsensitive(big, string("OnE")));
    VERIFY_IS_FALSE(StringUtility::Contains(big, string("OnE")));
}

BOOST_AUTO_TEST_CASE(SmokeSplit1)
{
    string input = " a,,  , string  with,    spaces and ,";
    vector<string> tokens;
    StringUtility::Split<string>(input, tokens, " ");

    vector<string> vect;
    vect.push_back("a,,");
    vect.push_back(",");
    vect.push_back("string");
    vect.push_back("with,");
    vect.push_back("spaces");
    vect.push_back("and");
    vect.push_back(",");
            
    VERIFY_IS_TRUE(tokens == vect);
}

BOOST_AUTO_TEST_CASE(SmokeSplit2)
{
    string input = " a,,  , string  with,    spaces and ,";
    vector<string> tokens;
    StringUtility::Split<string>(input, tokens, ",");
            
    vector<string> vect;
    vect.push_back(" a");
    vect.push_back("  ");
    vect.push_back(" string  with");
    vect.push_back("    spaces and ");
            
    VERIFY_IS_TRUE(tokens == vect);
}

BOOST_AUTO_TEST_CASE(SmokeSplit3)
{
    string input = " a,,  , string  with,    spaces and ,";
    vector<string> tokens;
    StringUtility::Split<string>(input, tokens, ", ");

    vector<string> vect;
    vect.push_back("a");
    vect.push_back("string");
    vect.push_back("with");
    vect.push_back("spaces");
    vect.push_back("and");
            
    VERIFY_IS_TRUE(tokens == vect);
}

BOOST_AUTO_TEST_CASE(SmokeSplit4)
{
    string input = " a string without any separators";
    vector<string> tokens;
    StringUtility::Split<string>(input, tokens, ";");
            
    vector<string> vect;
    vect.push_back(" a string without any separators");
            
    VERIFY_IS_TRUE(tokens == vect);
}

BOOST_AUTO_TEST_CASE(SmokeSplit5)
{
    string input = " a string without any separators";
    string token1;
    string token2;
    StringUtility::SplitOnce<string,char>(input, token1, token2, ';');
            
    VERIFY_IS_TRUE(token1 == input);
    VERIFY_IS_TRUE(token2 == "");
}


BOOST_AUTO_TEST_CASE(SmokeSplit6)
{
    string input = " a string without any separators";
    string token1;
    string token2;
    StringUtility::SplitOnce<string,char>(input, token1, token2, ':');
            
    VERIFY_IS_TRUE(token1 == input);
    VERIFY_IS_TRUE(token2 == "");
}

BOOST_AUTO_TEST_CASE(SmokeSplit7)
{
    string input = "HeaderName: HeaderValue";
    string token1;
    string token2;
    StringUtility::SplitOnce<string,char>(input, token1, token2, ':');
            
    VERIFY_IS_TRUE(token1 == "HeaderName");
    VERIFY_IS_TRUE(token2 == " HeaderValue");
}


BOOST_AUTO_TEST_CASE(SmokeSplit8)
{
    string input = "SOAPAction: \"http://tempuri.org/samples\"";
    string token1;
    string token2;
    StringUtility::SplitOnce<string,char>(input, token1, token2, ':');
            
    VERIFY_IS_TRUE(token1 == "SOAPAction");
    VERIFY_IS_TRUE(token2 == " \"http://tempuri.org/samples\"");
}
        
BOOST_AUTO_TEST_CASE(SmokeTrimWhitespace1)
{
    string input = "  \t      \ttest   whitespace     removal    \r\n";
    string output = "test   whitespace     removal";
    StringUtility::TrimWhitespaces(input);
    VERIFY_IS_TRUE(input == output);
}

BOOST_AUTO_TEST_CASE(SmokeTrimWhitespace2)
{
    string input = "  \t  \n   Just at the beginning";
    string output = "Just at the beginning";
    StringUtility::TrimWhitespaces(input);
    VERIFY_IS_TRUE(input == output);
}

BOOST_AUTO_TEST_CASE(SmokeTrimSpace1)
{
    string input = "        ";
    string output = "";
    StringUtility::TrimSpaces(input);
    VERIFY_IS_TRUE(input == output);
}

BOOST_AUTO_TEST_CASE(SmokeTrimSpace2)
{
    string input = "Just at the end        ";
    string output = "Just at the end";
    StringUtility::TrimSpaces(input);
    VERIFY_IS_TRUE(input == output);
}

BOOST_AUTO_TEST_CASE(SmokeToUpper1)
{
    string input = "This is a string like ANY other.";
    string output = "THIS IS A STRING LIKE ANY OTHER.";
    StringUtility::ToUpper(input);
    VERIFY_IS_TRUE(input == output);
}

BOOST_AUTO_TEST_CASE(SmokeToUpper2)
{
    string input = "This is a string like ANY other.";
    string output = "THIS IS A STRING LIKE ANY OTHER.";
    StringUtility::ToUpper(input);
    VERIFY_IS_TRUE(input == output);
}

BOOST_AUTO_TEST_CASE(SmokeToLower1)
{
    string input = "This is ANOTHER string like ANY other.";
    string output = "this is another string like any other.";
    StringUtility::ToLower(input);
    VERIFY_IS_TRUE(input == output);
}

BOOST_AUTO_TEST_CASE(SmokeToLower2)
{
    string input = "This is ANOTHER string like ANY other.";
    string output = "this is another string like any other.";
    StringUtility::ToLower(input);
    VERIFY_IS_TRUE(input == output);
}

BOOST_AUTO_TEST_CASE(StartsWithEndsWith)
{
    string s1 = "test";
    VERIFY_IS_TRUE(StringUtility::StartsWith(s1, string("t")));
    VERIFY_IS_TRUE(StringUtility::StartsWith(s1, string("test")));
    VERIFY_IS_TRUE(!StringUtility::StartsWith(s1, string("test1")));

    VERIFY_IS_TRUE(StringUtility::EndsWithCaseInsensitive(s1, string("T")));
    VERIFY_IS_TRUE(StringUtility::EndsWithCaseInsensitive(s1, string("Test")));
    VERIFY_IS_TRUE(!StringUtility::EndsWithCaseInsensitive(s1, string("*Test")));
}

BOOST_AUTO_TEST_CASE(UtfConversion)
{
    const wstring utf16Input = L"this is a test string";
    string utf8Output;
    StringUtility::Utf16ToUtf8(utf16Input, utf8Output);
    Trace.WriteInfo(TraceType, "utf8Output = {0}", utf8Output); 

    const string utf8OutputExpected = "this is a test string";
    BOOST_REQUIRE(utf8Output == utf8OutputExpected);

    wstring utf16Output;
    StringUtility::Utf8ToUtf16(utf8Output, utf16Output);
    Trace.WriteInfo(TraceType, "utf16Output =  {0}", utf16Output); 
    BOOST_REQUIRE(utf16Output == utf16Input);
}

BOOST_AUTO_TEST_CASE(CompareDigitsAsNumbers)
{
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)0, (const char*)"ac") == -1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)0, (const char*)0) == 0);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"a", (const char*)0) == 1);

    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"ab", (const char*)"ac") == -1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"ab", (const char*)"a") == 1);

    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"1", (const char*)"2") == -1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"0001", (const char*)"2") == -1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"10", (const char*)"2") == 1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"10", (const char*)"0002") == 1);

    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"ab1", (const char*)"ab2") == -1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"ab0001", (const char*)"ab2") == -1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"ab10", (const char*)"ab2") == 1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"ab0a", (const char*)"ab0a") == 0);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"ab000a", (const char*)"ab0a") == 0);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"ab2a", (const char*)"ab11a") == -1);

    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"ab222a", (const char*)"ab222c") == -1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"ab000222a", (const char*)"ab222c") == -1);

    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"ab000222a000", (const char*)"ab222a00") == 0);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"ab000222a10", (const char*)"ab222a002") == 1);

    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"2", (const char*)"10") == -1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"10", (const char*)"2") == 1);

    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"UD2", (const char*)"UD10") == -1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"UD10", (const char*)"UD2") == 1);

    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"fabric:/UD/2", (const char*)"fabric:/UD/10") == -1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"fabric:/UD/10", (const char*)"fabric:/UD/2") == 1);

    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"fabric:/UD/UD2", (const char*)"fabric:/UD/UD10") == -1);
    VERIFY_IS_TRUE(StringUtility::CompareDigitsAsNumbers((const char*)"fabric:/UD/UD10", (const char*)"fabric:/UD/UD2") == 1);
}

BOOST_AUTO_TEST_SUITE_END()

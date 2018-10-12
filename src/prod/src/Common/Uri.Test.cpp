// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

#include <boost/test/unit_test.hpp>
#include "Common/boost-taef.h"
#include "Common/Uri.h"
#include "AssertWF.h"

using namespace Common;

namespace Common
{
    class UriTest
    {
    };

    BOOST_FIXTURE_TEST_SUITE(UriTestSuite,UriTest)

    BOOST_AUTO_TEST_CASE(UriCreationTest)
    {
        Uri one("scheme");
        Uri two("scheme", "auth");
        Uri three("scheme", "auth", "path");
        Uri four("scheme", "auth", "path/morePath");
        Uri five("wf", "foo", "bar");

        CODING_ERROR_ASSERT(one.Scheme == two.Scheme);
        CODING_ERROR_ASSERT(one.Segments.empty() && two.Segments.empty());
        CODING_ERROR_ASSERT(two.Authority == three.Authority);
        CODING_ERROR_ASSERT(one.Authority != two.Authority);
        CODING_ERROR_ASSERT(one.Path != four.Path);
        CODING_ERROR_ASSERT(three.Segments.size() == 1);
        CODING_ERROR_ASSERT(four.Segments.size() == 2);
        CODING_ERROR_ASSERT(one.Scheme != five.Scheme);
        CODING_ERROR_ASSERT(two.Authority != five.Authority);
        CODING_ERROR_ASSERT(three.Path != five.Path);
    }

    BOOST_AUTO_TEST_CASE(UriSchemeCompareTest)
    {
        Uri one("red");
        Uri two("Red");
        Uri three("blue");
        Uri four("blue");

        CODING_ERROR_ASSERT(one == two);
        CODING_ERROR_ASSERT(three == four);
        CODING_ERROR_ASSERT(one != three);
    }

    BOOST_AUTO_TEST_CASE(UriAuthorityCompareTest)
    {
        Uri one("green", "square");
        Uri two("green", "Square");            
        Uri three("green", "square");                     
        Uri four("green", "square", "light");
        Uri five("green", "Square", "light");
        Uri six("green", "square", "dark");
        Uri seven("green", "circle", "light");
        Uri eight("yellow", "square", "light");

        CODING_ERROR_ASSERT(one == three);
        CODING_ERROR_ASSERT(one == two);
        CODING_ERROR_ASSERT(one.Scheme == four.Scheme);
        CODING_ERROR_ASSERT(one.Authority == four.Authority);
        CODING_ERROR_ASSERT(one.Scheme == five.Scheme);
        CODING_ERROR_ASSERT(one.Authority == five.Authority);
        CODING_ERROR_ASSERT(five.Scheme == one.Scheme);
        CODING_ERROR_ASSERT(five.Authority == one.Authority);
        CODING_ERROR_ASSERT(five.Scheme == six.Scheme);
        CODING_ERROR_ASSERT(five.Authority == six.Authority);
        CODING_ERROR_ASSERT(six.Scheme == seven.Scheme);
        CODING_ERROR_ASSERT(six.Authority != seven.Authority);
        CODING_ERROR_ASSERT(six.Scheme != eight.Scheme);
        CODING_ERROR_ASSERT(six.Authority == eight.Authority);
    }

    BOOST_AUTO_TEST_CASE(UriPathCompareTest)
    {
        Uri one("gold", "triangle", "dim");
        Uri two("gold", "triangle", "dim");
        Uri three("gold", "triangle", "bright");
        Uri four("gold", "triangle", "Dim");
        Uri five("gold", "octagon", "bright");

        CODING_ERROR_ASSERT(one == two);
        CODING_ERROR_ASSERT(one != three);
        CODING_ERROR_ASSERT(one != four);
        CODING_ERROR_ASSERT(one != five);
        CODING_ERROR_ASSERT(three != five);
    }

    BOOST_AUTO_TEST_CASE(UriToStringTest)
    {
        Uri one("orange", "pentagon", "cloudy");
        Uri two("purple", "hexagon");
        Uri three("fuchsia", "");

        CODING_ERROR_ASSERT(one.ToString() == "orange://pentagon/cloudy");
        CODING_ERROR_ASSERT(two.ToString() == "purple://hexagon");
        CODING_ERROR_ASSERT(three.ToString() == "fuchsia:");
    }

    BOOST_AUTO_TEST_CASE(UriParseTest)
    {
        std::string zero = "amber:";
        std::string one = "pink://";
        std::string two = "brown://decagon";
        std::string three = "black://dodecagon/sparkly";
        std::string four = "olive:/";
        std::string five = "purple//septagon";
        std::string six = "magenta:/torus/drab";
        std::string seven = "oval/pale";
        std::string eight = "black://purple/white/pink";
        std::string nine = "green://yellow:808/rhombus";
        std::string ten = "plaid://127.0.0.1:9000";
        std::string eleven = "chartreuse://[2001:4898:0:fff:0:5efe:10.121.33.93]:103";
        std::string twelve = "red://grey:987654321/";
        std::string legal13 = "indigo://white/space";
        std::string legal14 = "\t\r\n indigo://white/space \n\r\t";
        std::string legal15 = "indigo://white/ space";
        std::string illegal16 = "indigo://wh ite/space";
        std::string illegal17 = "indi go://white/space";
        std::string legal18 = "mauve://circle/no%20white%20space";
        std::string illegal19 = "mauve://circle/no%20white%2/space";
        Uri temp("white", "rhombus", "brilliant");
        std::string tempS = temp.ToString();

        CODING_ERROR_ASSERT(Uri::TryParse(zero, temp));
        CODING_ERROR_ASSERT(temp.ToString() == zero);
        CODING_ERROR_ASSERT(Uri::TryParse(one, temp));
        CODING_ERROR_ASSERT(temp.ToString() == one);
        CODING_ERROR_ASSERT(Uri::TryParse(two, temp));
        CODING_ERROR_ASSERT(temp.ToString() == two);
        CODING_ERROR_ASSERT(Uri::TryParse(three, temp));
        CODING_ERROR_ASSERT(temp.ToString() == three);
        CODING_ERROR_ASSERT(Uri::TryParse(eight, temp));
        CODING_ERROR_ASSERT(temp.ToString() == eight);
        CODING_ERROR_ASSERT(temp.Segments.size() == 2);
        CODING_ERROR_ASSERT(Uri::TryParse(four, temp));
        CODING_ERROR_ASSERT(temp.ToString() == four);
        CODING_ERROR_ASSERT(Uri::TryParse(six, temp));
        CODING_ERROR_ASSERT(temp.ToString() == six);
        CODING_ERROR_ASSERT(Uri::TryParse(legal13, temp));
        CODING_ERROR_ASSERT(temp.ToString() == legal13);
        CODING_ERROR_ASSERT(Uri::TryParse(legal14, temp));
        CODING_ERROR_ASSERT(temp.ToString() == legal13); // mismatch intentional: 13 is same as 14 but without spaces
        CODING_ERROR_ASSERT(Uri::TryParse(legal15, temp));
        CODING_ERROR_ASSERT(temp.ToString() == legal15);
        CODING_ERROR_ASSERT(Uri::TryParse(legal18, temp));
        CODING_ERROR_ASSERT(temp.ToString() == legal18);

        CODING_ERROR_ASSERT(Uri::TryParse(nine, temp));
        CODING_ERROR_ASSERT(temp.ToString() == nine);
        CODING_ERROR_ASSERT(temp.Port == 808);

        CODING_ERROR_ASSERT(Uri::TryParse(ten, temp));
        CODING_ERROR_ASSERT(temp.ToString() == ten);
        CODING_ERROR_ASSERT(temp.Port == 9000);

        CODING_ERROR_ASSERT(Uri::TryParse(eleven, temp));
        CODING_ERROR_ASSERT(temp.ToString() == eleven);
        CODING_ERROR_ASSERT(temp.Port == 103);

        temp = Uri("white", "rhombus", "brilliant");

        CODING_ERROR_ASSERT(!Uri::TryParse(five, temp));
        CODING_ERROR_ASSERT(temp.ToString() == tempS);
        CODING_ERROR_ASSERT(!Uri::TryParse(seven, temp));
        CODING_ERROR_ASSERT(temp.ToString() == tempS);
        CODING_ERROR_ASSERT(!Uri::TryParse(twelve, temp));
        CODING_ERROR_ASSERT(temp.ToString() == tempS);
        CODING_ERROR_ASSERT(!Uri::TryParse(illegal16, temp));
        CODING_ERROR_ASSERT(temp.ToString() == tempS);
        CODING_ERROR_ASSERT(!Uri::TryParse(illegal17, temp));
        CODING_ERROR_ASSERT(temp.ToString() == tempS);
        CODING_ERROR_ASSERT(!Uri::TryParse(illegal19, temp));
        CODING_ERROR_ASSERT(temp.ToString() == tempS);
    }

    BOOST_AUTO_TEST_CASE(LessThanTest)
    {
        Uri one("bronze");
        Uri two("Copper");
        Uri three("silver");
        Uri four("SILVER");

        CODING_ERROR_ASSERT(one < two && two < three);
        CODING_ERROR_ASSERT(!(three < four));

        Uri five("bronze", "parallelogram");
        Uri six("bronze", "Rectangle");
        Uri seven("bronze", "trapezoid");
        Uri eight("bronze", "TRAPEZOID");

        CODING_ERROR_ASSERT(five < six && six < seven);
        CODING_ERROR_ASSERT(!(seven < eight));

        Uri nine("bronze", "parallelogram", "fiery");
        Uri ten("bronze", "parallelogram", "iridescent");
        Uri eleven("bronze", "parallelogram", "shiny");
        Uri twelve("bronze", "parallelogram", "SHINY");

        CODING_ERROR_ASSERT(nine < ten && ten < eleven);
        CODING_ERROR_ASSERT(twelve < eleven);
    }

    BOOST_AUTO_TEST_CASE(UnicodeTest)
    {
        Uri uri;

        // *** arbitrary unicode characters
        std::string unicode("unicodetest1:/");
        unicode.push_back('\u9EAD');
        unicode.push_back('\uAEEF');
        unicode.push_back('/');
        unicode.push_back('\uB00D');
        unicode.push_back('\uCACE');
        unicode.push_back('\u3060'); // Hiragana

        VERIFY_IS_TRUE(Uri::TryParse(unicode, uri));
        VERIFY_IS_TRUE(uri.ToString() == unicode);

        // *** combining diacritical marks
        unicode = "unicodetest2:/";
        unicode.push_back('\uB00D');
        unicode.push_back('\u0300');
        unicode.push_back('\uB00D');
        unicode.push_back('\u032F');
        unicode.push_back('\uB00D');
        unicode.push_back('\u036F');
        unicode.push_back('/');
        unicode.push_back('\uB00D');
        unicode.push_back('\u0300');
        unicode.push_back('\uB00D');
        unicode.push_back('\u032F');
        unicode.push_back('\uB00D');
        unicode.push_back('\u036F');

        VERIFY_IS_TRUE(Uri::TryParse(unicode, uri));
        VERIFY_IS_TRUE(uri.ToString() == unicode);

        // *** combining diacritical marks supplement
        unicode = "unicodetest3:/";
        unicode.push_back('\uB00D');
        unicode.push_back('\u1DC0');
        unicode.push_back('\uB00D');
        unicode.push_back('\u1DA3');
        unicode.push_back('\uB00D');
        unicode.push_back('\u1DFF');
        unicode.push_back('/');
        unicode.push_back('\uB00D');
        unicode.push_back('\u1DC0');
        unicode.push_back('\uB00D');
        unicode.push_back('\u1DA3');
        unicode.push_back('\uB00D');
        unicode.push_back('\u1DFF');

        VERIFY_IS_TRUE(Uri::TryParse(unicode, uri));
        VERIFY_IS_TRUE(uri.ToString() == unicode);

        // *** combining half marks
        unicode = "unicodetest4:/";
        unicode.push_back('\uB00D');
        unicode.push_back('\uFE20');
        unicode.push_back('\uB00D');
        unicode.push_back('\uFE2C');
        unicode.push_back('\uB00D');
        unicode.push_back('\uFE2F');
        unicode.push_back('/');
        unicode.push_back('\uB00D');
        unicode.push_back('\uFE20');
        unicode.push_back('\uB00D');
        unicode.push_back('\uFE2C');
        unicode.push_back('\uB00D');
        unicode.push_back('\uFE2F');

        VERIFY_IS_TRUE(Uri::TryParse(unicode, uri));
        VERIFY_IS_TRUE(uri.ToString() == unicode);

        // *** combining diacritical marks for symbols
        unicode = "unicodetest5:/";
        unicode.push_back('\uB00D');
        unicode.push_back('\u20D0');
        unicode.push_back('\uB00D');
        unicode.push_back('\u20EB');
        unicode.push_back('\uB00D');
        unicode.push_back('\u20FF');
        unicode.push_back('/');
        unicode.push_back('\uB00D');
        unicode.push_back('\u20D0');
        unicode.push_back('\uB00D');
        unicode.push_back('\u20EB');
        unicode.push_back('\uB00D');
        unicode.push_back('\u20FF');

        VERIFY_IS_TRUE(Uri::TryParse(unicode, uri));
        VERIFY_IS_TRUE(uri.ToString() == unicode);
    }

    BOOST_AUTO_TEST_CASE(UnicodeSurrogatePairsTest)
    {
        Uri uri;

        // *** surrogate pairs in the valid range
        std::string unicode("surrogatepairs1:/");

        // first in range
        unicode.push_back(static_cast<char>(0xD800));
        unicode.push_back(static_cast<char>(0xDC00));

        // arbitrary in range
        unicode.push_back(static_cast<char>(0xDAAA));
        unicode.push_back(static_cast<char>(0xDEEB));

        // last in range
        unicode.push_back(static_cast<char>(0xDBFF));
        unicode.push_back(static_cast<char>(0xDFFF));

        unicode.push_back('/');

        // first in range
        unicode.push_back(static_cast<char>(0xD800));
        unicode.push_back(static_cast<char>(0xDC00));

        // arbitrary in range
        unicode.push_back(static_cast<char>(0xDAAA));
        unicode.push_back(static_cast<char>(0xDEEB));

        // last in range
        unicode.push_back(static_cast<char>(0xDBFF));
        unicode.push_back(static_cast<char>(0xDFFF));

        VERIFY_IS_TRUE(Uri::TryParse(unicode, uri));
        VERIFY_IS_TRUE(uri.ToString() == unicode);

        // *** reverse byte order of the above (no byte ordering is assumed)
        unicode = "surrogatepairs2:/";

        // first in range
        unicode.push_back(static_cast<char>(0xDC00));
        unicode.push_back(static_cast<char>(0xD800));

        // arbitrary in range
        unicode.push_back(static_cast<char>(0xDEEB));
        unicode.push_back(static_cast<char>(0xDAAA));

        // last in range
        unicode.push_back(static_cast<char>(0xDFFF));
        unicode.push_back(static_cast<char>(0xDBFF));

        unicode.push_back('/');

        // first in range
        unicode.push_back(static_cast<char>(0xDC00));
        unicode.push_back(static_cast<char>(0xD800));

        // arbitrary in range
        unicode.push_back(static_cast<char>(0xDEEB));
        unicode.push_back(static_cast<char>(0xDAAA));

        // last in range
        unicode.push_back(static_cast<char>(0xDFFF));
        unicode.push_back(static_cast<char>(0xDBFF));

        VERIFY_IS_TRUE(Uri::TryParse(unicode, uri));
        VERIFY_IS_TRUE(uri.ToString() == unicode);

        // *** missing leading code
        unicode = "surrogatepairs3:/";

        // first in range
        unicode.push_back(static_cast<char>(0xDC00));

        // arbitrary in range
        unicode.push_back(static_cast<char>(0xDEEB));

        // last in range
        unicode.push_back(static_cast<char>(0xDFFF));

        unicode.push_back('/');

        // first in range
        unicode.push_back(static_cast<char>(0xDC00));

        // arbitrary in range
        unicode.push_back(static_cast<char>(0xDEEB));

        // last in range
        unicode.push_back(static_cast<char>(0xDFFF));

        VERIFY_IS_FALSE(Uri::TryParse(unicode, uri));

        // *** missing trailing code
        unicode = "surrogatepairs4:/";

        // first in range
        unicode.push_back(static_cast<char>(0xD800));

        // arbitrary in range
        unicode.push_back(static_cast<char>(0xDAAA));

        // last in range
        unicode.push_back(static_cast<char>(0xDBFF));

        unicode.push_back('/');

        // first in range
        unicode.push_back(static_cast<char>(0xD800));

        // arbitrary in range
        unicode.push_back(static_cast<char>(0xDAAA));

        // last in range
        unicode.push_back(static_cast<char>(0xDBFF));

        VERIFY_IS_FALSE(Uri::TryParse(unicode, uri));

        // *** code is surrounded by non-surrogate code
        unicode = "surrogatepairs5:/";

        // first in range
        unicode.push_back(static_cast<char>(0xB00D));
        unicode.push_back(static_cast<char>(0xDC00));
        unicode.push_back(static_cast<char>(0xB00D));

        // arbitrary in range
        unicode.push_back(static_cast<char>(0xDEEB));
        unicode.push_back(static_cast<char>(0xB00D));

        // last in range
        unicode.push_back(static_cast<char>(0xDFFF));
        unicode.push_back(static_cast<char>(0xB00D));

        unicode.push_back('/');

        // first in range
        unicode.push_back(static_cast<char>(0xB00D));
        unicode.push_back(static_cast<char>(0xD800));
        unicode.push_back(static_cast<char>(0xB00D));

        // arbitrary in range
        unicode.push_back(static_cast<char>(0xDAAA));
        unicode.push_back(static_cast<char>(0xB00D));

        // last in range
        unicode.push_back(static_cast<char>(0xDBFF));
        unicode.push_back(static_cast<char>(0xB00D));

        VERIFY_IS_FALSE(Uri::TryParse(unicode, uri));
    }

    BOOST_AUTO_TEST_SUITE_END()
}

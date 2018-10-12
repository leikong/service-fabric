// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

#include <boost/test/unit_test.hpp>
#include "Common/boost-taef.h"
#include "Expression.h"
#include "Operators.h"

using namespace std;

namespace Common
{
    class ExpressionParserTest
    {
    };

    BOOST_FIXTURE_TEST_SUITE(ExpressionParserTestSuite,ExpressionParserTest)

    BOOST_AUTO_TEST_CASE(EmptyString)
    {
        string error;
        map<string, string> params;
        params.insert(make_pair<string, string>("A", "5"));

        ExpressionSPtr compiledExpression = Expression::Build("");
        bool result = compiledExpression->Evaluate(params, error);
        VERIFY_IS_TRUE(result);
        VERIFY_IS_TRUE(error == "");
    }

    BOOST_AUTO_TEST_CASE(ComparisonOperatorTest)
    {
        ExpressionSPtr compiledExpression = Expression::Build("a == 5");
        map<string, string> params;
        params.insert(make_pair<string, string>("a", "5"));
        OperationResult result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "6"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("b", "3"));
        string error;
        bool boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        params.clear();
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        compiledExpression = Expression::Build("a == alpha");

        params.clear();
        params.insert(make_pair<string, string>("a", "6"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "alpha"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("b", "beta"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        compiledExpression = Expression::Build("a != 5");

        params.clear();
        params.insert(make_pair<string, string>("a", "6"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "5"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("b", "3"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        compiledExpression = Expression::Build("a != alpha");

        params.clear();
        params.insert(make_pair<string, string>("a", "alpha"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "6"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("b", "beta"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        compiledExpression = Expression::Build("a >= 5");

        params.clear();
        params.insert(make_pair<string, string>("a", "5"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "4"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("b", "3"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        compiledExpression = Expression::Build("a > 5");

        params.clear();
        params.insert(make_pair<string, string>("a", "6"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "5"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("b", "3"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        compiledExpression = Expression::Build("a < 5");

        params.clear();
        params.insert(make_pair<string, string>("a", "4"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "5"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("b", "3"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        compiledExpression = Expression::Build("a <= 5");

        params.clear();
        params.insert(make_pair<string, string>("a", "5"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "6"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("b", "3"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        compiledExpression = Expression::Build("!(a > 5)");

        params.clear();
        params.insert(make_pair<string, string>("a", "5"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "6"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("b", "3"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        // operator ^ for string
        compiledExpression = Expression::Build("a ^ Str");

        params.clear();
        params.insert(make_pair<string, string>("a", "String"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "Str"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "tring"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("b", "Str"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        // operator ^ for Uri
        compiledExpression = Expression::Build("FaultDomain ^ fd:/DC0");

        params.clear();
        params.insert(make_pair<string, string>("FaultDomain", "fd:/DC0/Rack0/Shelf0"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("FaultDomain", "fd:/DC0/Rack0"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("FaultDomain", "fd:/DC0"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("FaultDomain", "fd:/DC00/Rack0"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        // operator !^ for Uri
        compiledExpression = Expression::Build("FaultDomain !^ fd:/DC0");

        params.clear();
        params.insert(make_pair<string, string>("FaultDomain", "fd:/DC0/Rack0"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("FaultDomain", "fd:/DC0"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("FaultDomain", "fd:/DC1"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        // operator ^P for Uri
        compiledExpression = Expression::Build("FaultDomain ^P fd:/DC0");

        params.clear();
        params.insert(make_pair<string, string>("FaultDomain", "fd:/DC0/Rack0"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("FaultDomain", "fd:/DC00/Rack0"));
        result = compiledExpression->Evaluate(params);
        // the result is true because ^P is skipped when evaluate for regular (non-primary) value
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        result = compiledExpression->Evaluate(params, true);
        // the result is false when evaluate for primary value
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        // numberic inputs are invalid for ^ 
        compiledExpression = Expression::Build("FaultDomain ^ 5");

        params.clear();
        params.insert(make_pair<string, string>("FaultDomain", "55"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        // operator ~ for FDPolicy
        compiledExpression = Expression::Build("FaultDomain ^P fd:/DC0 && FDPolicy ~ Nonpacking");
        VERIFY_IS_TRUE(compiledExpression->FDPolicy == "Nonpacking");

        compiledExpression = Expression::Build("FDPolicy ~ Ignore && a == 5 ");
        VERIFY_IS_TRUE(compiledExpression->FDPolicy == "Ignore");

        compiledExpression = Expression::Build("PlacePolicy ~ NonPartially ");
        VERIFY_IS_TRUE(compiledExpression->PlacePolicy == "NonPartially");

        params.clear();
        params.insert(make_pair<string, string>("a", "5"));
        result = compiledExpression->Evaluate(params, error);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);
    }

    BOOST_AUTO_TEST_CASE(BooleanOperatorTest)
    {
        ExpressionSPtr compiledExpression = Expression::Build("a == 5 && b != 6");
        map<string, string> params;
        params.insert(make_pair<string, string>("a", "5"));
        params.insert(make_pair<string, string>("b", "5"));
        OperationResult result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "6"));
        params.insert(make_pair<string, string>("b", "6"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        string error;
        bool boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        compiledExpression = Expression::Build("a == 5 || b != 6");

        params.clear();
        params.insert(make_pair<string, string>("a", "5"));
        params.insert(make_pair<string, string>("b", "5"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "6"));
        params.insert(make_pair<string, string>("b", "6"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);

        params.clear();
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        params.clear();
        params.insert(make_pair<string, string>("a", "6"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        params.clear();
        params.insert(make_pair<string, string>("b", "6"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        compiledExpression = Expression::Build("(a == 5) == (b == 5)");

        params.clear();
        params.insert(make_pair<string, string>("a", "5"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        params.clear();
        params.insert(make_pair<string, string>("b", "5"));
        error = "";
        boolResult = compiledExpression->Evaluate(params, error);
        VERIFY_ARE_NOT_EQUAL("", error);
        VERIFY_IS_FALSE(boolResult);

        compiledExpression = Expression::Build("! (a == 5)");

        params.clear();
        params.insert(make_pair<string, string>("a", "6"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_TRUE(result.NonLiteral);

        params.clear();
        params.insert(make_pair<string, string>("a", "5"));
        result = compiledExpression->Evaluate(params);
        VERIFY_IS_TRUE(result.IsNonLiteral());
        VERIFY_IS_FALSE(result.NonLiteral);
    }

    BOOST_AUTO_TEST_CASE(InvalidExpressionTest)
    {
        VERIFY_IS_TRUE(Expression::Build("(abc") == nullptr);

        VERIFY_IS_TRUE(Expression::Build("abc)") == nullptr);

        VERIFY_IS_TRUE(Expression::Build("1==") == nullptr);

        VERIFY_IS_TRUE(Expression::Build("==1") == nullptr);

        VERIFY_IS_TRUE(Expression::Build("==") == nullptr);

        VERIFY_IS_TRUE(Expression::Build("!") == nullptr);

        VERIFY_IS_TRUE(Expression::Build("!a && !") == nullptr);

        VERIFY_IS_TRUE(Expression::Build("a!Param)") == nullptr);

        VERIFY_IS_TRUE(Expression::Build("()var") == nullptr);
    }

    BOOST_AUTO_TEST_SUITE_END()
}

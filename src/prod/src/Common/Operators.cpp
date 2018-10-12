// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace std;
using namespace Common;

bool Operator::Match(std::string const& expression, size_t & start)
{
    if (Op.compare("Dummy") == 0)
    {
        return false;
    }

    if (start + Op.size() > expression.size())
    {
        return false;
    }

    for (size_t i = 0; i < Op.size(); i++)
    {
        if (Op[i] != expression[start + i])
        {
            return false;
        }
    }

    start += Op.size();
    return true;
}

OperationResult LiteralOperator::Evaluate(map<string, string> & /*params*/, ExpressionSPtr /*arg1*/, ExpressionSPtr /*arg2*/, bool /*arg3*/)
{
    return OperationResult(value_,false);
}

OperationResult ComparisonOperator::Evaluate(map<string, string> & params, ExpressionSPtr arg1, ExpressionSPtr arg2, bool forPrimary)
{
    OperationResult result1 = arg1->Evaluate(params, forPrimary);
    OperationResult result2 = arg2->Evaluate(params, forPrimary);

    if (result1.IsError())
    {
        return result1;
    }

    if (result2.IsError())
    {
        return result2;
    }

    if (result1.IsNonLiteral() || result2.IsNonLiteral())
    {
        return OperationResult("Comparison Operator is receiving a non literal", true);
    }

    if (forPrimary != this->ForPrimary)
    {
        // Evaluate for primary replica, regular operators are skipped; vice versa
        return true;
    }

    else if (Op.compare("~") == 0)
    {
        // Don't evaluate for FDPolicy
        return true;
    }

    string prop = result1.Literal;
    string value = result2.Literal;

    auto iter = params.find(prop);
    if (iter != params.end())
    {
        prop = iter->second;
        params.erase(iter);
    }
    else
    {
        return OperationResult(formatString.L("Property {0} is not defined", prop), true);
    }

    char *end;
    double val1 = strtod(prop.c_str(), &end);
    bool isNumeric = *end == 0;
    double val2 = strtod(value.c_str(), &end);
    isNumeric = isNumeric && (*end == 0);

    // Check if the input is Uri
    bool isUri = false;
    Uri propUri;
    Uri valueUri;

    if (!isNumeric)
    {
        isUri = Uri::TryParse(prop, propUri);
        if (isUri)
        {
            isUri = Uri::TryParse(value, valueUri);
        }
    }

    string tempOp = Op;
    if (this->ForPrimary)
    {
        tempOp.pop_back();
    }

    if (tempOp.compare("==") == 0)
    {
        if (isNumeric)
        {
            return val1 == val2;
        }
        else if (isUri)
        {
            return propUri == valueUri;
        }
        else
        {
            return prop.compare(value) == 0;
        }
    }
    else if (tempOp.compare("!=") == 0)
    {
        if (isNumeric)
        {
            return val1 != val2;
        }
        else if (isUri)
        {
            return propUri != valueUri;
        }
        else
        {
            return prop.compare(value) != 0;
        }
    }
    else if (tempOp.compare("^") == 0)
    {
        if (isNumeric)
        {
            return OperationResult("Numeric parameter with ^ comparison operator", true);
        }
        else if (isUri)
        {
            return (valueUri.IsPrefixOf(propUri) || valueUri == propUri);
        }
        else
        {
            return prop.size() >= value.size() && StringUtility::StartsWith<std::string>(prop, value);
        }
    }
    else if (tempOp.compare("!^") == 0)
    {
        if (isNumeric)
        {
            return OperationResult("Numeric parameter with !^ comparison operator", true);
        }
        else if (isUri)
        {
             return (!valueUri.IsPrefixOf(propUri) && valueUri != propUri);
        }
        else
        {
            return prop.size() < value.size() || !StringUtility::StartsWith<std::string>(prop, value);
        }
    }
    else if (!isNumeric)
    {
        return OperationResult("Non numeric parameter with non == comparison operator", true);
    }
    else if (tempOp.compare(">=") == 0)
    {
        return val1 >= val2;
    }
    else if (tempOp.compare("<=") == 0)
    {
        return val1 <= val2;
    }
    else if (tempOp.compare("<") == 0)
    {
        return val1 < val2;
    }
    else if (tempOp.compare(">") == 0)
    {
        return val1 > val2;
    }
    else
    {
        return OperationResult("Unkown operator", true);
    }
}

OperationResult BooleanOperator::Evaluate(map<string, string> & params, ExpressionSPtr arg1, ExpressionSPtr arg2, bool forPrimary)
{
    if (Op.compare("!") != 0)
    {
        OperationResult result1 = arg1->Evaluate(params, forPrimary);
        OperationResult result2 = arg2->Evaluate(params, forPrimary);

        if (result1.IsError())
        {
            return result1;
        }

        if (result2.IsError())
        {
            return result2;
        }

        if (!result1.IsNonLiteral() || !result2.IsNonLiteral())
        {
            return OperationResult("Boolean operator is receiving a non literal", true);
        }

        if (Op.compare("&&") == 0)
        {
            return result1.NonLiteral && result2.NonLiteral;
        }
        else if (Op.compare("||") == 0)
        {
            return result1.NonLiteral || result2.NonLiteral;
        }
        else
        {
            return OperationResult("Unkown operator" + Op, true);
        }
    }
    else 
    {
        OperationResult result = arg2->Evaluate(params, forPrimary);

        if (result.IsError())
        {
            return result;
        }

        return !result.NonLiteral;
    }
}

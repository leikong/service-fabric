// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"
#include <stdio.h>

using namespace Common;
using namespace std;

vector<OperatorSPtr>* Expression::CreateOperators()
{
    vector<OperatorSPtr>* operators = new vector<OperatorSPtr>();
    operators->reserve(16);
    operators->push_back(make_shared<LiteralOperator>(1, "("));
    operators->push_back(make_shared<LiteralOperator>(1, ")"));
    operators->push_back(make_shared<BooleanOperator>(2, "||"));
    operators->push_back(make_shared<BooleanOperator>(3, "&&"));
    operators->push_back(make_shared<ComparisonOperator>(4, "=="));
    operators->push_back(make_shared<ComparisonOperator>(4, "!="));
    operators->push_back(make_shared<ComparisonOperator>(4, ">="));
    operators->push_back(make_shared<ComparisonOperator>(4, "<="));
    operators->push_back(make_shared<ComparisonOperator>(4, "<"));
    operators->push_back(make_shared<ComparisonOperator>(4, ">"));
    operators->push_back(make_shared<ComparisonOperator>(4, "^P"));
    operators->push_back(make_shared<ComparisonOperator>(4, "!^P"));
    operators->push_back(make_shared<ComparisonOperator>(4, "^"));
    operators->push_back(make_shared<ComparisonOperator>(4, "!^"));
    operators->push_back(make_shared<ComparisonOperator>(4, "~"));
    operators->push_back(make_shared<BooleanOperator>(6, "!"));
    return operators;
}

vector<OperatorSPtr>* Expression::operators_ = Expression::CreateOperators();
OperatorSPtr* Expression::Sentinal = new OperatorSPtr(make_shared<LiteralOperator>(0, "Dummy"));

ExpressionSPtr Expression::ReadToken(string const& expression, size_t & start)
{
    if (start > expression.size())
    {
        return nullptr;
    }

    size_t i;
    for (i = start; i < expression.size(); i++)
    {
        char c = expression[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')))
        {
            size_t j = i;
            if (ReadOperator(expression, j) != nullptr)
            {
                break;
            }
        }
    }

    string token = expression.substr(start, i - start);
    StringUtility::TrimSpaces(token);
    start = i;
    
    if (token.size() == 0)
    {
        return nullptr;
    }

    return make_shared<Expression>(make_shared<LiteralOperator>(5, token));
}

OperatorSPtr Expression::ReadOperator(string const& expression, size_t & start)
{
	  for (OperatorSPtr const & op : *operators_)
    {
        if (op->Match(expression, start))
        {
            return op;
        }
    }
    return nullptr;
}

bool Expression::Process(stack<ExpressionSPtr> & tokens, stack<OperatorSPtr> & operators)
{
    if (tokens.size() < 2)
    {
        return false;
    }

    OperatorSPtr op = operators.top();
    operators.pop();

    ExpressionSPtr result = make_shared<Expression>(op);
    result->rightChild_ = tokens.top();

    if (result->rightChild_ == nullptr)
    {
        return false;
    }

    tokens.pop();

    result->leftChild_ = tokens.top();
    if (op->Op != "!" && result->leftChild_ == nullptr)
    {
        return false;
    }

    if (op->Op == "~" && result->leftChild_->IsLiteral && result->rightChild_->IsLiteral 
        && (result->leftChild_->op_->Op.compare("FDPolicy") == 0))
    {
        result->FDPolicy_ = result->rightChild_->op_->Op;
    }
    else if (result->leftChild_ != nullptr && !result->leftChild_->FDPolicy.empty())
    {
        result->FDPolicy_ = result->leftChild_->FDPolicy;
    }
    else if (result->rightChild_ != nullptr && !result->rightChild_->FDPolicy.empty())
    {
        result->FDPolicy_ = result->rightChild_->FDPolicy;
    }

    if (op->Op == "~" && result->leftChild_->IsLiteral && result->rightChild_->IsLiteral
        && (result->leftChild_->op_->Op.compare("PlacePolicy") == 0))
    {
        result->PlacePolicy_ = result->rightChild_->op_->Op;
    }
    else if (result->leftChild_ != nullptr && !result->leftChild_->PlacePolicy.empty())
    {
        result->PlacePolicy_ = result->leftChild_->PlacePolicy;
    }
    else if (result->rightChild_ != nullptr && !result->rightChild_->PlacePolicy.empty())
    {
        result->PlacePolicy_ = result->rightChild_->PlacePolicy;
    }

    tokens.pop();

    tokens.push(result);

    return true;
}

ExpressionSPtr Expression::Build(string const& expression)
{
    if (expression.size() == 0)
    {
        return make_shared<Expression>(make_shared<LiteralOperator>(0, "true"));
    }

    std::stack<ExpressionSPtr> tokens;
    std::stack<OperatorSPtr> operators;

    operators.push(*Sentinal);

    size_t start = 0;
    bool isToken = true;
    bool forPrimary = false;

    while (isToken || start < expression.size())
    {
        if (isToken)
        {
            ExpressionSPtr token = ReadToken(expression, start);
            if (token != nullptr)
            {
                tokens.push(token);
            }
        }
        else
        {
            start = expression.find_first_not_of(" \t\r\n", start);
            OperatorSPtr op = ReadOperator(expression, start);
            if (op == nullptr || op->IsInvalid)
            {
                return nullptr;
            }
            if (op->Op == "(")
            {
                operators.push(op);
            }
            else if (op->Op == ")")
            {
                while (!operators.empty() && operators.top()->Op != "(")
                {
                    if (!Process(tokens, operators))
                    {
                        return nullptr;
                    }
                }
                if (operators.empty())
                {
                    return nullptr;
                }
                operators.pop();
                isToken = true;
            }
            else if (op->Op == "!")
            {
                operators.push(op);
                tokens.push(nullptr);
            }
            else
            {
                if (!forPrimary && op->ForPrimary)
                {
                    forPrimary = true;
                }

                while (operators.top()->Level >= op->Level)
                {
                    if (!Process(tokens, operators))
                    {
                        return nullptr;
                    }
                }
                operators.push(op);
            }
        }
        isToken = !isToken;

    }

    while (operators.size() > 1)
    {
        if (!Process(tokens, operators))
        {
            return nullptr;
        }
    }

    if (tokens.size() != 1)
    {
        return nullptr;
    }

    if (forPrimary)
    {
        tokens.top()->forPrimary_ = true;
    }

    return tokens.top();
}

OperationResult Expression::Evaluate(map<string, string> params, bool forPrimary)
{
    return op_->Evaluate(params, leftChild_, rightChild_, forPrimary);
}

bool Expression::Evaluate(map<string, string> params, string & error, bool forPrimary)
{
    OperationResult result  = Evaluate(params, forPrimary);
    if (result.IsError())
    {
        error = result.Literal;
        return false;
    }
    else if (result.IsNonLiteral())
    {
        return result.NonLiteral;
    }
    else 
    {
        return result.Literal == "true";
    }
}

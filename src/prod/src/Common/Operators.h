// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once
#include "OperationResult.h"

namespace Common {
    class Expression;
    typedef std::shared_ptr<Expression> ExpressionSPtr;

    class Operator
    {
    public:
        Operator(int level, std::string const& op)
            : level_(level), op_(op)
        {
        }

        __declspec(property(get=getLevel)) int Level;
        int getLevel() const { return level_; }

        __declspec(property(get=getIsInvalid)) bool IsInvalid;
        bool getIsInvalid() const {return op_.compare("Invalid") == 0; }

        __declspec(property(get=getOp)) std::string const& Op;
        std::string const& getOp() const { return op_; }

        virtual OperationResult Evaluate(std::map<std::string, std::string> & params, ExpressionSPtr arg1, ExpressionSPtr arg2, bool forPrimary) = 0;

        bool Match(std::string const& expression, size_t & start);

        __declspec(property(get=getForPrimary)) bool ForPrimary;
        bool getForPrimary() const {return op_.back() == 'P'; }


    private:
        std::string op_;
        int level_;
    };

    typedef std::shared_ptr<Operator> OperatorSPtr;

    class LiteralOperator : public Operator
    {
    public:
        LiteralOperator(int level, std::string const& op)
            : Operator(level, op), value_(op)
        {
        }

        virtual OperationResult Evaluate(std::map<std::string, std::string> & params, ExpressionSPtr arg1, ExpressionSPtr arg2, bool forPrimary);

    private:
        std::string value_;
    };

    class ComparisonOperator : public Operator
    {
    public:
        ComparisonOperator(int level, std::string const& op)
            : Operator(level, op)
        {
        }

        virtual OperationResult Evaluate(std::map<std::string, std::string> & params, ExpressionSPtr arg1, ExpressionSPtr arg2, bool forPrimary);
    };

    class BooleanOperator : public Operator
    {
    public:
        BooleanOperator(int level, std::string const& op)
            : Operator(level, op)
        {
        }

       virtual OperationResult Evaluate(std::map<std::string, std::string> & params, ExpressionSPtr arg1, ExpressionSPtr arg2, bool forPrimary);
    };
}

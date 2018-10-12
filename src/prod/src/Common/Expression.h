// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class Expression
    {
    public:
        Expression(OperatorSPtr op)
            : op_(op), forPrimary_(false), leftChild_(nullptr), rightChild_(nullptr)
        {
        }

        static ExpressionSPtr Build(std::string const& expression);
        bool Evaluate(std::map<std::string, std::string> params, std::string & error, bool forPrimary = false);
        OperationResult Evaluate(std::map<std::string, std::string> params, bool forPrimary = false);

        __declspec (property(get=get_ForPrimary)) bool ForPrimary;
        bool get_ForPrimary() const { return forPrimary_; }

        __declspec (property(get=get_FDPolicy)) std::string FDPolicy;
        std::string get_FDPolicy() const { return FDPolicy_; }

        __declspec (property(get = get_PlacePolicy)) std::string PlacePolicy;
        std::string get_PlacePolicy() const { return PlacePolicy_; }

        __declspec (property(get=get_IsLiteral)) bool IsLiteral;
        bool get_IsLiteral() const { return (op_->Level == 5 && leftChild_ == nullptr && rightChild_ == nullptr); }

    private:
        static ExpressionSPtr ReadToken(std::string const& expression, size_t & start);

        static OperatorSPtr ReadOperator(std::string const& expression, size_t & start);

        static std::vector<OperatorSPtr>* CreateOperators();

        static bool Process(std::stack<ExpressionSPtr> & tokens, std::stack<OperatorSPtr> & operators);

        OperatorSPtr op_;
        ExpressionSPtr leftChild_;
        ExpressionSPtr rightChild_;
        bool forPrimary_;
        std::string FDPolicy_;
        std::string PlacePolicy_;

        static std::vector<OperatorSPtr>* operators_;
        static OperatorSPtr* Sentinal;
    };
}

// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common 
{
    struct OperationResult
    {
        OperationResult(std::string const& value, bool isError)
            : Type(isError ? "Error" : "Literal"), Literal(value)
        {
        }

        OperationResult(bool value)
            : Type("NonLiteral"), NonLiteral(value)
        {
        }

        OperationResult(OperationResult const & value)
            :Type(value.Type), NonLiteral(value.NonLiteral), Literal(value.Literal)
        {
        }

        
        bool IsError() { return Type.compare("Error") == 0; }
        bool IsNonLiteral() { return Type.compare("NonLiteral") == 0; }

        bool NonLiteral;
        std::string Literal;

    private:
        std::string Type;
    };
}

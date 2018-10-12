// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class PerformanceCounterDefinition
    {
    public:
        PerformanceCounterDefinition(PerformanceCounterId identifier, PerformanceCounterType::Enum type, std::string const & name, std::string const & description);
        PerformanceCounterDefinition(PerformanceCounterId identifier, PerformanceCounterId baseIdentifier, PerformanceCounterType::Enum type, std::string const & name, std::string const & description);
        PerformanceCounterDefinition(PerformanceCounterDefinition const & other);
        PerformanceCounterDefinition(PerformanceCounterDefinition && other);
        ~PerformanceCounterDefinition(void);

        PerformanceCounterDefinition const & operator = (PerformanceCounterDefinition const & other);
        PerformanceCounterDefinition const & operator = (PerformanceCounterDefinition && other);

        __declspec(property(get=get_Identifier)) PerformanceCounterId Identifier;
        inline PerformanceCounterId get_Identifier() const { return identifier_; }

        __declspec(property(get=get_BaseIdentifier)) PerformanceCounterId BaseIdentifier;
        inline PerformanceCounterId get_BaseIdentifier() const { return baseIdentifier_; }

        __declspec(property(get=get_Type)) PerformanceCounterType::Enum Type;
        inline PerformanceCounterType::Enum get_Type() const { return counterType_; }

        __declspec(property(get=get_Name)) std::string const & Name;
        inline std::string const & get_Name() const { return name_; }

        __declspec(property(get=get_Description)) std::string const & Description;
        inline std::string const & get_Description() const { return description_; }

    private:

        PerformanceCounterId identifier_;
        PerformanceCounterId baseIdentifier_;
        PerformanceCounterType::Enum counterType_;

        std::string name_;
        std::string description_;
    };
}


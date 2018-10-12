// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

#include <map>
#include "PerformanceCounterDefinition.h"
#include "PerformanceCounterSetInstanceType.h"

namespace Common
{
    typedef std::map<PerformanceCounterId, PerformanceCounterDefinition> PerformanceCounterDefinitionMap;

    class PerformanceCounterSetDefinition
    {
    public:
        PerformanceCounterSetDefinition(Guid const & identifier, std::string const & name, std::string const & description, PerformanceCounterSetInstanceType::Enum instanceType);
        PerformanceCounterSetDefinition(PerformanceCounterSetDefinition const & other);
        PerformanceCounterSetDefinition(PerformanceCounterSetDefinition && other);
        ~PerformanceCounterSetDefinition(void);

        void AddCounterDefinition(PerformanceCounterDefinition const & PerformanceCounterDefinition);
        void AddCounterDefinition(PerformanceCounterDefinition && PerformanceCounterDefinition);

        PerformanceCounterSetDefinition const & operator = (PerformanceCounterSetDefinition const & other);
        PerformanceCounterSetDefinition const & operator = (PerformanceCounterSetDefinition && other);

        __declspec(property(get=get_CounterDefinitions)) PerformanceCounterDefinitionMap const & CounterDefinitions;
        inline std::map<PerformanceCounterId, PerformanceCounterDefinition> const & get_CounterDefinitions() const { return counterDefinitions_; }
        
        __declspec(property(get=get_Identifier)) Guid const & Identifier;
        inline Guid const & get_Identifier() const { return identifier_; }

        __declspec(property(get=get_Name)) std::string const & Name;
        inline std::string const & get_Name() const { return name_; }

        __declspec(property(get=get_Description)) std::string const & Description;
        inline std::string const & get_Description() const { return description_; }

        __declspec(property(get=get_InstanceType)) PerformanceCounterSetInstanceType::Enum InstanceType;
        inline PerformanceCounterSetInstanceType::Enum get_InstanceType() const { return counterSetInstanceType_; }

    private:

        Guid identifier_;
        PerformanceCounterSetInstanceType::Enum counterSetInstanceType_;

        std::string name_;
        std::string description_;

        PerformanceCounterDefinitionMap counterDefinitions_;
    };

}


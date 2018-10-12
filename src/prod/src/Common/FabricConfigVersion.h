// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once 

namespace Common
{
    class FabricConfigVersion : public Serialization::FabricSerializable
    {
    public:
        // Invalid and Default are defined in FabricVersionConstants.cpp
        static Global<FabricConfigVersion> Invalid;
        static Global<FabricConfigVersion> Default;

        FabricConfigVersion();
        explicit FabricConfigVersion(std::string const & value);
        FabricConfigVersion(FabricConfigVersion const & other);
        FabricConfigVersion(FabricConfigVersion && other);
        virtual ~FabricConfigVersion();

        __declspec(property(get=get_Value)) std::string const & Value;
        std::string const & get_Value() const { return value_; }

        __declspec(property(get=get_IsValid)) bool IsValid;
        bool get_IsValid() const { return !value_.empty(); }

        FabricConfigVersion const & operator = (FabricConfigVersion const & other);
        FabricConfigVersion const & operator = (FabricConfigVersion && other);

        int compare(FabricConfigVersion const & other) const;
        bool operator < (FabricConfigVersion const & other) const;
        bool operator == (FabricConfigVersion const & other) const;
        bool operator != (FabricConfigVersion const & other) const;

        void WriteTo(__in TextWriter&, FormatOptions const &) const;
        
        std::string ToString() const;
        ErrorCode FromString(std::string const &);
        static ErrorCode FromString(std::string const &, __out FabricConfigVersion &);
        static bool TryParse(std::string const &, __out FabricConfigVersion &);
        
        FABRIC_FIELDS_01(value_);

        static std::string AddField(TraceEvent & traceEvent, std::string const & name);
        void FillEventData(TraceEventContext & context) const;

    private:
        std::string value_;
    };
}
DEFINE_USER_ARRAY_UTILITY(Common::FabricConfigVersion);


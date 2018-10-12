// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once 

namespace Common
{
    // <Settings><Section ..><Parameter .../>
    struct ConfigParameter
    {
    public:
        ConfigParameter();
        
        ConfigParameter(
            std::string && name,
            std::string && value,
            bool mustOverride,
            bool isEncrypted,
            std::string && type);

        ConfigParameter(ConfigParameter const & other);
        ConfigParameter(ConfigParameter && other);

        ConfigParameter & operator = (ConfigParameter const & other) = default;
        ConfigParameter & operator = (ConfigParameter && other) = default;

        bool operator == (ConfigParameter const & other) const;
        bool operator != (ConfigParameter const & other) const;

        void WriteTo(TextWriter & w, FormatOptions const &) const;

        void ToPublicApi(__in ScopedHeap & heap, __out FABRIC_CONFIGURATION_PARAMETER & publicParameter) const;

        void clear();
    public:
        std::string Name;
        std::string Value;
        bool MustOverride;
        bool IsEncrypted;
        std::string Type;

    private:
    };
}

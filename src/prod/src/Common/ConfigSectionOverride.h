// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once 

namespace Common
{
    // <ConfigOverride ...><Settings><Section ...>
    struct ConfigSettingsOverride;
    struct ConfigSectionOverride
    {
    public:
        typedef std::map<std::string, ConfigParameterOverride, IsLessCaseInsensitiveComparer<std::string>> ParametersMapType;

        ConfigSectionOverride();
        ConfigSectionOverride(std::string && name, ParametersMapType && parameters);
        ConfigSectionOverride(ConfigSectionOverride const & other);
        ConfigSectionOverride(ConfigSectionOverride && other);

        ConfigSectionOverride const & operator = (ConfigSectionOverride const & other);
        ConfigSectionOverride const & operator = (ConfigSectionOverride && other);

        bool operator == (ConfigSectionOverride const & other) const;
        bool operator != (ConfigSectionOverride const & other) const;

        void WriteTo(TextWriter & w, FormatOptions const &) const;

        void clear();
    public:
        std::string Name;
        ParametersMapType Parameters;
    };
}

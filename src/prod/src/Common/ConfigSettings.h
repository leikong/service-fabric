// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once 

namespace Common
{
    // <Settings>
    struct ConfigSettings
    {
    public:
        typedef std::map<std::string, ConfigSection, IsLessCaseInsensitiveComparer<std::string>> SectionMapType;

        ConfigSettings();
        ConfigSettings(SectionMapType && sections);
        ConfigSettings(ConfigSettings const & other);
        ConfigSettings(ConfigSettings && other);

        ConfigSettings const & operator = (ConfigSettings const & other);
        ConfigSettings const & operator = (ConfigSettings && other);

        bool operator == (ConfigSettings const & other) const;
        bool operator != (ConfigSettings const & other) const;

        void WriteTo(TextWriter & w, FormatOptions const &) const;

        void ToPublicApi(__in ScopedHeap & heap, __out FABRIC_CONFIGURATION_SETTINGS & publicSettings) const;

        void ApplyOverrides(ConfigSettingsOverride const& configSettingsOverride);
        void Merge(ConfigSettings const & other);

        void Set(std::string const & sectionName, ConfigParameter && parameter);

        void Remove(std::string const & sectionName);

        void clear();

    public:
        // TODO: HACK - this should not be public it is violating encapsulation
        SectionMapType Sections;
    };
}


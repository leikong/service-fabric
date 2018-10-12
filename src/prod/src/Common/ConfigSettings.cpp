// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace std;
using namespace Common;

ConfigSettings::ConfigSettings()
    : Sections()
{
}

ConfigSettings::ConfigSettings(SectionMapType && sections)
    : Sections(move(sections))
{
}

ConfigSettings::ConfigSettings(ConfigSettings const & other)
    : Sections(other.Sections)
{
}

ConfigSettings::ConfigSettings(ConfigSettings && other)
    : Sections(move(other.Sections))
{
}

ConfigSettings const & ConfigSettings::operator = (ConfigSettings const & other)
{
    if (this != &other)
    {
        this->Sections = other.Sections;
    }

    return *this;
}

ConfigSettings const & ConfigSettings::operator = (ConfigSettings && other)
{
    if (this != &other)
    {
        this->Sections = move(other.Sections);
    }

    return *this;
}

bool ConfigSettings::operator == (ConfigSettings const & other) const
{
    bool equals = true;

    equals = this->Sections.size() == other.Sections.size();
    if (!equals) { return equals; }

    for(auto iter1 = this->Sections.cbegin(); iter1 != this->Sections.cend(); ++iter1)
    {
        auto iter2 = other.Sections.find(iter1->first);
        if (iter2 == other.Sections.end()) { return false; }

        equals = iter1->second == iter2->second;
        if (!equals) { return equals; }
    }

    return equals;
}

bool ConfigSettings::operator != (ConfigSettings const & other) const
{
    return !(*this == other);
}

void ConfigSettings::WriteTo(TextWriter & w, FormatOptions const &) const
{
    w.Write("ConfigSettings {");

    w.Write("Sections = {");
    for(auto iter = Sections.cbegin(); iter != Sections.cend(); ++ iter)
    {
        w.WriteLine("{0},",*iter);
    }
    w.Write("}");

    w.Write("}");
}

void ConfigSettings::ToPublicApi(__in ScopedHeap & heap, __out FABRIC_CONFIGURATION_SETTINGS & publicSettings) const
{
    auto publicSectionList = heap.AddItem<FABRIC_CONFIGURATION_SECTION_LIST>();
    publicSettings.Sections = publicSectionList.GetRawPointer();

    auto sectionCount = this->Sections.size();
    publicSectionList->Count = static_cast<ULONG>(sectionCount);

    if (sectionCount > 0)
    {
        auto publicItems = heap.AddArray<FABRIC_CONFIGURATION_SECTION>(sectionCount);
        publicSectionList->Items = publicItems.GetRawArray();

        size_t ix = 0;
        for (auto iter = this->Sections.cbegin(); iter != this->Sections.cend(); ++iter)
        {
            iter->second.ToPublicApi(heap, publicItems[ix]);
            ++ix;
        }
    }
}

void ConfigSettings::ApplyOverrides(ConfigSettingsOverride const& configSettingsOverride)
{
    //MustOverride validation has already been done at the Image builder
    for(auto configSettingsOverrideIter = configSettingsOverride.Sections.begin(); configSettingsOverrideIter != configSettingsOverride.Sections.end(); ++configSettingsOverrideIter)
    {
        ConfigSectionOverride const& configSectionOverride = configSettingsOverrideIter->second;
        auto iter1 = Sections.find(configSectionOverride.Name);
        ASSERT_IF(iter1 == Sections.end(), "ConfigSectionOverride {0} not found in ConfigSections", configSectionOverride.Name);
        ConfigSection & configSection = iter1->second;

        for(auto configParameterOverrideIter = configSectionOverride.Parameters.begin(); configParameterOverrideIter != configSectionOverride.Parameters.end(); ++configParameterOverrideIter)
        {
            ConfigParameterOverride const& configParameterOverride = configParameterOverrideIter->second;
            auto iter2 = configSection.Parameters.find(configParameterOverride.Name);
            ASSERT_IF(iter1 == Sections.end(), "ConfigParameterOverride {0} not found in ConfigParameter", configParameterOverride.Name);
            ConfigParameter & configParameter = iter2->second;

            configParameter.Name = configParameterOverride.Name;
            configParameter.Value = configParameterOverride.Value;
            if (!configParameterOverride.Type.empty())
            {
                configParameter.Type = configParameterOverride.Type;
            }
        }
    }
}

void ConfigSettings::Merge(ConfigSettings const & other)
{
    for(auto otherSectionIter = other.Sections.cbegin(); 
        otherSectionIter != other.Sections.cend();
        ++otherSectionIter)
    {
        // find the section in this settings object, if present merge the parameters
        // otherwise add to this settings object
        auto thisSectionIter = this->Sections.find(otherSectionIter->first);
        if (thisSectionIter != this->Sections.end())
        {
            thisSectionIter->second.Merge(otherSectionIter->second);
        }
        else
        {
            this->Sections.insert(make_pair(otherSectionIter->first, otherSectionIter->second));
        }
    }
}

void ConfigSettings::Set(string const & sectionName, ConfigParameter && parameter)
{
    auto it = this->Sections.find(sectionName);
    if (it == this->Sections.end())
    {
        ConfigSection section;
        section.Set(move(parameter));
        this->Sections.insert(make_pair(sectionName, move(section)));
    }
    else
    {
        it->second.Set(move(parameter));
    }
}

void ConfigSettings::Remove(string const & sectionName)
{
    auto it = this->Sections.find(sectionName);
    if (it != this->Sections.end())
    {
        this->Sections.erase(it);
    }
}

void ConfigSettings::clear()
{
    this->Sections.clear();
}

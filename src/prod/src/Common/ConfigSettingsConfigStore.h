// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class ConfigSettingsConfigStore : public ConfigStore
    {
        DENY_COPY(ConfigSettingsConfigStore)

    public:             
        typedef std::function<void(Common::ConfigSettings &)> Preprocessor;

        ConfigSettingsConfigStore(ConfigSettings const & configSettings);
        ConfigSettingsConfigStore(ConfigSettings && configSettings);
        virtual ~ConfigSettingsConfigStore();

        virtual std::string ReadString(
            std::string const & section,
            std::string const & key,
            __out bool & isEncrypted) const; 
       
        virtual void GetSections(
            Common::StringCollection & sectionNames, 
            std::string const & partialName = "") const;

        virtual void GetKeys(
            std::string const & section,
            Common::StringCollection & keyNames, 
            std::string const & partialName = "") const;

        void Set(std::string const & sectionName, ConfigParameter && parameter);

        void Remove(std::string const & sectionName);

        bool Update(ConfigSettings const & updatedSettings);

    private:
        void ProcessAddedOrRemovedSection(
            ConfigSection const & section,
            __inout std::vector<std::pair<std::string, std::string>> & changes);

        void ProcessModifiedSection(
            ConfigSection const & existingSection,
            ConfigSection const & updatedSection,
            __inout std::vector<std::pair<std::string, std::string>> & changes);

        ConfigSettings settings_;
        mutable RwLock lock_;
    };
} 

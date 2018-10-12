// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

#include "Common/ConfigStore.h"
#include "Common/StringUtility.h"

namespace Common
{
    class FileConfigStore;
    typedef std::shared_ptr<FileConfigStore> FileConfigStoreSPtr;

    class FileConfigStore : public ConfigStore
    {
        DENY_COPY(FileConfigStore)

    public:

        // Map with <key, value> pairs for a section
        typedef std::map<std::string, std::string, Common::IsLessCaseInsensitiveComparer<std::string>> SectionMap;
        typedef std::map<std::string, std::string, Common::IsLessCaseInsensitiveComparer<std::string>>::const_iterator SectionMapConstIterator;

        // Map that holds the list of sections found in the document
        typedef std::map<std::string, SectionMap> DocumentMap;
        typedef std::map<std::string, SectionMap>::const_iterator DocumentMapConstIterator;

        // ReadFromString is only for unit tests;
        // to make testing easier, we just pass some strings
        explicit FileConfigStore(std::string const & fileName, bool readFromString = false);
        
        virtual std::string ReadString(
            std::string const & section,
            std::string const & key,
            __out bool & isEncrypted) const; 

        virtual void GetSections(
            Common::StringCollection & sectionNames, 
            std::string const & partialName) const;

        virtual void GetKeys(
            std::string const & section,
            Common::StringCollection & keyNames, 
            std::string const & partialName) const;

        virtual ~FileConfigStore();

    private:
        // Loads the content of the stream, creating sections and key/values
        void Parse(std::string const & str);

        // Adds a section (name and key/values) to the map of sections
        void AddSection(std::string const & crtSectionName, SectionMap const & crtKeyValues);

        // Parses the line, gets the key and the value and adds them to the provided section
        void ParseKeyValue(std::string const & line, SectionMap & crtSection);

        // Parses the line and extracts the section name
        void ParseSectionName(std::string const & line, std::string & crtSectionName);

        // Looks for a section with the provided name.
        // If it exists, returns true 
        // and keyValuesMap contais the map associated with that section;
        // Otherwise, returns false.
        bool GetSectionMap(std::string const & section, SectionMap & keyValuesMap) const;

        // Looks for a key inside the specified section map.
        // If it exists, returns true
        // and sets the key value.
        // Otherwise, returns false.
        bool GetKey(std::string const & key, SectionMap const & keyValueMap, std::string & value) const;

        DocumentMap sections;
    }; // end class FileConfigStore
} // end namespace Common

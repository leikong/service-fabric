//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#pragma once

namespace Common
{
    // Represents a loaded config store
    class ConfigStoreDescription
    {
    public:
        ConfigStoreDescription(
            ConfigStoreSPtr const & store,
            std::string const & storeEnvironmentVariableName,
            std::string const & storeEnvironmentVariableValue) :
            store_(store),
            storeEnvironmentVariableName_(storeEnvironmentVariableName),
            storeEnvironmentVariableValue_(storeEnvironmentVariableValue)
        {
        }

        __declspec(property(get = get_Store)) Common::ConfigStoreSPtr const & Store;
        Common::ConfigStoreSPtr const & get_Store() const { return store_; }

        __declspec(property(get = get_StoreEnvironmentVariableName)) std::string const & StoreEnvironmentVariableName;
        std::string const & get_StoreEnvironmentVariableName() const { return storeEnvironmentVariableName_; }

        __declspec(property(get = get_StoreEnvironmentVariableValue)) std::string const & StoreEnvironmentVariableValue;
        std::string const & get_StoreEnvironmentVariableValue() const { return storeEnvironmentVariableValue_; }

    private:
        ConfigStoreSPtr store_;
        std::string storeEnvironmentVariableName_;
        std::string storeEnvironmentVariableValue_;
    };

    typedef std::unique_ptr<ConfigStoreDescription> ConfigStoreDescriptionUPtr;
}
// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

namespace Common
{
    using namespace std;
    using Common::Environment;
    using Common::StringUtility;

    volatile LONG Config::assertAndTracingConfigInitialized = 0;

    shared_ptr<Config> Config::Create(HMODULE dllModule)
    {
        shared_ptr<Config> result(new Config(dllModule));
        return result;
    }

    Config::Config(ConfigStoreSPtr const & store)
        : store_(store)
    {
        try
        {
            InitAssertAndTracingConfig();
        }
        catch(...)
        {
            store_ = nullptr;
            throw;
        } 
    }

    Config::Config(HMODULE dllModule)
        : store_()
    {
        // TODO: Why does this parameter exist then?
        UNREFERENCED_PARAMETER(dllModule);

        try
        {
            store_ = InitConfigStore();
            InitAssertAndTracingConfig();
        }
        catch(...)
        {
            store_ = nullptr;
            throw;
        }
    }

    Config::~Config()
    {
    }

    void Config::GetKeyValues(std::string const & section, StringMap & entries) const
    {
        StringCollection keys;
        GetKeys(section, keys);

        for (std::string const & key : keys)
        {                
            bool isEncrypted;
            entries[key] = ReadString(section, key, isEncrypted);
        }
    }

    void Config::InitAssertAndTracingConfig()
    {
        if (InterlockedIncrement(&assertAndTracingConfigInitialized) == 1)
        {
            Assert::LoadConfiguration(*this);
            TraceProvider::LoadConfiguration(*this);
        }
    }

    ConfigStoreSPtr Config::InitConfigStore()
    {
        // TODO: Rethink this mechanism of overriding the default configuration store
        ConfigStoreSPtr store = defaultStore_;

        if (!store)
        {
            store = FabricGlobals::Get().GetConfigStore().Store;
        }

        return store;
    }

    ConfigStoreSPtr Config::defaultStore_ = nullptr;

    void Config::SetConfigStore(ConfigStoreSPtr const & store)
    {
        defaultStore_ = store;
    }
}

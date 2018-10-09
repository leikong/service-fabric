//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "FabricGlobals.h"
#include "SymbolPathInitializer.h"

using Common::FabricGlobals;
using Common::ConfigStoreDescription;

Common::Global<FabricGlobals> FabricGlobals::singleton_ = Common::make_global<FabricGlobals>();
FabricGlobals *FabricGlobals::accessor_ = nullptr;

typedef void (*FabricGetGlobalsPtr)(void **globals);

namespace
{
    std::once_flag getOnceFlag, initConfigStoreFlag, initSymbolPathFlag;
}

#ifdef PLATFORM_UNIX
namespace Common
{
    ConfigStoreDescriptionUPtr CreateConfigStore()
    {
        wstring configFilePath = L"config.ini";

        return make_unique<ConfigStoreDescription>(
            make_shared<FileConfigStore>(configFilePath),
            L"",
            L"");
/*
        ConfigStoreType::Enum storeType;
        wstring storeLocation;

        auto error = FabricEnvironment::GetStoreTypeAndLocation(nullptr, storeType, storeLocation);
        ASSERT_IF(!error.IsSuccess(), "FabricEnvironment::GetStoreTypeAndLocation failed with {0}", error);

        ConfigEventSource::Events.ConfigStoreInitialized(ConfigStoreType::ToString(storeType), storeLocation);

        if (storeType == ConfigStoreType::Cfg)
        {
            return make_unique<ConfigStoreDescription>(
                make_shared<FileConfigStore>(storeLocation),
                *FabricEnvironment::FileConfigStoreEnvironmentVariable,
                storeLocation);
        }

        if (storeType == ConfigStoreType::Package)
        {
            auto store = PackageConfigStore::Create(
                storeLocation, 
                L"Fabric.Config",
                [](Common::ConfigSettings & settings) { ConfigLoader::ProcessFabricConfigSettings(settings); });

            return make_unique<ConfigStoreDescription>(
                store,
                *FabricEnvironment::PackageConfigStoreEnvironmentVariable,
                storeLocation);
        }

        if (storeType == ConfigStoreType::SettingsFile)
        {
            auto store = FileXmlSettingsStore::Create(
                storeLocation,
                [](Common::ConfigSettings & settings) { ConfigLoader::ProcessFabricConfigSettings(settings); });

            return make_unique<ConfigStoreDescription>(
                store,
                *FabricEnvironment::SettingsConfigStoreEnvironmentVariable,
                storeLocation);
        }

        if (storeType == ConfigStoreType::None)
        {
            return make_unique<ConfigStoreDescription>(
                make_shared<ConfigSettingsConfigStore>(move(ConfigSettings())),
                L"",
                L"");
        }

        Assert::CodingError("unknown config store type {0}", static_cast<int>(storeType));
*/
    }
}

void FabricGetGlobals(void ** globals)
{
    static std::once_flag fabricGlobalsInitFlag;

    std::call_once(
        fabricGlobalsInitFlag,
        []()
        {
            FabricGlobals::InitializeAsMaster(&Common::CreateConfigStore);
        });

    *globals = reinterpret_cast<void*>(&FabricGlobals::Get());
}
#endif

FabricGlobals & FabricGlobals::Get()
{
    // Already know where the real instance is
    if (accessor_ != nullptr)
    {
        return *accessor_;
    }

    std::call_once(
        getOnceFlag,
        []()
        {
            FabricGlobals* ptr = nullptr;

#ifdef PLATFORM_UNIX
            // For now not doing the optimization on linux to do 
            // dynamic loading of FabricCommon
            ::FabricGetGlobals(reinterpret_cast<void**>(&ptr));     

#else            
            /*
                Note, the code is intentionally not doing FreeLibrary here
                because FabricCommon and its singleton should stay loaded in memory
                after this point. It is safe to do so because if this code is running
                and wants access to globals AND it did not initialize the globals itself
                it must be depending on FabricCommon (by convention)
            */
            auto hInstance = ::LoadLibrary(L"FabricCommon");
            if (hInstance == NULL)
            {
                auto lastError = ::GetLastError();
                Assert::CodingError("FabricGlobals::Failed to load library with {0}", lastError);
            }

            FabricGetGlobalsPtr fptr = nullptr;
            fptr = reinterpret_cast<FabricGetGlobalsPtr>(GetProcAddress(hInstance, "FabricGetGlobals"));

            fptr(reinterpret_cast<void**>(&ptr));
#endif

            accessor_ = ptr;
        });
    
    return *accessor_;
}

ConfigStoreDescription const & FabricGlobals::GetConfigStore() const
{
    if (configStore_ != nullptr)
    {
        return *configStore_;
    }

    std::call_once(initConfigStoreFlag,
        [this]()
        {
            /*
                This assert would fire if no module defines a master instance of FabricGlobals

                In production code there are two scenarios where it can happen:
                - There is a bug in FabricCommon DllMain where it is not declaring itself the master
                - This is a component that is not referenceing FabricCommon and hence it needs to actually 
                  mark itself the master by initializing FabricGlobals

                In test code this is either the test not referencing fabric common or not
                initializing the config using the global fixture
            */
            ASSERT_IF(configLoader_ == nullptr, "FabricGlobals master has not been set and configuration is being initialized");
            configStore_ = (*configLoader_)();

            ASSERT_IF(configStore_ == nullptr, "ConfigLoader failed. See call to InitializeAsMaster to find where it was set");
        });

    return *configStore_;
}

std::string const & FabricGlobals::GetSymbolPath() const
{
    if (!symbolPath_.empty())
    {
        return symbolPath_;
    }

    std::call_once(
        initSymbolPathFlag,
        [this]()
        {
            symbolPath_ = SymbolPathInitializer().InitializeSymbolPaths();
        });

    return symbolPath_;
}

void FabricGlobals::InitializeAsMaster(ConfigLoaderFnPtr configLoaderFunction)
{
    accessor_ = &(*singleton_);
    accessor_->configLoader_ = configLoaderFunction;
}

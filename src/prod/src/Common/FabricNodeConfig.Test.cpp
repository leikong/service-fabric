// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

#include <boost/test/unit_test.hpp>
#include "Common/boost-taef.h"
#include "Common/Config.h"

#define FabricNodeConfigTestTraceType "FabricNodeConfigTest"

namespace Common
{
    using namespace std;

    class FabricNodeConfigTest : protected TextTraceComponent<Common::TraceTaskCodes::Common>
    {
    protected:
        FabricNodeConfigTest() { BOOST_REQUIRE(Setup()); }
        TEST_CLASS_SETUP(Setup);

        void CreateConfigSettings(string const & nodeVersion, __out ConfigSettings & configSettings);
    };

    BOOST_FIXTURE_TEST_SUITE(NodeConfigTests,FabricNodeConfigTest)

    BOOST_AUTO_TEST_CASE(VersionLoadTest)
    {
        ENTER;
      
        ConfigSettings configSettings;
        string nodeVersionV1 = "1.0.960.0:V1:0";

        this->CreateConfigSettings(nodeVersionV1, configSettings);

        auto configStore = make_shared<ConfigSettingsConfigStore>(configSettings);

        // create fabric node configuration using the config settings config store
        auto nodeConfig = make_shared<FabricNodeConfig>(configStore);

        // get the node version
        auto nodeVersion= nodeConfig->NodeVersion;
        WriteInfo(FabricNodeConfigTestTraceType, "nodeConfig->NodeVersion = {0}", nodeVersion.ToString());
        
        // check if it is the same
        VERIFY_IS_TRUE(nodeVersionV1 == nodeVersion.ToString());

        LEAVE;
    }

    BOOST_AUTO_TEST_CASE(VersionUpdateTest)
    {
        ENTER;

         
          FabricVersionInstance updatedNodeVersion;
          shared_ptr<ConfigSettingsConfigStore> configStore;
          FabricNodeConfigSPtr nodeConfig;

        {
            string nodeVersionString = "1.0.960.0:V1.100:100";
            ConfigSettings configSettings;
            this->CreateConfigSettings(nodeVersionString, configSettings);
        
            configStore = make_shared<ConfigSettingsConfigStore>(configSettings);
            nodeConfig = make_shared<FabricNodeConfig>(configStore);

            auto nodeVersion= nodeConfig->NodeVersion;
            WriteInfo(FabricNodeConfigTestTraceType, "nodeConfig->NodeVersion = {0}", nodeVersion.ToString());
            VERIFY_IS_TRUE(nodeVersionString == nodeVersion.ToString());
        }

        // register for configuration change notification
        auto waiter = make_shared<ManualResetEvent>(false);
        nodeConfig->NodeVersionEntry.AddHandler(
            [nodeConfig, waiter, &updatedNodeVersion] (EventArgs const &)
            {
                updatedNodeVersion = nodeConfig->NodeVersion;
                waiter->Set();
            });

        {
            waiter->Reset();

            // change config and instance
            string updatedNodeVersionString = "1.0.960.0:V2.200:200";
            ConfigSettings updatedConfigSettings;
            this->CreateConfigSettings(updatedNodeVersionString, updatedConfigSettings);

            configStore->Update(updatedConfigSettings);
            VERIFY_IS_TRUE(waiter->WaitOne(5000));

            WriteInfo(FabricNodeConfigTestTraceType, "updatedNodeVersion = {0}", updatedNodeVersion.ToString());
            VERIFY_IS_TRUE(updatedNodeVersionString == nodeConfig->NodeVersion.ToString());
        }
        
        {
            waiter->Reset();

            // change instance only
            string updatedNodeVersionString = "1.0.960.0:V2.201:201";
            ConfigSettings updatedConfigSettings;
            this->CreateConfigSettings(updatedNodeVersionString, updatedConfigSettings);

            configStore->Update(updatedConfigSettings);
            VERIFY_IS_TRUE(waiter->WaitOne(5000));

            WriteInfo(FabricNodeConfigTestTraceType, "updatedNodeVersion = {0}", updatedNodeVersion.ToString());
            VERIFY_IS_TRUE(updatedNodeVersionString == nodeConfig->NodeVersion.ToString());
        }
        
        {
            waiter->Reset();

            // change code 
            string updatedNodeVersionString = "1.0.961.0:V2.202:202";
            ConfigSettings updatedConfigSettings;
            this->CreateConfigSettings(updatedNodeVersionString, updatedConfigSettings);

            configStore->Update(updatedConfigSettings);
            VERIFY_IS_TRUE(waiter->WaitOne(5000));

            WriteInfo(FabricNodeConfigTestTraceType, "updatedNodeVersion = {0}", updatedNodeVersion.ToString());
            VERIFY_IS_TRUE(updatedNodeVersionString == nodeConfig->NodeVersion.ToString());
        }
        
        {
            waiter->Reset();

            // change code 
            string updatedNodeVersionString = "2.0.961.0:V2.204:204";
            ConfigSettings updatedConfigSettings;
            this->CreateConfigSettings(updatedNodeVersionString, updatedConfigSettings);

            configStore->Update(updatedConfigSettings);
            VERIFY_IS_TRUE(waiter->WaitOne(5000));

            WriteInfo(FabricNodeConfigTestTraceType, "updatedNodeVersion = {0}", updatedNodeVersion.ToString());
            VERIFY_IS_TRUE(updatedNodeVersionString == nodeConfig->NodeVersion.ToString());
        }
        
        LEAVE;
    }

    BOOST_AUTO_TEST_SUITE_END()

    void FabricNodeConfigTest::CreateConfigSettings(string const & nodeVersion, __out ConfigSettings & configSettings)
    {
         // create configuration settings store
        ConfigSection fabricNodeConfigSection;
        fabricNodeConfigSection.Name = "FabricNode";
        
        ConfigParameter nodeVersionParam;
        nodeVersionParam.Name = "NodeVersion";
        nodeVersionParam.Value = nodeVersion;
        fabricNodeConfigSection.Parameters[nodeVersionParam.Name] = nodeVersionParam;
        
        configSettings.Sections[fabricNodeConfigSection.Name] = fabricNodeConfigSection;
        WriteInfo(FabricNodeConfigTestTraceType, "Configuration Settings: {0}", configSettings);
    }
    bool FabricNodeConfigTest::Setup()
    {
        // ensures that the trace settings are loaded from the default configuration store
        DllConfig::GetConfig();

        return true;
    }
}

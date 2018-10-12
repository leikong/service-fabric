// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class Config : public std::enable_shared_from_this<Config>
    {
        DENY_COPY(Config);
    public:
        static std::shared_ptr<Config> Create(HMODULE dllModule = 0);

        Config(ConfigStoreSPtr const & store);
        Config(HMODULE dllModule = 0);
        ~Config();

        static ConfigStoreSPtr InitConfigStore();

        void RegisterForUpdate(std::string const & section, IConfigUpdateSink* sink)
        {
            store_->RegisterForUpdate(section, sink);
        }

        void UnregisterForUpdate(IConfigUpdateSink const* sink)
        {
            store_->UnregisterForUpdate(sink);
        }

#pragma region( Parse )
        template<typename T>
        static bool TryParse(T & result, std::string const & value);

        template<>
        bool TryParse<std::string>(std::string & result, std::string const & value)
        {
            result = value;
            return true;
        }

        template<>
        bool TryParse<SecureString>(SecureString & result, std::string const & value)
        {
            result = SecureString(value);
            return true;
        }

        template<>
        bool TryParse<int64>(int64 & result, std::string const & value)
        {
            if (!TryParseInt64(value, result))
            {
                return false;
            }

            return true;
        }

        template<>
        bool TryParse<int>(int & result, std::string const & value)
        {
            int64 temp;
            if (!TryParseInt64(value, temp))
            {
                return false;
            }

            result = static_cast<int>(temp);
            return true;
        }

        template<>
        bool TryParse<uint>(uint & result, std::string const & value)
        {
            int64 temp;
            if (!TryParseInt64(value, temp) || temp < 0)
            {
                return false;
            }

            result = static_cast<uint>(temp);
            return true;
        }
        
        template<>
        bool TryParse<double>(double & result, std::string const & value)
        {
            // TODO: not handling incorrect format yet
            result = Double_Parse(value);
            return true;
        }

        template<>
        bool TryParse<TimeSpan>(TimeSpan & result, std::string const & value)
        {
            double seconds;
            if (!TryParse<double>(seconds, value) || seconds < 0.0)
            {
                return false;
            }

            result = TimeSpan::FromSeconds(seconds);
            return true;
        }

        template<>
        bool TryParse<bool>(bool & result, std::string const & value)
        {
            result = StringUtility::AreEqualCaseInsensitive(value, "true");
            return true;
        }

        template<>
        bool TryParse<StringCollection>(StringCollection & result, std::string const & value)
        {
            StringUtility::Split<std::string>(value, result, ",");
            return true;
        }

        template<>
        bool TryParse<FabricVersionInstance>(FabricVersionInstance & result, std::string const & value)
        {
            auto error = FabricVersionInstance::FromString(value, result);
            return error.IsSuccess();
        }
		
		template<>
        bool TryParse<X509StoreLocation::Enum>(X509StoreLocation::Enum & result, std::string const & value)
        {
            auto error = X509StoreLocation::Parse(value, result);
            return error.IsSuccess();
        }

        template<typename T>
        static T Parse(std::string const & value)
        {
            T result;
            TryParse(result, value);

            return result;
        }

#pragma endregion

        // If a parameter is encypted, this method returns the decrypted value.
        template<typename T>        
        bool ReadUnencryptedConfig(std::string const & section, std::string const & key, T & result, T const & defaultValue)
        {
            bool isEncrypted;
            std::string stringValue = ReadString(section, key, isEncrypted);

            ASSERT_IF(isEncrypted, "This method does not support encrypted configuration. Section={0}, Key={1}", section, key);

            if (stringValue.empty())
            {
                result = defaultValue;
                return true;
            }

            return TryParse<T>(result, stringValue);
        }        

        // This method returns a string and also indicates whether the string is encrypted through "isEncrypted"        
        std::string ReadString(std::string const & section, std::string const & key, bool & isEncrypted) const
        {            
            return store_->ReadString(section, key, isEncrypted);
        }

        void GetSections(
            Common::StringCollection & sectionNames, 
            std::string const & partialName = "") const 
        {
            store_->GetSections(sectionNames, partialName);
        }

        void GetKeys(
            std::string const & section,
            Common::StringCollection & keyNames, 
            std::string const & partialName = "") const 
        {
            return store_->GetKeys(section, keyNames, partialName);
        }

        void GetKeyValues(std::string const & section, StringMap & entries) const;

        // sets the config store to use, if not set appropriate 
        // config store will be created based on the store provider
        // settings obtained from the environment variables
        static void SetConfigStore(ConfigStoreSPtr const & store);

        static ConfigStoreSPtr & Test_DefaultConfigStore()
        {
            return defaultStore_;
        }        

    private:        
        void InitAssertAndTracingConfig();
        static LONG volatile assertAndTracingConfigInitialized;
        static ConfigStoreSPtr defaultStore_;

        ConfigStoreSPtr store_;
    };
}

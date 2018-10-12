// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Transport
{
    class UnreliableTransportConfig : Common::ComponentConfig
    {
        DECLARE_SINGLETON_COMPONENT_CONFIG(UnreliableTransportConfig, "UnreliableTransport")

        INTERNAL_CONFIG_ENTRY(Common::TimeSpan, "UnreliableTransport", MaxAllowedDelayInSeconds, Common::TimeSpan::FromSeconds(40), Common::ConfigEntryUpgradePolicy::Static)

    public:

        bool AddSpecification(UnreliableTransportSpecificationUPtr && spec);

        bool AddSpecification(std::string const & name, std::string const & data);

        bool RemoveSpecification(std::string const & name);

        Common::TimeSpan GetDelay(std::string const & source, std::string const & destination, std::string const & action);
        
        Common::TimeSpan GetDelay(std::string const & source, std::string const & destination, Transport::Message const & message);

        void WriteTo(Common::TextWriter & w, Common::FormatOptions const &) const;

        virtual void Initialize();

        Common::ErrorCode StartMonitoring(std::string const & workFolder, std::string const & nodeId);

        Common::ErrorCode Test_StopMonitoring(std::string const & nodeId);

        static Common::ErrorCode ReadFromINI(std::string const & fileName, std::vector<std::string> & parameters, bool shouldLockFile = true);

        static Common::ErrorCode ReadFromINI(std::string const & fileName, std::vector<std::pair<std::string, std::string> > & parameters, bool shouldLockFile = true);

        static Common::ErrorCode WriteToINI(std::string const & fileName, std::vector<std::pair<std::string, std::string> > const & parameters, bool shouldLockFile = true);

        bool AddSpecificationNoSourceNode(std::string const & nodeId, std::string const & name, std::string const & data);

        bool IsDisabled();

        static Common::GlobalString SettingsFileName;

    private:
        typedef std::vector<UnreliableTransportSpecificationUPtr> SpecTable;

        void LoadConfiguration(Common::Config & config);

        void LoadConfigurationFromINI(std::string const & fileName, std::string const & nodeId, bool shouldLockFile = true);

        static Common::ErrorCode ReadTextFile(std::string const & fileName, std::string & text);

        static Common::ErrorCode WriteTextFile(std::string const & fileName, std::vector<std::pair<std::string, std::string> > const & parameters);

        void ClearNodeIdSpecifications(std::string const & nodeId);

        void UpdateIsUnreliableTransportEnabled();

        SpecTable specifications_;
        Common::RwLock specsTableLock_;

        static BOOL CALLBACK InitSingleton(PINIT_ONCE, PVOID, PVOID *);
        static INIT_ONCE initOnce_;
        static UnreliableTransportConfig* Singleton;

        bool isDisabledUnreliableTransport_;
        std::map<std::string, Common::FileChangeMonitor2SPtr> nodeIdFileChangeMonitor;     // dictionary of file monitors where the key is the nodeid
        Common::RwLock fileChangeMonitorDictionaryLock_;
    };
}

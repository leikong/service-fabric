// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Transport
{
    class UnreliableTransportConfiguration : Common::ComponentConfig
    {
        DECLARE_SINGLETON_COMPONENT_CONFIG(UnreliableTransportConfiguration, "UnreliableTransport")

        INTERNAL_CONFIG_ENTRY(Common::TimeSpan, "UnreliableTransport", MaxAllowedDelayInSeconds, Common::TimeSpan::FromSeconds(40), Common::ConfigEntryUpgradePolicy::Dynamic)

    public:
        bool AddSpecification(UnreliableTransportSpecificationUPtr && spec);

        bool AddSpecification(std::string const & name, std::string const & data);

        bool RemoveSpecification(std::string const & name);

        Common::TimeSpan GetDelay(std::string const & source, std::string const & destination, std::string const & action);

        void WriteTo(Common::TextWriter & w, Common::FormatOptions const &) const;

        virtual void Initialize();

    private:
        typedef std::vector<UnreliableTransportSpecificationUPtr> SpecTable;

        void LoadConfiguration(Common::Config & config);

        SpecTable specifications_;
        Common::RwLock specsTableLock_;

        static BOOL CALLBACK InitSingleton(PINIT_ONCE, PVOID, PVOID *);
        static INIT_ONCE initOnce_;
        static UnreliableTransportConfiguration* Singleton;
    };
}

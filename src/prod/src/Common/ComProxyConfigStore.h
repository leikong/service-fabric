// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class ComProxyConfigStore : public ConfigStore
    {
        DENY_COPY(ComProxyConfigStore)

    public:
        typedef std::function<HRESULT(REFIID, IFabricConfigStoreUpdateHandler *, void **)> GetConfigStoreImpl;

    public:
        explicit ComProxyConfigStore(ComPointer<IFabricConfigStore2> const & store);
        virtual ~ComProxyConfigStore();

        virtual bool GetIgnoreUpdateFailures() const;

        virtual void SetIgnoreUpdateFailures(bool value);

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

        static ConfigStoreSPtr Create(GetConfigStoreImpl const & getConfigStore);

    private:
        bool DispatchUpdate(std::string const & section, std::string const & key);
        bool DispatchCheckUpdate(std::string const & section, std::string const & key, std::string const & value, bool isEncrypted);
    
    private:
        class ComConfigStoreUpdateHandler;
        ComPointer<IFabricConfigStore2> const store_;
    };
}

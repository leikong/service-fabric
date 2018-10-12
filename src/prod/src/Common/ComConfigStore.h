// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class ComConfigStore : 
        public IFabricConfigStore2, 
        protected ComUnknownBase
    {
        DENY_COPY(ComConfigStore)

        BEGIN_COM_INTERFACE_LIST(ComConfigStore)
            COM_INTERFACE_ITEM(IID_IUnknown,IFabricConfigStore)
            COM_INTERFACE_ITEM(IID_IFabricConfigStore,IFabricConfigStore)
            COM_INTERFACE_ITEM(IID_IFabricConfigStore2,IFabricConfigStore2)
        END_COM_INTERFACE_LIST()

    public:
        ComConfigStore(ConfigStoreSPtr const & store, ComPointer<IFabricConfigStoreUpdateHandler> const & updateHandler);
        virtual ~ComConfigStore();

        HRESULT STDMETHODCALLTYPE GetSections( 
            /* [in] */ LPCSTR partialSectionName,
            /* [retval][out] */ IFabricStringListResult **result);
        
        HRESULT STDMETHODCALLTYPE GetKeys( 
            /* [in] */ LPCSTR sectionName,
            /* [in] */ LPCSTR partialKeyName,
            /* [retval][out] */ IFabricStringListResult **result);
        
        HRESULT STDMETHODCALLTYPE ReadString( 
            /* [in] */ LPCSTR section,
            /* [in] */ LPCSTR key,
            /* [out] */ BOOLEAN * isEncrypted,
            /* [retval][out] */ IFabricStringResult **result);

        BOOLEAN STDMETHODCALLTYPE get_IgnoreUpdateFailures(void);
        
        void STDMETHODCALLTYPE set_IgnoreUpdateFailures(BOOLEAN value);

    private:
        ULONG STDMETHODCALLTYPE TryAddRef();
        bool OnUpdate(std::string const & section, std::string const & key);
        bool CheckUpdate(std::string const & section, std::string const & key, std::string const & value, bool isEncrypted);

        class ConfigUpdateSink;
        ConfigStoreSPtr store_;
        ComPointer<IFabricConfigStoreUpdateHandler> updateHandler_;
        std::shared_ptr<ConfigUpdateSink> updateSink_;
    };
}

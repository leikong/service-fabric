// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class ComStringResult
        : public IFabricStringResult,
          protected ComUnknownBase
    {
        DENY_COPY(ComStringResult)

        COM_INTERFACE_LIST1(
            ComStringResult,
            IID_IFabricStringResult,
            IFabricStringResult)

    public:
        ComStringResult(std::string const & value);
        virtual ~ComStringResult();
        LPCSTR STDMETHODCALLTYPE get_String(void);

        static HRESULT ReturnStringResult(std::string const & result, IFabricStringResult **value);

    private:
        std::string value_;
    };

    class ComSecureStringResult
        : public IFabricStringResult,
        protected ComUnknownBase
    {
        DENY_COPY(ComSecureStringResult)

            COM_INTERFACE_LIST1(
            ComSecureStringResult,
            IID_IFabricStringResult,
            IFabricStringResult)

    public:
        ComSecureStringResult(SecureString const & value);
        virtual ~ComSecureStringResult();
        LPCSTR STDMETHODCALLTYPE get_String(void);

        static HRESULT ReturnStringResult(SecureString const & result, IFabricStringResult **value);

    private:
        SecureString value_;
    };

    class ComStringCollectionResult : 
        public IFabricStringListResult,
        protected ComUnknownBase
    {
        DENY_COPY(ComStringCollectionResult)
        
        COM_INTERFACE_LIST1(
            ComStringCollectionResult,
            IID_IFabricStringListResult,
            IFabricStringListResult)

    public:
        ComStringCollectionResult(StringCollection && collection);
        virtual ~ComStringCollectionResult();

        virtual HRESULT STDMETHODCALLTYPE GetStrings( 
            /* [out] */ ULONG *itemCount,
            /* [retval][out] */ const LPCSTR **bufferedItems);

        static HRESULT ReturnStringCollectionResult(StringCollection && result, IFabricStringListResult **value);

    private:
        Common::ScopedHeap heap_;
        Common::ReferenceArray<LPCSTR> stringCollection_;
    };
}



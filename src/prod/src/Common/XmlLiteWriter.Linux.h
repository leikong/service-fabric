// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

#define U_HAVE_INT64_T  1
#define U_HAVE_UINT64_T 1
#include <libxml/xmlwriter.h>

namespace Common
{

    class XmlLiteWriterOutputStream : public IStream, public IUnknown
    {
    public: 
        XmlLiteWriterOutputStream(xmlBufferPtr buf) : buf_(buf), pos_(0),refCount_(0) { };
        virtual ~XmlLiteWriterOutputStream() { };

    public:
        virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
                REFIID riid,
                _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);
        virtual ULONG STDMETHODCALLTYPE AddRef( void);
        virtual ULONG STDMETHODCALLTYPE Release( void);

    public: 
        virtual HRESULT STDMETHODCALLTYPE Read( 
            __out_bcount_part(cb, *pcbRead)  void *pv,
            ULONG cb,
            __out_opt  ULONG *pcbRead);
        
        virtual HRESULT STDMETHODCALLTYPE Write( 
            __in_bcount(cb)  const void *pv,
            ULONG cb,
            __out_opt  ULONG *pcbWritten);

    public:
        virtual HRESULT STDMETHODCALLTYPE Seek( 
            LARGE_INTEGER dlibMove,
            DWORD dwOrigin,
            __out_opt  ULARGE_INTEGER *plibNewPosition);
        
        virtual HRESULT STDMETHODCALLTYPE SetSize( 
            ULARGE_INTEGER libNewSize);
        
        virtual HRESULT STDMETHODCALLTYPE CopyTo( 
            IStream *pstm,
            ULARGE_INTEGER cb,
            __out_opt  ULARGE_INTEGER *pcbRead,
            __out_opt  ULARGE_INTEGER *pcbWritten);
        
        virtual HRESULT STDMETHODCALLTYPE Commit( 
            DWORD grfCommitFlags);
        
        virtual HRESULT STDMETHODCALLTYPE Revert(void);
        
        virtual HRESULT STDMETHODCALLTYPE LockRegion( 
            ULARGE_INTEGER libOffset,
            ULARGE_INTEGER cb,
            DWORD dwLockType);
        
        virtual HRESULT STDMETHODCALLTYPE UnlockRegion( 
            ULARGE_INTEGER libOffset,
            ULARGE_INTEGER cb,
            DWORD dwLockType);
        
        virtual HRESULT STDMETHODCALLTYPE Stat( 
            __RPC__out STATSTG *pstatstg,
            DWORD grfStatFlag);
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            __RPC__deref_out_opt IStream **ppstm);

    public:
        xmlBufferPtr GetXmlBuffer() { return buf_; }

    private:
        xmlBufferPtr buf_;
        int pos_;
        LONG refCount_;
    };

    class XmlLiteWriter : public IXmlWriter, public IUnknown
    {
    public: 
        XmlLiteWriter(xmlTextWriterPtr wr) : writer_(wr),refCount_(0) { };

    public:
        virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
                REFIID riid,
                _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);
        virtual ULONG STDMETHODCALLTYPE AddRef( void);
        virtual ULONG STDMETHODCALLTYPE Release( void);

    public:
        virtual HRESULT STDMETHODCALLTYPE SetOutput( 
            _In_opt_  IUnknown *pOutput);
        
        virtual HRESULT STDMETHODCALLTYPE GetProperty( 
            _In_  UINT nProperty,
            _Out_  LONG_PTR *ppValue);
        
        virtual HRESULT STDMETHODCALLTYPE SetProperty( 
            _In_  UINT nProperty,
            _In_opt_  LONG_PTR pValue);
        
        virtual HRESULT STDMETHODCALLTYPE WriteAttributes( 
            _In_  IXmlReader *pReader,
            _In_  BOOL fWriteDefaultAttributes);
        
        virtual HRESULT STDMETHODCALLTYPE WriteAttributeString( 
            _In_opt_  LPCSTR pwszPrefix,
            _In_opt_  LPCSTR pwszLocalName,
            _In_opt_  LPCSTR pwszNamespaceUri,
            _In_opt_  LPCSTR pwszValue);
        
        virtual HRESULT STDMETHODCALLTYPE WriteCData( 
            _In_opt_  LPCSTR pwszText);
        
        virtual HRESULT STDMETHODCALLTYPE WriteCharEntity( 
            _In_  CHAR wch);
        
        virtual HRESULT STDMETHODCALLTYPE WriteChars( 
            _In_reads_opt_(cwch)  const CHAR *pwch,
            _In_  UINT cwch);
        
        virtual HRESULT STDMETHODCALLTYPE WriteComment( 
            _In_opt_  LPCSTR pwszComment);
        
        virtual HRESULT STDMETHODCALLTYPE WriteDocType( 
            _In_opt_  LPCSTR pwszName,
            _In_opt_  LPCSTR pwszPublicId,
            _In_opt_  LPCSTR pwszSystemId,
            _In_opt_  LPCSTR pwszSubset);
        
        virtual HRESULT STDMETHODCALLTYPE WriteElementString( 
            _In_opt_  LPCSTR pwszPrefix,
            _In_      LPCSTR pwszLocalName,
            _In_opt_  LPCSTR pwszNamespaceUri,
            _In_opt_  LPCSTR pwszValue);
        
        virtual HRESULT STDMETHODCALLTYPE WriteEndDocument( void);
        
        virtual HRESULT STDMETHODCALLTYPE WriteEndElement( void);
        
        virtual HRESULT STDMETHODCALLTYPE WriteEntityRef( 
            _In_  LPCSTR pwszName);
        
        virtual HRESULT STDMETHODCALLTYPE WriteFullEndElement( void);
        
        virtual HRESULT STDMETHODCALLTYPE WriteName( 
            _In_  LPCSTR pwszName);
        
        virtual HRESULT STDMETHODCALLTYPE WriteNmToken( 
            _In_  LPCSTR pwszNmToken);
        
        virtual HRESULT STDMETHODCALLTYPE WriteNode( 
            _In_  IXmlReader *pReader,
            _In_  BOOL fWriteDefaultAttributes);
        
        virtual HRESULT STDMETHODCALLTYPE WriteNodeShallow( 
            _In_  IXmlReader *pReader,
            _In_  BOOL fWriteDefaultAttributes);
        
        virtual HRESULT STDMETHODCALLTYPE WriteProcessingInstruction( 
            _In_  LPCSTR pwszName,
            _In_opt_  LPCSTR pwszText);
        
        virtual HRESULT STDMETHODCALLTYPE WriteQualifiedName( 
            _In_  LPCSTR pwszLocalName,
            _In_opt_  LPCSTR pwszNamespaceUri);
        
        virtual HRESULT STDMETHODCALLTYPE WriteRaw( 
            _In_opt_  LPCSTR pwszData);
        
        virtual HRESULT STDMETHODCALLTYPE WriteRawChars( 
            _In_reads_opt_(cwch)  const CHAR *pwch,
            _In_  UINT cwch);
        
        virtual HRESULT STDMETHODCALLTYPE WriteStartDocument( 
            _In_  XmlStandalone standalone);
        
        virtual HRESULT STDMETHODCALLTYPE WriteStartElement( 
            _In_opt_  LPCSTR pwszPrefix,
            _In_  LPCSTR pwszLocalName,
            _In_opt_  LPCSTR pwszNamespaceUri);
        
        virtual HRESULT STDMETHODCALLTYPE WriteString( 
            _In_opt_  LPCSTR pwszText);
        
        virtual HRESULT STDMETHODCALLTYPE WriteSurrogateCharEntity( 
            _In_  CHAR wchLow,
            _In_  CHAR wchHigh);
        
        virtual HRESULT STDMETHODCALLTYPE WriteWhitespace( 
            _In_opt_  LPCSTR pwszWhitespace);
        
        virtual HRESULT STDMETHODCALLTYPE Flush( void);

    private:
        xmlTextWriterPtr writer_;
        LONG refCount_;
    };
}


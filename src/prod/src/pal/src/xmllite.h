// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

#include "internal/pal_common.h"
#include "objidl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef IUnknown IXmlReaderInput;
typedef IUnknown IXmlWriterOutput;

STDAPI CreateXmlReader(_In_ REFIID riid,
    _In_ LPCSTR ppwszFileName,
    _Outptr_ void ** ppvObject,
    _In_opt_ IMalloc * pMalloc);

STDAPI CreateXmlWriter(_In_ REFIID riid,
    _In_ LPCSTR ppwszFileName,
    _Out_ void ** ppvObject,
    _In_opt_ IMalloc * pMalloc);

// LINUX specific API
STDAPI CreateMemoryXmlWriter(
    _In_ REFIID riid,
    IUnknown *output,
    _Outptr_ void **ppvObject,
    _In_opt_ IMalloc *pMalloc);

// 122
STDAPI CreateXmlWriterOutputWithEncodingName(
    __in IUnknown *pOutputStream,
    __in_opt IMalloc *pMalloc,
    __in LPCSTR pwszEncodingName,
    __out IXmlWriterOutput **ppOutput);

typedef 
enum XmlNodeType
    {	XmlNodeType_None	= 0,
	XmlNodeType_Element	= 1,
	XmlNodeType_Attribute	= 2,
	XmlNodeType_Text	= 3,
	XmlNodeType_CDATA	= 4,
	XmlNodeType_ProcessingInstruction	= 7,
	XmlNodeType_Comment	= 8,
	XmlNodeType_DocumentType	= 10,
	XmlNodeType_Whitespace	= 13,
	XmlNodeType_EndElement	= 15,
	XmlNodeType_XmlDeclaration	= 17,
	_XmlNodeType_Last	= 17
    } 	XmlNodeType;

typedef 
enum XmlStandalone
    {	XmlStandalone_Omit	= 0,
	XmlStandalone_Yes	= 1,
	XmlStandalone_No	= 2,
	_XmlStandalone_Last	= 2
    } 	XmlStandalone;

typedef 
enum XmlWriterProperty
    {
        XmlWriterProperty_MultiLanguage	= 0,
        XmlWriterProperty_Indent	= ( XmlWriterProperty_MultiLanguage + 1 ) ,
        XmlWriterProperty_ByteOrderMark	= ( XmlWriterProperty_Indent + 1 ) ,
        XmlWriterProperty_OmitXmlDeclaration	= ( XmlWriterProperty_ByteOrderMark + 1 ) ,
        XmlWriterProperty_ConformanceLevel	= ( XmlWriterProperty_OmitXmlDeclaration + 1 ) ,
        _XmlWriterProperty_Last	= XmlWriterProperty_OmitXmlDeclaration
    } 	XmlWriterProperty;

EXTERN_C const IID IID_IXmlReader;

MIDL_INTERFACE("7279FC81-709D-4095-B63D-69FE4B0D9030")
IXmlReader : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetInput( 
        /*  */ 
        __in_opt  IUnknown *pInput) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetProperty( 
        /*  */ 
        __in  UINT nProperty,
        /*  */ 
        __out  LONG_PTR *ppValue) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetProperty( 
        /*  */ 
        __in  UINT nProperty,
        /*  */ 
        __in_opt  LONG_PTR pValue) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE Read( 
        /*  */ 
        __out_opt  XmlNodeType *pNodeType) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetNodeType( 
        /*  */ 
        __out  XmlNodeType *pNodeType) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE MoveToFirstAttribute( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE MoveToNextAttribute( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE MoveToAttributeByName( 
        /*  */ 
        __in  LPCSTR pwszLocalName,
        /*  */ 
        __in_opt  LPCSTR pwszNamespaceUri) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE MoveToElement( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetQualifiedName( 
        /*  */ 
        __out  LPCSTR *ppwszQualifiedName,
        /*  */ 
        __out_opt  UINT *pcwchQualifiedName) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetNamespaceUri( 
        /*  */ 
        __out  LPCSTR *ppwszNamespaceUri,
        /*  */ 
        __out_opt  UINT *pcwchNamespaceUri) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetLocalName( 
        /*  */ 
        __out  LPCSTR *ppwszLocalName,
        /*  */ 
        __out_opt  UINT *pcwchLocalName) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetPrefix( 
        /*  */ 
        __out  LPCSTR *ppwszPrefix,
        /*  */ 
        __out_opt  UINT *pcwchPrefix) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetValue( 
        /*  */ 
        __out  LPCSTR *ppwszValue,
        /*  */ 
        __out_opt  UINT *pcwchValue) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE ReadValueChunk( 
        /*  */ 
        __out_ecount_part(cwchChunkSize, *pcwchRead)  CHAR *pwchBuffer,
        /*  */ 
        __in  UINT cwchChunkSize,
        /*  */ 
        __inout  UINT *pcwchRead) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetBaseUri( 
        /*  */ 
        __out  LPCSTR *ppwszBaseUri,
        /*  */ 
        __out_opt  UINT *pcwchBaseUri) = 0;
    
    virtual BOOL STDMETHODCALLTYPE IsDefault( void) = 0;
    
    virtual BOOL STDMETHODCALLTYPE IsEmptyElement( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetLineNumber( 
        /*  */ 
        __out  UINT *pnLineNumber) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetLinePosition( 
        /*  */ 
        __out  UINT *pnLinePosition) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetAttributeCount( 
        /*  */ 
        __out  UINT *pnAttributeCount) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetDepth( 
        /*  */ 
        __out  UINT *pnDepth) = 0;
    
    virtual BOOL STDMETHODCALLTYPE IsEOF( void) = 0;
    
};

EXTERN_C const IID IID_IXmlWriter;

MIDL_INTERFACE("7279FC88-709D-4095-B63D-69FE4B0D9030")
IXmlWriter : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetOutput( 
        /*  */ 
        __in_opt  IUnknown *pOutput) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetProperty( 
        /*  */ 
        __in  UINT nProperty,
        /*  */ 
        __out  LONG_PTR *ppValue) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetProperty( 
        /*  */ 
        __in  UINT nProperty,
        /*  */ 
        __in_opt  LONG_PTR pValue) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteAttributes( 
        /*  */ 
        __in  IXmlReader *pReader,
        /*  */ 
        __in  BOOL fWriteDefaultAttributes) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteAttributeString( 
        /*  */ 
        __in  LPCSTR pwszPrefix,
        /*  */ 
        __in  LPCSTR pwszLocalName,
        /*  */ 
        __in  LPCSTR pwszNamespaceUri,
        /*  */ 
        __in  LPCSTR pwszValue) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteCData( 
        /*  */ 
        __in  LPCSTR pwszText) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteCharEntity( 
        /*  */ 
        __in  CHAR wch) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteChars( 
        /*  */ 
        __in_ecount(cwch)  const CHAR *pwch,
        /*  */ 
        __in  UINT cwch) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteComment( 
        /*  */ 
        __in  LPCSTR pwszComment) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteDocType( 
        /*  */ 
        __in  LPCSTR pwszName,
        /*  */ 
        __in  LPCSTR pwszPublicId,
        /*  */ 
        __in  LPCSTR pwszSystemId,
        /*  */ 
        __in  LPCSTR pwszSubset) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteElementString( 
        /*  */ 
        __in  LPCSTR pwszPrefix,
        /*  */ 
        __in  LPCSTR pwszLocalName,
        /*  */ 
        __in  LPCSTR pwszNamespaceUri,
        /*  */ 
        __in  LPCSTR pwszValue) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteEndDocument( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteEndElement( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteEntityRef( 
        /*  */ 
        __in  LPCSTR pwszName) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteFullEndElement( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteName( 
        /*  */ 
        __in  LPCSTR pwszName) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteNmToken( 
        /*  */ 
        __in  LPCSTR pwszNmToken) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteNode( 
        /*  */ 
        __in  IXmlReader *pReader,
        /*  */ 
        __in  BOOL fWriteDefaultAttributes) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteNodeShallow( 
        /*  */ 
        __in  IXmlReader *pReader,
        /*  */ 
        __in  BOOL fWriteDefaultAttributes) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteProcessingInstruction( 
        /*  */ 
        __in  LPCSTR pwszName,
        /*  */ 
        __in  LPCSTR pwszText) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteQualifiedName( 
        /*  */ 
        __in  LPCSTR pwszLocalName,
        /*  */ 
        __in  LPCSTR pwszNamespaceUri) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteRaw( 
        /*  */ 
        __in  LPCSTR pwszData) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteRawChars( 
        /*  */ 
        __in_ecount(cwch)  const CHAR *pwch,
        /*  */ 
        __in  UINT cwch) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteStartDocument( 
        /*  */ 
        __in  XmlStandalone standalone) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteStartElement( 
        /*  */ 
        __in  LPCSTR pwszPrefix,
        /*  */ 
        __in  LPCSTR pwszLocalName,
        /*  */ 
        __in  LPCSTR pwszNamespaceUri) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteString( 
        /*  */ 
        __in  LPCSTR pwszText) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteSurrogateCharEntity( 
        /*  */ 
        __in  CHAR wchLow,
        /*  */ 
        __in  CHAR wchHigh) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteWhitespace( 
        /*  */ 
        __in  LPCSTR pwszWhitespace) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE Flush( void) = 0;
    
};

#ifdef __cplusplus
}
#endif

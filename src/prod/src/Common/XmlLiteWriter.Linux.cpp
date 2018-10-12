// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"
#include <codecvt>
#include <algorithm>

using namespace std;
using namespace Common;

// UTF-8 to UTF-16 Conversion. TODO: char seems still 4-bytes in to_bytes()
static string utf8to16(const char *str)
{
    string_convert<codecvt_utf8_utf16<char16_t>, char16_t> conv;
    u16string u16str = conv.from_bytes(str);
    return string((char *) u16str.c_str());
}

static string utf16to8(const char *wstr)
{
    string_convert<codecvt_utf8_utf16<char16_t>, char16_t> conv;
    return conv.to_bytes((const char16_t *) wstr);
}

static HRESULT xmlResultConv(int xmlResult)
{
    return (xmlResult == 1 ? S_OK : (xmlResult == 0 ? S_FALSE : E_FAIL));
}

static HRESULT xmlBytesResultConv(int xmlResult)
{
    return xmlResult == -1 ? E_FAIL : S_OK;
}

STDAPI CreateXmlWriter(_In_ REFIID riid, _In_ LPCSTR ppwszFileName, _Outptr_ void **ppvObject, _In_opt_ IMalloc *pMalloc)
{
    string filename = utf16to8(ppwszFileName);
    xmlTextWriterPtr xmlWriter = xmlNewTextWriterFilename(filename.c_str(), 0);
    if (xmlWriter != NULL)
    {
        *ppvObject = new XmlLiteWriter(xmlWriter);
        ((XmlLiteWriter*) (*ppvObject))->AddRef();
        xmlTextWriterStartDocument(xmlWriter, NULL, "utf-8", NULL);
    }
    return (xmlWriter != NULL ? S_OK : E_FAIL);
}

STDAPI CreateMemoryXmlWriter(_In_ REFIID riid, _In_ IUnknown* output, _Outptr_ void **ppvObject, _In_opt_ IMalloc *pMalloc)
{
    void* outputStream = output; 
    xmlTextWriterPtr xmlWriter = xmlNewTextWriterMemory(((XmlLiteWriterOutputStream*)outputStream)->GetXmlBuffer(), 0);
    if (xmlWriter != NULL)
    {
        *ppvObject = new XmlLiteWriter(xmlWriter);
        ((XmlLiteWriter*) (*ppvObject))->AddRef();
        xmlTextWriterStartDocument(xmlWriter, NULL, "utf-8", NULL);
    }
    return (xmlWriter != NULL ? S_OK : E_FAIL);
}

LWSTDAPI_(IStream *) SHCreateMemStream(
    _In_reads_bytes_opt_(cbInit) const BYTE *pInit,
    _In_ UINT cbInit
    )
{
    xmlBufferPtr buf = xmlBufferCreate();
    IStream *writerOutputStream = new XmlLiteWriterOutputStream(buf);
    return writerOutputStream;
}

STDAPI CreateXmlWriterOutputWithEncodingName (
    _In_ IUnknown *pOutputStream,
    _In_opt_ IMalloc *pMalloc,
    _In_ LPCSTR pwszEncodingName,
    _Out_ IXmlWriterOutput **ppOutput)
{
    *ppOutput = (IXmlWriterOutput*) pOutputStream;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriterOutputStream::QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	*ppvObject = this;
	return S_OK;
}

ULONG STDMETHODCALLTYPE XmlLiteWriterOutputStream::AddRef(void)
{
	return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE XmlLiteWriterOutputStream::Release(void)
{
	LONG result = InterlockedDecrement(&refCount_);
	ASSERT(result >= 0);

	if (result == 0)
	{
            //delete this;
	}
	return result;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriterOutputStream::Read(
    __out_bcount_part(cb, *pcbRead)  void *pv,
    ULONG cb,
    __out_opt  ULONG *pcbRead)
{
    cb /= sizeof(char);

    ULONG left = buf_->use - pos_;
    ULONG sz = left > cb ? cb : left;

    char *buf = new char[sz + 1];

    memset(buf, 0, sz + 1);
    memcpy(buf, buf_->content + pos_, sz);
    string wstr = utf8to16(buf);

    memcpy(pv, wstr.c_str(), sz * sizeof(char));
    *pcbRead = sz * sizeof(char);

    delete[] buf;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriterOutputStream::Write(
    __in_bcount(cb)  const void *pv,
    ULONG cb,
    __out_opt  ULONG *pcbWritten)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriterOutputStream::Seek(
    LARGE_INTEGER dlibMove,
    DWORD dwOrigin,
    __out_opt  ULARGE_INTEGER *plibNewPosition)
{
    // only support STREAM_SEEK_SET for now
    if (dwOrigin == STREAM_SEEK_SET && dlibMove.LowPart < buf_->use)
    {
        if (plibNewPosition != NULL)
        {
            plibNewPosition->HighPart = 0;
            plibNewPosition->LowPart = dlibMove.LowPart;
        }
        pos_ = (int) dlibMove.LowPart / sizeof(char);
        return S_OK;
    }
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriterOutputStream::Stat(
    __RPC__out STATSTG *pstatstg,
    DWORD grfStatFlag)
{
    memset(pstatstg, 0, sizeof(*pstatstg));
    pstatstg->cbSize.LowPart = buf_->use * sizeof(char);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriterOutputStream::SetSize(
    ULARGE_INTEGER libNewSize)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriterOutputStream::CopyTo(
    IStream *pstm,
    ULARGE_INTEGER cb,
    __out_opt  ULARGE_INTEGER *pcbRead,
    __out_opt  ULARGE_INTEGER *pcbWritten)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriterOutputStream::Commit(
    DWORD grfCommitFlags)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriterOutputStream::Revert(void)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriterOutputStream::LockRegion(
    ULARGE_INTEGER libOffset,
    ULARGE_INTEGER cb,
    DWORD dwLockType)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriterOutputStream::UnlockRegion(
    ULARGE_INTEGER libOffset,
    ULARGE_INTEGER cb,
    DWORD dwLockType)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriterOutputStream::Clone(
    __RPC__deref_out_opt IStream **ppstm)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
{
    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE XmlLiteWriter::AddRef(void)
{
    return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE XmlLiteWriter::Release(void)
{
    LONG result = InterlockedDecrement(&refCount_);
    ASSERT(result >= 0);

    if (result == 0)
    {
        //delete this;
        if (writer_ != NULL)
        {
            xmlTextWriterEndDocument(writer_);
            xmlFreeTextWriter(writer_);
            writer_ = NULL;
        }
    }
    return result;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::SetOutput( 
    _In_opt_  IUnknown *pOutput)
{
    // libxml2 does not use IStream - just return success.
    return 0;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::GetProperty( 
    _In_  UINT nProperty,
    _Out_  LONG_PTR *ppValue)
{
    // property of XmlWriter - not implemented. 
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::SetProperty( 
    _In_  UINT nProperty,
    _In_opt_  LONG_PTR pValue)
{
    HRESULT result = S_OK;

    switch (nProperty)
    {
        case XmlWriterProperty_ByteOrderMark:
            break;
        case XmlWriterProperty_Indent:
            xmlTextWriterSetIndent(writer_, pValue);
            break;
        default:
            result = E_NOTIMPL;
            break;
    }
    return result;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteAttributes( 
    _In_  IXmlReader *pReader,
    _In_  BOOL fWriteDefaultAttributes)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteAttributeString( 
    _In_opt_  LPCSTR pwszPrefix,
    _In_opt_  LPCSTR pwszLocalName,
    _In_opt_  LPCSTR pwszNamespaceUri,
    _In_opt_  LPCSTR pwszValue)
{
    string pszPrefix = utf16to8(pwszPrefix);
    string pszLocalName = utf16to8(pwszLocalName);
    string pszNamespaceUri = utf16to8(pwszNamespaceUri);
    string pszValue = utf16to8(pwszValue);

    int result;
    result = xmlTextWriterWriteAttributeNS(
                writer_, 
                (const xmlChar *) (pszPrefix.c_str()[0] ? pszPrefix.c_str() : NULL), 
                (const xmlChar *) pszLocalName.c_str(), 
                (const xmlChar *) (pszNamespaceUri.c_str()[0] ? pszNamespaceUri.c_str() : NULL), 
                (const xmlChar *) pszValue.c_str());

    return xmlBytesResultConv(result);
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteCData( 
    _In_opt_  LPCSTR pwszText)
{
    string pszText = utf16to8(pwszText);
    int result = xmlTextWriterWriteCDATA(writer_, (const xmlChar *) pszText.c_str());
    return xmlBytesResultConv(result);
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteCharEntity( 
    _In_  CHAR wch)
{
    int result = xmlTextWriterWriteBinHex(writer_, (const char*)&wch, 0, sizeof(CHAR));
    return xmlBytesResultConv(result);
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteChars( 
    _In_reads_opt_(cwch)  const CHAR *pwch,
    _In_  UINT cwch)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteComment( 
    _In_opt_  LPCSTR pwszComment)
{
    string pszComment = utf16to8(pwszComment);
    int result = xmlTextWriterWriteComment(writer_, (const xmlChar *) pszComment.c_str()); 
    return xmlBytesResultConv(result);
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteDocType( 
    _In_opt_  LPCSTR pwszName,
    _In_opt_  LPCSTR pwszPublicId,
    _In_opt_  LPCSTR pwszSystemId,
    _In_opt_  LPCSTR pwszSubset)
{
    string pszName = utf16to8(pwszName);
    string pszPublicId = utf16to8(pwszPublicId);
    string pszSystemId = utf16to8(pwszSystemId);
    string pszSubset = utf16to8(pwszSubset);

    int result = xmlTextWriterWriteDTD(
            writer_, 
            (const xmlChar *) pszName.c_str(), 
            (const xmlChar *) pszPublicId.c_str(), 
            (const xmlChar *) pszSystemId.c_str(), 
            (const xmlChar *) pszSubset.c_str());

    return xmlBytesResultConv(result);
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteElementString( 
    _In_opt_  LPCSTR pwszPrefix,
    _In_      LPCSTR pwszLocalName,
    _In_opt_  LPCSTR pwszNamespaceUri,
    _In_opt_  LPCSTR pwszValue)
{
    string pszPrefix = utf16to8(pwszPrefix);
    string pszLocalName = utf16to8(pwszLocalName);
    string pszNamespaceUri = utf16to8(pwszNamespaceUri);
    string pszValue  = utf16to8(pwszValue);

    int result = xmlTextWriterWriteElementNS(
            writer_, 
            (const xmlChar *) (pszPrefix.c_str()[0] ? pszPrefix.c_str() : NULL), 
            (const xmlChar *) pszLocalName.c_str(), 
            (const xmlChar *) (pszNamespaceUri.c_str()[0] ? pszNamespaceUri.c_str() : NULL),
            (const xmlChar *) pszValue.c_str());

    return xmlBytesResultConv(result);
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteEndDocument(void)
{
    int result = xmlTextWriterEndDocument(writer_);
    return xmlBytesResultConv(result);
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteEndElement(void)
{
    int result = xmlTextWriterEndElement(writer_);
    return xmlBytesResultConv(result);
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteEntityRef( 
    _In_  LPCSTR pwszName)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteFullEndElement( void)
{
    int result = xmlTextWriterFullEndElement(writer_);
    return xmlBytesResultConv(result);
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteName( 
    _In_  LPCSTR pwszName)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteNmToken( 
    _In_  LPCSTR pwszNmToken)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteNode( 
    _In_  IXmlReader *pReader,
    _In_  BOOL fWriteDefaultAttributes)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteNodeShallow( 
    _In_  IXmlReader *pReader,
    _In_  BOOL fWriteDefaultAttributes)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteProcessingInstruction( 
    _In_  LPCSTR pwszName,
    _In_opt_  LPCSTR pwszText)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteQualifiedName( 
    _In_  LPCSTR pwszLocalName,
    _In_opt_  LPCSTR pwszNamespaceUri)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteRaw( 
    _In_opt_  LPCSTR pwszData)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteRawChars( 
    _In_reads_opt_(cwch)  const CHAR *pwch,
    _In_  UINT cwch)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteStartDocument( 
    _In_  XmlStandalone standalone)
{
    int result = xmlTextWriterStartDocument(writer_, NULL, NULL, NULL);
    return xmlBytesResultConv(result);
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteStartElement( 
    _In_opt_  LPCSTR pwszPrefix,
    _In_  LPCSTR pwszLocalName,
    _In_opt_  LPCSTR pwszNamespaceUri)
{
    string pszPrefix = utf16to8(pwszPrefix);
    string pszLocalName = utf16to8(pwszLocalName);
    string pszNamespaceUri = utf16to8(pwszNamespaceUri);
    int result = xmlTextWriterStartElementNS(writer_, 
            (const xmlChar *) (pszPrefix.c_str()[0] ? pszPrefix.c_str() : NULL),
            (const xmlChar *) pszLocalName.c_str(),
            (const xmlChar *) (pszNamespaceUri.c_str()[0] ? pszNamespaceUri.c_str() : NULL));
    return xmlBytesResultConv(result);
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteString( 
    _In_opt_  LPCSTR pwszText)
{
    string pszText = utf16to8(pwszText);
    int result = xmlTextWriterWriteString(writer_, (const xmlChar *) pszText.c_str());
    return xmlBytesResultConv(result);
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteSurrogateCharEntity( 
    _In_  CHAR wchLow,
    _In_  CHAR wchHigh)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::WriteWhitespace( 
    _In_opt_  LPCSTR pwszWhitespace)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE XmlLiteWriter::Flush(void)
{
    int result = xmlTextWriterFlush(writer_);
    return (result != -1 ? S_OK : E_FAIL);
}

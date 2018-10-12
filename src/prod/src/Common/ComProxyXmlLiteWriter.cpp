// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace std;
using namespace Common;

StringLiteral const TraceType_XmlLiteWriter = "XmlLiteWriter";

ComProxyXmlLiteWriter::ComProxyXmlLiteWriter(ComPointer<IXmlWriter> writer)
    : writer_(writer), 
    outputName_()
{    
}

ComProxyXmlLiteWriter::~ComProxyXmlLiteWriter()
{
    Close();
}

ErrorCode ComProxyXmlLiteWriter::Create(
    __in std::string const & outputName,
    __out ComProxyXmlLiteWriterUPtr & xmlLiteWriter, 
    bool writeByteOrderMark, 
    bool indent)
{
#if !defined(PLATFORM_UNIX)
    ComPointer<IStream> xmlFileStream;
    auto error = ComXmlFileStream::Open(outputName, true, xmlFileStream);
    if (!error.IsSuccess()) { return error; }

    ComPointer<IUnknown> comPointer;
    HRESULT hr = xmlFileStream->QueryInterface(IID_IUnknown, reinterpret_cast<void**>(comPointer.InitializationAddress()));
    if (!SUCCEEDED(hr)) { return ErrorCode::FromHResult(hr); }

    error = ComProxyXmlLiteWriter::Create(comPointer, xmlLiteWriter, writeByteOrderMark, indent);
    if (!error.IsSuccess()) { return error; }

#else
    ComPointer<IXmlWriter> writer;
    auto hr = ::CreateXmlWriter(
        ::IID_IXmlWriter, 
        outputName.c_str(),
        writer.VoidInitializationAddress(),
        NULL);
    auto error = ToErrorCode(hr, "CreateXmlWriter", "", true, false);
    if (!error.IsSuccess()) { return error; }

    xmlLiteWriter = move(make_unique<ComProxyXmlLiteWriter>(writer));
#endif

    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode ComProxyXmlLiteWriter::Create(
    ComPointer<IUnknown> const& output,
    __out ComProxyXmlLiteWriterUPtr & xmlLiteWriter,
    bool writeByteOrderMark,
    bool indent)
{
    ComPointer<IXmlWriter> writer;

#if !defined(PLATFORM_UNIX)
    auto hr = ::CreateXmlWriter(
        ::IID_IXmlWriter,
        writer.VoidInitializationAddress(),
        NULL);
#else
    auto hr = ::CreateMemoryXmlWriter(
        ::IID_IXmlWriter,
        output.GetRawPointer(),
        writer.VoidInitializationAddress(),
        NULL);
#endif

    auto error = ToErrorCode(hr, "CreateXmlWriter", "", true, false);
    if (!error.IsSuccess()) { return error; }

    if (writeByteOrderMark == false)
    {
        hr = writer->SetProperty(XmlWriterProperty_ByteOrderMark, FALSE);
        error = ToErrorCode(hr, "SetProperty", "", true, false);
        if (!error.IsSuccess()) { return error; }
    }
    if (indent)
    {
        hr = writer->SetProperty(XmlWriterProperty_Indent, TRUE);
        error = ToErrorCode(hr, "SetProperty", "", true, false);
        if (!error.IsSuccess()) { return error; }
    }

    xmlLiteWriter = move(make_unique<ComProxyXmlLiteWriter>(writer));

#if !defined(PLATFORM_UNIX)
    error = xmlLiteWriter->SetOutput("CustomOutput", output);
    if (!error.IsSuccess()) { return error; }
#endif

    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode ComProxyXmlLiteWriter::SetOutput(string const & outputName, ComPointer<IUnknown> const & output)
{
    auto hr = writer_->SetOutput(output.GetRawPointer());
    auto error = ToErrorCode(hr, "SetOutput", outputName, true, false);
    if (error.IsSuccess()) 
    { 
        outputName_ = outputName;
    }

    return error;
}

ErrorCode ComProxyXmlLiteWriter::WriteNode(ComPointer<IXmlReader> const& reader, bool writeDefaultAttributes)
{
    HRESULT hr = writer_->WriteNode(reader.GetRawPointer(), writeDefaultAttributes);

    // The ToErrorCode method below has an option to accept S_FALSE as a success HRESULT value. This parameter must be set to true, or else
    // this method returns an error.
    // While XMLWriter normally returns S_OK as a success value, WriteNode does not.
    return ToErrorCode(hr, "WriteNode", true, true);
}

ErrorCode ComProxyXmlLiteWriter::Flush()
{
    HRESULT hr = writer_->Flush();
    return ToErrorCode(hr, "Flush", true, false);
}

ErrorCode ComProxyXmlLiteWriter::WriteStartDocument(XmlStandalone value) 
{
    HRESULT hr = writer_->WriteStartDocument(value);
    return ToErrorCode(hr, "StartDocument", true, false);
}

ErrorCode ComProxyXmlLiteWriter::WriteAttribute(std::string const & attrName, std::string const & value,
    std::string const & prefix, std::string const & nameSpace) 
{
    HRESULT hr = writer_->WriteAttributeString(prefix.c_str(), attrName.c_str(), nameSpace.c_str(), value.c_str());
    return ToErrorCode(hr, "WriteAttribute", true, false);
}

ErrorCode ComProxyXmlLiteWriter::WriteComment(std::string const & comment)
{
    HRESULT hr = writer_->WriteComment(comment.c_str());
    return ToErrorCode(hr, "WriteComment", true, false);
}

ErrorCode ComProxyXmlLiteWriter::WriteDocType(std::string const & name, std::string const & pubId, std::string const & sysid,
    std::string const & subset)
{
    HRESULT hr = writer_->WriteDocType(name.c_str(), pubId.c_str(), sysid.c_str(), subset.c_str());
    return ToErrorCode(hr, "WriteDocType", true, false);
}

ErrorCode ComProxyXmlLiteWriter::WriteChars(char const* charPtr)
{
    HRESULT hr = writer_->WriteString(charPtr);
    return ToErrorCode(hr, "WriteChars", true, false);
}

ErrorCode ComProxyXmlLiteWriter::WriteString(std::string const & content)
{
    HRESULT hr = writer_->WriteString(content.c_str());
    return ToErrorCode(hr, "WriteChars", true, false);
}

ErrorCode ComProxyXmlLiteWriter::WriteStartElement(std::string const & name, std::string const & prefix,
    std::string const & nameSpace)
{
    HRESULT hr = writer_->WriteStartElement(prefix.c_str(), name.c_str(), nameSpace.c_str());
    return ToErrorCode(hr, "WriteStartElement", true, false);
}

ErrorCode ComProxyXmlLiteWriter::WriteElementWithContent(std::string const & name, std::string const content,
    std::string const & prefix, std::string const & nameSpace)
{
    ErrorCode error = WriteStartElement(name, prefix, nameSpace);
    if (!error.IsSuccess())
    {
        return error;
    }
    error = WriteString(content);
    if (!error.IsSuccess())
    {
        return error;
    }
    return WriteEndElement();
}

ErrorCode ComProxyXmlLiteWriter::WriteEndDocument()
{
    HRESULT hr = writer_->WriteEndDocument();
    return ToErrorCode(hr, "WriteEndDocument", true, false);
}

ErrorCode ComProxyXmlLiteWriter::WriteEndElement()
{
    HRESULT hr = writer_->WriteEndElement();
    return ToErrorCode(hr, "WriteEndElement", true, false);
}

ErrorCode ComProxyXmlLiteWriter::Close()
{
    writer_->WriteEndDocument();
    return Flush();
}


ErrorCode ComProxyXmlLiteWriter::ToErrorCode(
    HRESULT hr, 
    string const & operationName,
    bool sOkIsSuccess,
    bool sFalseIsSuccess)
{
    return ToErrorCode(hr, operationName, outputName_, sOkIsSuccess, sFalseIsSuccess);
}

ErrorCode ComProxyXmlLiteWriter::ToErrorCode(
    HRESULT hr, 
    string const & operationName,
    string const & outputName,
    bool sOkIsSuccess,
    bool sFalseIsSuccess)
{
    if (hr == S_OK && !sOkIsSuccess)
    {
        hr = E_FAIL;
    }
    else if (hr == S_FALSE && !sFalseIsSuccess)
    {
        hr = E_FAIL;
    }

    if (FAILED(hr))
    {
         WriteError(
            TraceType_XmlLiteWriter,
            "{0} failed. HRESULT={1}, Output={2}",
            operationName,
            hr,
            outputName);
    }

    return ErrorCode::FromHResult(hr);
}


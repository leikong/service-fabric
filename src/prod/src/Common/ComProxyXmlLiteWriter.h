// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once 

namespace Common
{
    class ComProxyXmlLiteWriter;
    typedef std::unique_ptr<ComProxyXmlLiteWriter> ComProxyXmlLiteWriterUPtr;

    // wraps methods of XmlLite IXmlWriter
    class ComProxyXmlLiteWriter :
        TextTraceComponent<TraceTaskCodes::Common>
    {
        DENY_COPY(ComProxyXmlLiteWriter)

    public:
        ComProxyXmlLiteWriter(ComPointer<IXmlWriter> writer);
        ~ComProxyXmlLiteWriter();

        static ErrorCode Create(
                __in std::string const & outputName,
                __out ComProxyXmlLiteWriterUPtr & xmlLiteWriter, 
                bool writeByteOrderMark = true, 
                bool indent = true);

        static ErrorCode Create(
                ComPointer<IUnknown> const& output,
                __out ComProxyXmlLiteWriterUPtr & xmlLiteWriter, 
                bool writeByteOrderMark = true, 
                bool indent = true);

        ErrorCode SetOutput(std::string const & outputName, ComPointer<IUnknown> const & output);

        ErrorCode WriteNode(ComPointer<IXmlReader> const& reader, bool writeDefaultAttributes = false);
        ErrorCode Flush();

        //Writes XML version and if the document is  standalone or otherwise
        //@param XmlStandalone - if the given document is a standalone XML document
        //       or if it belongs to another document, default value is Omit this part
        //       of this delcaration
        ErrorCode WriteStartDocument(XmlStandalone value = XmlStandalone_Omit);

        //Gives the the current element the given attribute name, value pair.
        //@param std::string & attrName, the name of the attribute
        //@param std::string & attrValue, the value of the attribute
        //@param std::string & prefix, the prefix of the attribute (default is empty string)
        //@param std::string & nameSpace, the namespace for the attribute (default is empty string)
        ErrorCode WriteAttribute(std::string const & attrName, std::string const & value,
            std::string const & prefix = "", std::string const & nameSpace = "");
        
        //Writes a valid comment at the current head of the writer 
        //@param std::string & comment - the comment to be written
        ErrorCode WriteComment(std::string const & comment);

        //Writes the document type with the describing parameters
        ErrorCode WriteDocType(std::string const & name, std::string const & pubid, std::string const & sysid,
            std::string const & subset);

        //Writes the given c-style string as content to the current element
        //@param char* the pointer the first character of the null-terminated string
        ErrorCode WriteChars(char const* charPtr);

        //Writes the given wchar as content to the current element
        //@param std::string content - the content to write to the string
        ErrorCode WriteString(std::string const & content);

        //Creates a new element as a child of the current element with the given parameters
        ErrorCode WriteStartElement(std::string const & name, std::string const & prefix = "",
            std::string const & nameSpace = "");

        //Creats a new element with the given parameters with the given string as content
        ErrorCode WriteElementWithContent(std::string const & name, std::string const content,
            std::string const & prefix = "", std::string const & nameSpace = "");

        //Ends all the current tags up to and including the root element
        ErrorCode WriteEndDocument();

        //Writes the end tag for the current element
        ErrorCode WriteEndElement();

        //Ends the document and flushes
        ErrorCode Close();

        __declspec (property(get=get_OutputName)) std::string const & OutputName;
        std::string const & get_OutputName() const { return this->outputName_; }

    private:
        ComPointer<IXmlWriter> writer_;
        std::string outputName_;

        ErrorCode ToErrorCode(HRESULT hr, std::string const & operationName, bool sOkIsSuccess = "true", bool sFalseIsSuccess = "true");
        static ErrorCode ToErrorCode(HRESULT hr, std::string const & operationName, std::string const & outputName, bool sOkIsSuccess = "true", bool sFalseIsSuccess = "true");
    };
}

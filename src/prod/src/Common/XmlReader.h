// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once 

namespace Common
{
    class XmlReader;
    typedef std::unique_ptr<XmlReader> XmlReaderUPtr;

    // XmlReader built on top of XmlLite based reader that provides methods 
    // similar to .NET XmlReader to allow easier de-serialization of XML.
    // 
    // All instance methods of this type throws XmlException if reader 
    // fails. Use of exception allows writing straightforward deserialization 
    // code. For example, see Parser.cpp in ServiceModel.
    class XmlReader :
        TextTraceComponent<TraceTaskCodes::Common>
    {
        DENY_COPY(XmlReader)

    public:
        static ErrorCode Create(std::string const & fileName, __out XmlReaderUPtr & reader);
        static Common::ErrorCode ReadXmlFile(std::string const & fileName, std::string const & rootElementName,  std::string const & namespaceUri, __out std::string & xmlContent);

        XmlReader(ComProxyXmlLiteReaderUPtr & xmlLiteReader);
        ~XmlReader();

        // skips non-content nodes. checks if the first content node is an element with the specified name and namespace
        // use this method to parse optional elements
        // positions the reader on the start element tag that allows reading through the attributes
        bool IsStartElement(std::string const & localName, std::string const & namespaceUri, bool readAttrs = true);

        // skips non-content nodes and ensures that the first content node is an element with the specified name and namespace
        // use this method to ensure required elements are present in the XML
        // positions the reader on the start element tag that allows reading through the attributes
        void StartElement(std::string const & localName, std::string const & namespaceUri, bool readAttrs = true);

        // reads the start element tag including all of the attributes and positions the reader on the next node
        // the reader must be positioned on the start element. This method will skip all of the attributes. To read
        // the attribute use StartElement method and then call attribute methods
        void ReadStartElement();

        // moves the reader to first non-content node and ensures that it is an end element tag and consumes it.
        // positions the reader on the node after the end element tag
        void ReadEndElement();

        // ensures that the reader is positioned on the start element tag and then reads both start and end element tags
        // use thid method to read through the elements that have no content (takes care of the element without end tags)
        // positions the reader on the node after the end element tag
        void ReadElement();


        // ensures that the reader is positioned on the start element tag and then reads start and 
        // positions the reader on the node after the element tag (child element)
        void MoveToNextElement();

        // moves the reader to the next non-content node and ensures that it is an element node with the specified
        // name and namespace (if not optional). reads through the entire XML under that element and the end element tag. 
        // positions the reader on the next node after the end element tag
        // use this method to skip through the elements that you do not care in the de-serialization
        void SkipElement(std::string const & localName, std::string const & namespaceUri, bool isOptional = false);
        
        // returns true if the reader is positioned on the start element and the element is empty (no end tag)
        bool IsEmptyElement();

        // return true if the reader has specified attribute
        // call this method after positioning the reader on the start element using StartElement or successful IsStartElement method
        bool HasAttribute(std::string const & attrName, std::string const & namespaceUri = "");

        // returns the value of the specified attribute, throws an error if the attribute is not found
        // call this method after positioning the reader on the start element using StartElement or successful IsStartElement method
        std::string ReadAttributeValue(std::string const & attrName, std::string const & namespaceUri = "", bool trim = false);

        // reads the content of the element in a string
        // reads through the content of the element and advances reader to the end element tag or next element tag
        std::string ReadElementValue(bool trim = false);

        // reads the content, including markup, representing this node and all its children.
        std::string ReadOuterXml();

        // returns the value of the specified attribute as the T, throws an error if the attribute is not found
        // call this method after positioning the reader on the start element using StartElement or successful IsStartElement method
        template <typename T>
        T ReadAttributeValueAs(std::string const & attrName, std::string const & namespaceUri = "")
        {
            T parsed;
            std::string attrValue = ReadAttributeValue(attrName, namespaceUri);
            if (!StringUtility::TryFromWString(attrValue, parsed))
            {
                ThrowInvalidContent(
                    StringUtility::ToWString(std::string(typeid(parsed).name())), 
                    attrValue);
            }

            return parsed;
        }

        /*
            reads a list of children where each child has a unique Name
            the factory must return both the name and the item
            
            Duplicate items throw an XmlException with XmlInvalidContent

            It assumes that the element has already been started and the map with the list of children is the last item
            
            Example: 

            <MyMap>
                <Item>
                    <Name>Hello</Name>
                    <Value>3</Value>
                </Item>
                <Item>
                    <Name>World</Name>
                    <Value>4</Value>
                </Item>
            </MyMap>

            Assume that the value is represented as 
                struct MyMapItem
                {
                    string Name;
                    int Value;
                };

            This function can be used to return a map<string, MyMapItem>.

            The parameters would be:
                - childNodeName: "Item"
                - factory: a function that can start at "Item" and return an instance of pair<string, MyMapItem> given the reader pointing at item                    

            The reader must have consumed MyMap.
        */
        template <typename TMap>
        TMap ReadMap(
            std::string const & childNodeName,
            std::string const & namespaceUri,
            std::function<typename TMap::value_type(Common::XmlReader & reader)> const & factory) 
        {
            TMap rv;

            if (IsEmptyElement())
            {
                ReadElement();
            }
            else
            {
                ReadStartElement();

                while (IsStartElement(childNodeName, namespaceUri, false))
                {
                    auto parameterPair = factory(*this);

                    auto insertResult = rv.insert(move(parameterPair));

                    if (!insertResult.second)
                    {
                        auto msg = Common::formatString(
                            "Duplicate element {0} detected at Input={1}, Line={2}, Position={3}",
                            insertResult.first->first,
                            FileName,
                            GetLineNumber(),
                            GetLinePosition());

                        throw XmlException(ErrorCode(ErrorCodeValue::XmlInvalidContent, move(msg)));
                    }
                }

                ReadEndElement();
            }

            return rv;
        }

        /*
            Same as above except the factory assumes that the name is defined on the .Name field of the child item
        */
        template <typename TMap>
        TMap ReadMapWithInlineName(
            std::string const & childNodeName,
            std::string const & namespaceUri,
            std::function<typename TMap::mapped_type(Common::XmlReader & reader)> const & factory) 
        {
            return ReadMap<TMap>(
                childNodeName,
                namespaceUri,
                [&factory](Common::XmlReader & reader)
                {
                    auto child = factory(reader);
                    auto name = child.Name;

                    typedef typename TMap::value_type pairtype;

                    return pairtype(std::move(name), std::move(child));
                });
        }

        UINT GetLineNumber();
        UINT GetLinePosition();

        __declspec (property(get=get_FileName)) std::string const & FileName;
        std::string const & get_FileName() const { return this->liteReader_->InputName; }

        __declspec (property(get=get_LiteReader)) ComProxyXmlLiteReaderUPtr const& LiteReader;
        ComProxyXmlLiteReaderUPtr const& get_LiteReader() const { return this->liteReader_; }

        ::XmlNodeType MoveToContent();

#if !defined(PLATFORM_UNIX)
    private:
#else
    public:
#endif
        void ReadContent(::XmlNodeType contentType);
        void ReadAttributes();
        std::string GetFullyQualifiedName();
        static std::string GetFullyQualifiedName(std::string const & namespaceUri, std::string const & localName);

        // wrappers for XmlLite Readers that throws
        UINT GetAttributeCount();
        UINT GetDepth();
        std::string GetLocalName();
        std::string GetNamespaceUri();
        ::XmlNodeType GetNodeType();
        std::string GetPrefix();
        std::string GetQualifiedName();
        std::string GetValue();
        void MoveToElement();
        bool MoveToFirstAttribute();
        bool MoveToNextAttribute();
        void Read();
        bool Read(__out ::XmlNodeType & nodeType);

        // throw utility methods
        void ThrowUnexpectedEOF(std::string const & operationName);
        void ThrowInvalidContent(XmlNodeType expectedContentType, XmlNodeType actualContentType);
        void ThrowInvalidContent(std::string const & expectedContent, std::string const & actualContent);
        void ThrowIfNotSuccess(ErrorCode const & error);
        void ThrowIfNotSuccess(ErrorCode const & error, std::string const& operationNAme);
        void ThrowIfNotSuccess(HRESULT hr, std::string const& operationNAme);
    private:
        ComProxyXmlLiteReaderUPtr liteReader_;
        std::map<std::string, std::string> attrValues_;
    };
}

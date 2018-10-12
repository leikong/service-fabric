// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace std;

namespace Common
{
    GlobalString NamingUri::NameUriScheme = make_global<std::string>("fabric");
    GlobalString NamingUri::RootAuthority = make_global<std::string>("");
    GlobalString NamingUri::RootNamingUriString = make_global<std::string>("fabric:");
    GlobalString NamingUri::InvalidRootNamingUriString = make_global<std::string>("fabric://");
    GlobalString NamingUri::NamingUriConcatenationReservedTokenDelimiter = make_global<std::string>("+");
    // All reserved token delimiters should be listed here.
    GlobalString NamingUri::ReservedTokenDelimiters = make_global<std::string>("$+|~");
    Global<NamingUri> NamingUri::RootNamingUri = make_global<NamingUri>("");
    GlobalString NamingUri::SegmentDelimiter = make_global<std::string>("/");
    GlobalString NamingUri::HierarchicalNameDelimiter = make_global<std::string>("~");

    NamingUri::NamingUri(
        std::string const & scheme,
        std::string const & authority,
        std::string const & path,
        std::string const & query,
        std::string const & fragment)
        : Uri(scheme, authority, path, query, fragment)
    {
        ASSERT_IFNOT(IsNamingUri(*this), "Invalid naming uri");
    }

    NamingUri NamingUri::GetAuthorityName() const
    {
        return Segments.empty() ? NamingUri() : NamingUri(Segments[0]);
    }

    NamingUri NamingUri::GetParentName() const
    {
        if (Path.empty())
        {
            return NamingUri::RootNamingUri;
        }

        size_t index = Path.find_last_of("/");
        ASSERT_IF(index == string::npos, "No / in path?");

        string truncPath = Path.substr(0, index);
        return NamingUri(truncPath);
    }

    NamingUri NamingUri::GetTrimQueryAndFragmentName() const
    {
        if (this->Query.empty() && this->Fragment.empty())
        {
            return *this;
        }
        else
        {
            return NamingUri(this->Path);
        }
    }

    size_t NamingUri::GetHash() const
    {
        return StringUtility::GetHash(this->ToString());
    }

    size_t NamingUri::GetTrimQueryAndFragmentHash() const
    {
        return StringUtility::GetHash(this->GetTrimQueryAndFragment().ToString());
    }

    bool NamingUri::IsPrefixOf(NamingUri const & other) const
    {
        return Uri::IsPrefixOf(other);
    }

    bool NamingUri::TryParse(std::string const & input, __out NamingUri & output)
    {
        if (StringUtility::StartsWith<std::string>(input, InvalidRootNamingUriString))
        {
            Trace.WriteWarning(
                Uri::TraceCategory,
                "NamingUri '{0}' is invalid: authorities are not supported.",
                input);
            return false;
        }

        NamingUri uri;
        if (Uri::TryParse(input, uri) && IsNamingUri(uri))
        {
            output = std::move(uri);
            return true;
        }
        return false;
    }

    bool NamingUri::TryParse(FABRIC_URI name, __out NamingUri & output)
    {
        if (name == NULL)
        {
            Trace.WriteWarning(Uri::TraceCategory, "Invalid NULL parameter: FABRIC_URI.");
            return false;
        }

        std::string tempName(name);
        return NamingUri::TryParse(tempName, /*out*/output);
    }

    HRESULT NamingUri::TryParse(
        FABRIC_URI name,
        std::string const & traceId,
        __out NamingUri & nameUri)
    {
        // Validate and parse the input name pointer
        if (name == NULL)
        {
            Trace.WriteWarning(Uri::TraceCategory, traceId, "Invalid NULL parameter: name.");
            return E_POINTER;
        }

        auto error = ParameterValidator::IsValid(
            name,
            ParameterValidator::MinStringSize,
            CommonConfig::GetConfig().MaxNamingUriLength);
        if (!error.IsSuccess())
        {
            Trace.WriteWarning(
                Uri::TraceCategory,
                traceId,
                "Input uri doesn't respect parameter size limits ({0}-{1}).",
                ParameterValidator::MinStringSize,
                CommonConfig::GetConfig().MaxNamingUriLength);
            return error.ToHResult();
        }

        std::string tempName(name);
        if (!NamingUri::TryParse(tempName, /*out*/nameUri))
        {
            Trace.WriteWarning(Uri::TraceCategory, traceId, "{0}: Input uri is not a valid naming uri.", tempName);
            return FABRIC_E_INVALID_NAME_URI;
        }

        return S_OK;
    }

    ErrorCode NamingUri::TryParse(
        FABRIC_URI name,
        StringLiteral const & parameterName,
        __inout NamingUri & nameUri)
    {
        // Validate and parse the input name pointer
        if (name == NULL)
        {
            ErrorCode innerError(ErrorCodeValue::ArgumentNull, formatString.L("{0} {1}.", GET_COMMON_RC(Invalid_Null_Pointer), parameterName));
            Trace.WriteWarning(Uri::TraceCategory, "NamingUri::TryParse: {0}: {1}", innerError, innerError.Message);
            return innerError;
        }

        auto error = ParameterValidator::IsValid(
            name,
            ParameterValidator::MinStringSize,
            CommonConfig::GetConfig().MaxNamingUriLength);
        if (!error.IsSuccess())
        {
            ErrorCode innerError(
                ErrorCodeValue::InvalidArgument,
                formatString.L("{0} {1}, {2}, {3}.", GET_COMMON_RC(Invalid_LPCSTR_Length), parameterName, ParameterValidator::MinStringSize, CommonConfig::GetConfig().MaxNamingUriLength));
            Trace.WriteWarning(Uri::TraceCategory, "NamingUri::TryParse: {0}: {1}", innerError, innerError.Message);
            return innerError;
        }

        std::string tempName(name);
        if (!NamingUri::TryParse(tempName, /*out*/nameUri))
        {
            ErrorCode innerError(ErrorCodeValue::InvalidNameUri, formatString.L("{0} {1}.", GET_NAMING_RC(Invalid_Uri), tempName));
            Trace.WriteWarning(Uri::TraceCategory, "NamingUri::TryParse: {0}: {1}", innerError, innerError.Message);
            return innerError;
        }

        return ErrorCode::Success();
    }

    ErrorCode NamingUri::TryParse(
        std::string const & nameText,
        std::string const & traceId,
        __out NamingUri & nameUri)
    {
        auto error = ParameterValidator::IsValid(
            nameText.c_str(),
            ParameterValidator::MinStringSize,
            CommonConfig::GetConfig().MaxNamingUriLength);
        if (!error.IsSuccess())
        {
            ErrorCode innerError(ErrorCodeValue::InvalidNameUri, formatString(
                "{0} {1} {2}, {3}.", 
                GET_COMMON_RC(Invalid_LPCSTR_Length), 
                nameText, 
                ParameterValidator::MinStringSize, 
                CommonConfig::GetConfig().MaxNamingUriLength));
            Trace.WriteInfo(Uri::TraceCategory, traceId, "NamingUri::TryParse: {0}: {1}", innerError, innerError.Message);
            return innerError;
        }

        if (!TryParse(nameText, nameUri))
        {
            ErrorCode innerError(ErrorCodeValue::InvalidNameUri, formatString.L("{0} {1}.", GET_NAMING_RC(Invalid_Uri), nameText));
            Trace.WriteWarning(Uri::TraceCategory, traceId, "NamingUri::TryParse: {0}: {1}", innerError, innerError.Message);
            return innerError;
        }

        return ErrorCode::Success();
    }

    bool NamingUri::IsNamingUri(Uri const & question)
    {
        if (question.Scheme != NameUriScheme)
        {
            Trace.WriteWarning(
                Uri::TraceCategory,
                "NamingUri '{0}' has invalid scheme: supported scheme is 'fabric:'.",
                question);
            return false;
        }

        if ((question.Type != UriType::Absolute) && (question.Type != UriType::Empty))
        {
            Trace.WriteWarning(
                Uri::TraceCategory,
                "NamingUri '{0}' is invalid: 'fabric:' scheme requires absolute path.",
                question);
            return false;
        }

        // Disallow empty segments
        if (StringUtility::Contains<string>(question.Path, "//"))
        {
            Trace.WriteWarning(
                Uri::TraceCategory,
                "NamingUri '{0}' is invalid: empty segments in path are not supported.",
                question);
            return false;
        }

        // List of reserved characters
        for (auto delimiterIt = ReservedTokenDelimiters->begin(); delimiterIt != ReservedTokenDelimiters->end(); ++delimiterIt)
        {
            string delimiter = formatString.L("{0}", *delimiterIt);
            if (StringUtility::Contains<string>(question.Path, delimiter))
            {
                Trace.WriteWarning(
                    Uri::TraceCategory,
                    "NamingUri '{0}' is invalid: '{1}' is a reserved character.",
                    question,
                    delimiter);
                return false;
            }
        }

        // Disallow trailing slash
        if ((question.Path.size() > 0) && (question.Path[question.Path.size() - 1] == '/'))
        {
            Trace.WriteWarning(
                Uri::TraceCategory,
                "NamingUri '{0}' is invalid: trailing '/' is not supported.",
                question);
            return false;
        }

        return true;
    }

    ErrorCode NamingUri::ValidateName(NamingUri const & uri, std::string const & uriString, bool allowFragment)
    {
        if (uri == NamingUri::RootNamingUri)
        {
            return ErrorCode(
                ErrorCodeValue::AccessDenied,
                formatString(GET_COMMON_RC(InvalidOperationOnRootNameURI), uriString));
        }

        if (uriString.size() > static_cast<size_t>(CommonConfig::GetConfig().MaxNamingUriLength))
        {
            return ErrorCode(
                ErrorCodeValue::InvalidNameUri,
                formatString(GET_COMMON_RC(InvalidNameExceedsMaxSize), uriString, uriString.size(), CommonConfig::GetConfig().MaxNamingUriLength));
        }

        if (!uri.Query.empty())
        {
            return ErrorCode(
                ErrorCodeValue::InvalidNameUri,
                formatString(GET_COMMON_RC(InvalidNameQueryCharacter), uriString));
        }

        if (allowFragment && !uri.Fragment.empty())
        {
            return ErrorCode(
                ErrorCodeValue::InvalidNameUri,
                formatString(GET_COMMON_RC(InvalidNameServiceGroupCharacter), uriString));
        }

        return ErrorCode::Success();
    }

    bool NamingUri::TryCombine(std::string const & path, __out NamingUri & result) const
    {
        string temp(this->ToString());

        if (!path.empty())
        {
            temp.append("/");
            temp.append(path);
        }

        NamingUri uri;
        if (TryParse(temp, uri))
        {
            result = move(uri);
            return true;
        }

        return false;
    }

    NamingUri NamingUri::Combine(NamingUri const & name, string const & path)
    {
        NamingUri result;
        if (!name.TryCombine(path, result))
        {
            Assert::CodingError("could not combine {0} and {1} into a Naming Uri", name, path);
        }

        return result;
    }

    ErrorCode NamingUri::FabricNameToId(std::string const & name, __inout std::string &escapedId)
    {
        std::string temp = name;
        StringUtility::TrimLeading<std::string>(temp, NamingUri::RootNamingUriString);
        StringUtility::TrimLeading<std::string>(temp, NamingUri::SegmentDelimiter);

        return EscapeString(temp, escapedId);
    }

    ErrorCode NamingUri::FabricNameToId(std::string const & name, bool useDelimiter, __inout std::string &escapedId)
    {
        std::string temp = name;
        std::string tempId;
        NamingUri::FabricNameToId(temp, tempId);
        if (useDelimiter)
        {
            StringUtility::Replace<std::string>(tempId, NamingUri::SegmentDelimiter, NamingUri::HierarchicalNameDelimiter);
        }

        return EscapeString(tempId, escapedId);
    }

    ErrorCode NamingUri::IdToFabricName(std::string const& scheme, std::string const& id, __inout std::string &name)
    {
        ASSERT_IF(scheme.empty(), "IdToFabricName - scheme cannot be empty");

        std::string escapedId;
        auto error = UnescapeString(id, escapedId);
        if (!error.IsSuccess())
        {
            return error;
        }

        StringUtility::Replace<std::string>(escapedId, NamingUri::HierarchicalNameDelimiter, NamingUri::SegmentDelimiter);
        name = formatString.L("{0}{1}{2}", scheme, *NamingUri::SegmentDelimiter, escapedId);
        return ErrorCode::Success();
    }

    void NamingUri::ParseHost(std::string & input, std::string & host, std::string & remain)
    {
        input = input.substr(1, std::string::npos);
        std::size_t nextFound = remain.find("/");
        if (nextFound == std::string::npos)
        {
            nextFound = remain.find("?");
            if (nextFound == std::string::npos)
            {
                nextFound = remain.find("#");
                if (nextFound == std::string::npos)
                {
                    nextFound = input.length();
                }
            }
        }

        host = "/";
        host = host + input.substr(0, nextFound);
        remain = input.substr(nextFound, std::string::npos);
    }

    void NamingUri::ParsePath(std::string & input, std::string & path, std::string & remain)
    {
        std::size_t nextFound = remain.find("?");
        if (nextFound == std::string::npos)
        {
            nextFound = remain.find("#");
            if (nextFound == std::string::npos)
            {
                nextFound = input.length();
            }
        }

        path = input.substr(0, nextFound);
        remain = input.substr(nextFound, std::string::npos);
    }

    ErrorCode NamingUri::ParseUnsafeUri(std::string const & input, std::string & protocol, std::string & host, std::string & path, std::string & queryFragment)
    {
        std::size_t protocolFound = input.find(":/");
        std::string remain;
        if (protocolFound != std::string::npos)
        {
            protocol = input.substr(0, protocolFound + 2);
            remain = input.substr(protocolFound + 2, std::string::npos);
        }
        else
        {
            remain = input;
        }

        // Has host
        if (remain.length() > 0 && *(remain.begin()) == '/')
        {
            ParseHost(remain, host, remain);
        }

        ParsePath(remain, path, remain);
        queryFragment = remain;

        return ErrorCodeValue::Success;
    }

    ErrorCode NamingUri::EscapeString(std::string const & input, __inout std::string &output)
    {
#if defined(PLATFORM_UNIX)
        std::string protocol;
        std::string host;
        std::string path;
        std::string queryFragment;
        ErrorCode result = ParseUnsafeUri(input, protocol, host, path, queryFragment);

        std::string pathString;
        Utf16ToUtf8NotNeeded2(path, pathString);
        static const char lookup[] = "0123456789ABCDEF";
        std::ostringstream out;
        for (std::string::size_type i = 0; i < pathString.length(); i++) {
            const char& c = pathString.at(i);
            if (c == '[' ||
                c == ']' ||
                c == '&' ||
                c == '^' ||
                c == '`' ||
                c == '{' ||
                c == '}' ||
                c == '|' ||
                c == '"' ||
                c == '<' ||
                c == '>' ||
                c == '\\' ||
                c == ' ')
            {
                out << '%';
                out << lookup[(c & 0xF0) >> 4];
                out << lookup[(c & 0x0F)];
            }
            else
            {
                out << c;
            }
        }
        std::string pathEscape;
        Utf8ToUtf16NotNeeded2(out.str(), pathEscape);
        output = protocol + host + pathEscape + queryFragment;
#else
        DWORD size = 1;
        std::vector<CHAR> buffer;
        buffer.resize(size);

        HRESULT hr = UrlEscape(const_cast<CHAR*>(input.data()), buffer.data(), &size, URL_ESCAPE_AS_UTF8);
        if (FAILED(hr) && hr == E_POINTER)
        {
            buffer.resize(size);
            hr = UrlEscape(const_cast<CHAR*>(input.data()), buffer.data(), &size, URL_ESCAPE_AS_UTF8);
            if (FAILED(hr))
            {
                return ErrorCode::FromHResult(hr);
            }
            output = buffer.data();
        }
        else
        {
            return ErrorCode::FromHResult(hr);
        }
#endif

        return ErrorCodeValue::Success;
    }

    ErrorCode NamingUri::UnescapeString(std::string const& input, __inout std::string& output)
    {
#if defined(PLATFORM_UNIX)
        std::string protocol;
        std::string host;
        std::string path;
        std::string queryFragment;
        ErrorCode result = ParseUnsafeUri(input, protocol, host, path, queryFragment);

        std::ostringstream out;
        std::string pathString;
        Utf16ToUtf8NotNeeded2(path, pathString);
        for (std::string::size_type i = 0; i < pathString.length(); i++) {
            if (pathString.at(i) == '%') {
                std::string temp(pathString.substr(i + 1, 2));
                std::istringstream in(temp);
                short c = 0;
                in >> std::hex >> c;
                out << static_cast<unsigned char>(c);
                i += 2;
            }
            else {
                out << pathString.at(i);
            }
        }
        std::string pathUnescape;
        Utf8ToUtf16NotNeeded2(out.str(), pathUnescape);
        output = protocol + host + pathUnescape + queryFragment;
#else
        DWORD size = 1;
        std::vector<CHAR> buffer;
        buffer.resize(size);

        HRESULT hr = UrlUnescape(const_cast<CHAR*>(input.data()), buffer.data(), &size, URL_UNESCAPE_AS_UTF8);
        if (FAILED(hr) && hr == E_POINTER)
        {
            buffer.resize(size);
            hr = UrlUnescape(const_cast<CHAR*>(input.data()), buffer.data(), &size, URL_UNESCAPE_AS_UTF8);
            if (FAILED(hr))
            {
                return ErrorCodeValue::InvalidNameUri;
            }
            output = buffer.data();
        }
        else
        {
            return ErrorCodeValue::InvalidNameUri;
        }
#endif

        return ErrorCode::Success();
    }

    ErrorCode NamingUri::UrlEscapeString(std::string const & input, std::string & output)
    {
        // Clear the output because we are appending to it.
        output.clear();

        string basicEncodedStr;

        // First call escape string
        auto error = NamingUri::EscapeString(input, basicEncodedStr);

        // If escape returns an error, then return that error
        if (!error.IsSuccess())
        {
            return error;
        }

        // Replace chars with their encodings by appending
        // either the original char or its replacement to the output string
        // comma(",")
        // colon(":")
        // dollar("$")
        // plus sign("+")
        // semi - colon(";")
        // equals("=")
        // 'At' symbol("@")
        //
        // ampersand ("&")
        // forward slash("/")
        // question mark("?")
        // pound("#")
        // less than and greater than("<>")
        // open and close brackets("[]")
        // open and close braces("{}")
        // pipe("|")
        // backslash("\")
        // caret("^")
        // space(" ")
        // percent ("%") is not encoded here because it cannot be encoded twice.
        for (char c : basicEncodedStr)
        {
            if (c == ',')          // ,
            {
                output += "%2C";
            }
            else if (c == ':')     // :
            {
                output += "%3A";
            }
            else if (c == '$')     // $
            {
                output += "%24";
            }
            else if (c == '+')     // +
            {
                output += "%2B";
            }
            else if (c == ';')     // ;
            {
                output += "%3B";
            }
            else if (c == '=')     // =
            {
                output += "%3D";
            }
            else if (c == '@')     // @
            {
                output += "%40";
            } // The following may never be called since EscapeString may have already taken care of them (OS dependent)
            else if (c == '&')     // &
            {
                output += "%26";
            }
            else if (c == '/')     // / (forward slash)
            {
                output += "%2F";
            }
            else if (c == '\\')     // back slash
            {
                output += "%5C";
            }
            else if (c == '?')     // ?
            {
                output += "%3F";
            }
            else if (c == '#')     // #
            {
                output += "%23";
            }
            else if (c == '<')     // <
            {
                output += "%3C";
            }
            else if (c == '>')     // >
            {
                output += "%3E";
            }
            else if (c == '[')     // [
            {
                output += "%5B";
            }
            else if (c == ']')     // ]
            {
                output += "%5D";
            }
            else if (c == '{')     // {
            {
                output += "%7B";
            }
            else if (c == '}')     // }
            {
                output += "%7D";
            }
            else if (c == '|')     // |
            {
                output += "%7C";
            }            
            else if (c == '^')     // ^
            {
                output += "%5E";
            }
            else if (c == ' ')     // space
            {
                output += "%20";
            }
            else
            {
                output += c;
            }
        }

        return ErrorCode::Success();
    }
}

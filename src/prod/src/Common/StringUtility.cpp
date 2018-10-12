// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"
#include <codecvt>
#include <pdhmsg.h>
#include <regex>

using namespace std;

namespace Common
{
    void StringUtility::ToUpper(string & str)
    {
        transform(str.begin(), str.end(), str.begin(), ToUpperChar);
    }

    void StringUtility::ToLower(string & str)
    {
        transform(str.begin(), str.end(), str.begin(), ToLowerChar);
    }

    void StringUtility::TrimSpaces(string & str)
    {
        static const string space(" ");
        Trim<string>(str, space);
    }

    void StringUtility::TrimWhitespaces(string & str)
    {
        static const string whitespaces(" \t\r\n");
        Trim<string>(str, whitespaces);
    }

    void StringUtility::GenerateRandomString(int length, string & randomString)
    {
        string allowedChars =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        size_t numberOfPossibilities = allowedChars.length();
        for (int i = 0; i < length; ++i)
        {
            randomString += allowedChars[Random().Next() % numberOfPossibilities];
        }
    }

// TODO: This conversion code has been found duplicated in at least two places:
//       ktlpal.cpp and here
//
using lchar_t = wchar_t;

    void StringUtility::Utf16ToUtf8(std::wstring const & utf16String, std::string & utf8String)
    {
#ifdef PLATFORM_UNIX
        wstring_convert<codecvt_utf8_utf16<lchar_t>, lchar_t> convertor;
        utf8String = convertor.to_bytes((const lchar_t *)utf16String.c_str());
#else
        wstring_convert<codecvt_utf8_utf16<unsigned short>, unsigned short> convertor;
        utf8String = convertor.to_bytes((const unsigned short *)utf16String.c_str());
#endif
    }

    string StringUtility::Utf16ToUtf8(std::wstring const & utf16String)
    {
        string utf8String;
        Utf16ToUtf8(utf16String, utf8String);
        return utf8String;
    }

    void StringUtility::Utf8ToUtf16(std::string const & utf8String, std::wstring & utf16String)
    {
#ifdef PLATFORM_UNIX
        wstring_convert<codecvt_utf8_utf16<lchar_t>, lchar_t> convertor;
#else
        wstring_convert<codecvt_utf8_utf16<unsigned short>, unsigned short> convertor;
#endif
        utf16String.clear();
        for (auto const & ch : convertor.from_bytes(utf8String))
        {
            utf16String.push_back(ch);
        }
    }

    wstring StringUtility::Utf8ToUtf16(std::string const & utf8String)
    {
        wstring utf16String;
        Utf8ToUtf16(utf8String, utf16String);
        return utf16String;
    }

    HRESULT StringUtility::LpcwstrToWstring(LPCSTR ptr, bool acceptNull, __inout std::string & output)
    {
        return LpcwstrToWstring2(ptr, acceptNull, output).ToHResult();
    }

    HRESULT StringUtility::LpcwstrToWstring(LPCSTR ptr, bool acceptNull, size_t minSize, size_t maxSize, __inout std::string & output)
    {
        return LpcwstrToWstring2(ptr, acceptNull, minSize, maxSize, output).ToHResult();
    }

    ErrorCode StringUtility::LpcwstrToWstring2(LPCSTR ptr, bool acceptNull, __inout std::string & output)
    {
        size_t minSize = acceptNull ? 0 : ParameterValidator::MinStringSize;
        return StringUtility::LpcwstrToWstring2(ptr, acceptNull, minSize, ParameterValidator::MaxStringSize, output);
    }

    ErrorCode StringUtility::LpcwstrToWstring2(LPCSTR ptr, bool acceptNull, size_t minSize, size_t maxSize, __inout std::string & output)
    {
        if (ptr == nullptr)
        {
            if (acceptNull)
            {
                output = "";
                return ErrorCodeValue::Success;
            }
            else
            {
                return ErrorCodeValue::ArgumentNull;
            }
        }
        else
        {
            auto error = ParameterValidator::IsValid(ptr, minSize, maxSize);
            if (error.IsSuccess())
            {
                output = string(ptr);
            }

            return error;
        }
    }

    ErrorCode StringUtility::LpcwstrToTruncatedWstring(LPCSTR ptr, bool acceptNull, size_t minSize, size_t maxSize, __inout std::string & output)
    {
        if (ptr == nullptr)
        {
            if (acceptNull)
            {
                output = "";
                return ErrorCodeValue::Success;
            }
            else
            {
                return ErrorCodeValue::ArgumentNull;
            }
        }
        else
        {
            size_t length = 0;
            auto error = ParameterValidator::IsValidTruncated(ptr, minSize, length);
            if (!error.IsSuccess())
            {
                return error;
            }

            if (length <= maxSize)
            {
                output = string(ptr);
            }
            else
            {
                // Need to truncate the string as it's longer than allowed size.
                output = string(ptr, maxSize);
                ReplaceEndWithTruncateMarker(output);
            }

            return ErrorCode::Success();
        }
    }

    bool StringUtility::TruncateIfNeeded(__in std::string & s, size_t maxSize)
    {
        if (s.size() <= maxSize)
        {
            return false;
        }

        s.resize(maxSize);
        ReplaceEndWithTruncateMarker(s);
        return true;
    }
    
    bool StringUtility::TruncateStartIfNeeded(__in std::string & s, size_t maxSize)
    {
        if (s.size() <= maxSize)
        {
            return false;
        }

        s.erase(0, s.size() - maxSize);
        ReplaceStartWithTruncateMarker(s);
        return true;
    }

    std::string StringUtility::LimitNumberOfCharacters(__in const std::string &s, char token, size_t maxSize)
    {
        size_t position = 0;
        size_t numberOfOccurences = 0;

        for (; position < s.size(); ++position)
        {
            if (s[position] == token)
            {
                ++numberOfOccurences;
                if (numberOfOccurences > maxSize)
                {
                    break;
                }
            }
        }

        return s.substr(0, position);
    }

    void StringUtility::ReplaceEndWithTruncateMarker(__in std::string & s)
    {
        // Replace last characters with a marker so users know the string was truncated
        static string TruncateMarker(" [Truncated]");

        size_t size = s.size();
        ASSERT_IF(size <= TruncateMarker.size(), "Can't replace last characters in string with truncated marker as maxSize is too small: {0}", size);
        size_t replaceStart = size - TruncateMarker.size();
        s.replace(replaceStart, TruncateMarker.size(), TruncateMarker);
        
#ifdef DBG
        ASSERT_IFNOT(s.size() == size, "ReplaceEndWithTruncateMarker: initial size should be same for truncated string {0}", s);
#endif
    }
    
    void StringUtility::ReplaceStartWithTruncateMarker(__in std::string & s)
    {
        // Replace first characters with a marker so users know the string was truncated
        static string TruncateMarker(" [Truncated]");

        size_t size = s.size();
        ASSERT_IF(size <= TruncateMarker.size(), "Can't replace first characters in string with truncated marker as maxSize is too small: {0}", size);
        s.replace(0, TruncateMarker.size(), TruncateMarker);

#ifdef DBG
        ASSERT_IFNOT(s.size() == size, "ReplaceStartWithTruncateMarker: initial size should be same for truncated string {0}", s);
#endif
    }

    HRESULT StringUtility::LpcwstrToFilePath(LPCSTR ptr, __out std::string & path)
    {
        // Allow empty paths for current directory
        return StringUtility::LpcwstrToWstring2(ptr, true, 0, ParameterValidator::MaxFilePathSize, path).ToHResult();
    }

    HRESULT StringUtility::FromLPCSTRArray(ULONG count, LPCSTR * items, __out std::vector<std::string> & output)
    {
        ASSERT_IF(((count > 0) && (items == NULL)), "The items must not be null.");

        std::vector<std::string> temp;
        for (ULONG i = 0; i < count; ++i)
        {
            string result;
            auto error = StringUtility::LpcwstrToWstring2(items[i], true /*acceptNull*/, 0 /*minStringSize*/, ParameterValidator::MaxStringSize, result);
            if (!error.IsSuccess())
            {
                return error.ToHResult();
            }

            temp.push_back(move(result));
        }

        output.swap(temp);
        return ErrorCodeValue::Success;
    }

    bool StringUtility::AreStringCollectionsEqualCaseInsensitive(vector<string> const & v1, vector<string> const & v2)
    {
        if (v1.size() != v2.size())
        {
            return false;
        }

        for (size_t ix = 0; ix < v1.size(); ++ix)
        {
            if (!StringUtility::AreEqualCaseInsensitive(v1[ix], v2[ix]))
            {
                return false;
            }
        }

        return true;
    }

    size_t StringUtility::GetHash(string const & value)
    {
        // Code copied from <cxcache>Microsoft.WindowsAzure.EngSys.External.Windows.sdk.9.1.0\inc\crt\stl100\locale
        // This is to make hash computation same for both windows and linux
        uint val = 0;
        for (int i = 0 ; i < value.size() - 1; ++i)
        {
           val = (val << 8 | val >> 24) + value[i];
        }

        return static_cast<size_t>((int)(val));
    }

    bool StringUtility::StartsWith(std::string const & bigStr, StringLiteral const & smallStr)
    {
        auto bigStrPrefix = bigStr.substr(0, smallStr.size());
        return bigStrPrefix == smallStr;
    }

    bool operator==(StringLiteral const & lhs, StringLiteral const & rhs)
    {
        if (lhs.size() != rhs.size())
        {
            return false;
        }

        return !memcmp(lhs.begin(), rhs.begin(), lhs.size());
    }

    bool operator==(std::string const & lhs, StringLiteral const & rhs)
    {
        return (rhs == lhs);
    }

    bool operator!=(StringLiteral const & lhs, std::string const & rhs)
    {
        return !(lhs == rhs);
    }

    bool operator!=(std::string const & lhs, StringLiteral const & rhs)
    {
        return !(rhs == lhs);
    }

    bool operator!=(StringLiteral const & lhs, StringLiteral const & rhs)
    {
        return !(lhs == rhs);
    }

    bool operator==(StringLiteral const & lhs, std::string const & rhs)
    {
        if (lhs.size() != rhs.size())
        {
            return false;
        }

        return !memcmp(lhs.begin(), rhs.c_str(), lhs.size() << 1);
    }

    bool operator==(GlobalString const & lhs, GlobalString const & rhs)
    {
        return (*lhs == *rhs);
    }

    bool operator!=(GlobalString const & lhs, GlobalString const & rhs)
    {
        return (*lhs != *rhs);
    }

    bool operator==(GlobalString const & lhs, std::string const & rhs)
    {
        return (*lhs == rhs);
    }

    bool operator==(std::string const & lhs, GlobalString const & rhs)
    {
        return (lhs == *rhs);
    }

    bool operator!=(GlobalString const & lhs, std::string const & rhs)
    {
        return (*lhs != rhs);
    }

    bool operator!=(std::string const & lhs, GlobalString const & rhs)
    {
        return (lhs != *rhs);
    }
} // end namespace 

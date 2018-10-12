// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

#define JSON_E_MISSING_CLOSING_QUOTE                _HRESULT_TYPEDEF_(0x803F0001)
#define JSON_E_NOT_FIELDNAME_TOKEN                  _HRESULT_TYPEDEF_(0x803F0002)
#define JSON_E_INVALID_PARAMETER                    _HRESULT_TYPEDEF_(0x803F0003)
#define JSON_E_NUMBER_TOO_LONG                      _HRESULT_TYPEDEF_(0x803F0004)
#define JSON_E_BUFFER_TOO_SMALL                     _HRESULT_TYPEDEF_(0x803F0005)
#define JSON_E_MISSING_NAME_SEPARATOR               _HRESULT_TYPEDEF_(0x803F0006)
#define JSON_E_UNEXPECTED_TOKEN                     _HRESULT_TYPEDEF_(0x803F0007)
#define JSON_E_UNEXPECTED_END_ARRAY                 _HRESULT_TYPEDEF_(0x803F0008)
#define JSON_E_UNEXPECTED_END_OBJECT                _HRESULT_TYPEDEF_(0x803F0009)
#define JSON_E_INVALID_TOKEN                        _HRESULT_TYPEDEF_(0x803F000A)
#define JSON_E_UNEXPECTED_NAME_SEPARATOR            _HRESULT_TYPEDEF_(0x803F000B)
#define JSON_E_UNEXPECTED_VALUE_SEPARATOR           _HRESULT_TYPEDEF_(0x803F000C)
#define JSON_E_MISSING_END_OBJECT                   _HRESULT_TYPEDEF_(0x803F000D)
#define JSON_E_MISSING_END_ARRAY                    _HRESULT_TYPEDEF_(0x803F000E)
#define JSON_E_NOT_STRING_TOKEN                     _HRESULT_TYPEDEF_(0x803F000F)
#define JSON_E_MAX_NESTING_EXCEEDED                 _HRESULT_TYPEDEF_(0x803F0010)
#define JSON_E_UNEXPECTED_ESCAPE_CHARACTER          _HRESULT_TYPEDEF_(0x803F0011)
#define JSON_E_INVALID_ESCAPED_CHARACTER            _HRESULT_TYPEDEF_(0x803F0012)
#define JSON_E_INVALID_UNICODE_ESCAPE               _HRESULT_TYPEDEF_(0x803F0013)
#define JSON_E_INVALID_STRING_CHARACTER             _HRESULT_TYPEDEF_(0x803F0014)
#define JSON_E_INVALID_NUMBER                       _HRESULT_TYPEDEF_(0x803F0015)
#define JSON_E_NOT_NUMBER_TOKEN                     _HRESULT_TYPEDEF_(0x803F0016)
#define JSON_E_NUMBER_OUT_OF_RANGE                  _HRESULT_TYPEDEF_(0x803F0017)

namespace Common
{
    //-------------------------------------------------------------------------------------------------------------------------------------------
    template<typename CharType>
    struct JsonBufferManagerTraits
    {
    };
    //-------------------------------------------------------------------------------------------------------------------------------------------
    template<typename CharType>
    class JsonBufferManager2
    {
        template<typename T>
        friend struct JsonBufferManagerTraits;
    public:
        JsonBufferManager2()
        {
            JsonBufferManager2(nullptr, 0);
        }

        JsonBufferManager2(const CharType* pszBuffer, ULONG nLength)
            : m_pszCharBuffer(pszBuffer), m_pszCharBufferStart(pszBuffer), m_nBufferLength(nLength)
        {
        }

        // Returns the current character and advances to the next
        //
        char Read()
        {
            return JsonBufferManagerTraits<CharType>::GetNextChar(*this, &m_pszCharBuffer);
        }

        // Returns the current character. Does NOT advance to the next character.
        //
        char Peek() const
        {
            void *pContext = 0;
            return Peek(&pContext);
        }

        // ppContext MUST NOT be nullptr.
        // if *ppContext is nullptr, returns the current character and stores a value in *ppContext such
        // that subsequent calls to Peek with that same context will return consecutive characters
        //
        char Peek(void** ppContext) const
        {
            if(ppContext == nullptr) return 0;

            return PeekChar((const CharType**)ppContext);
        }

        void Skip(DWORD dwCharsToSkip)
        {
            // TODO: Fast path for UTF-16
            while(dwCharsToSkip)
            {
                if(!Read()) break; // GetChar returns 0 if we hit the end of the buffer
                dwCharsToSkip--;
            }
        }

    private:
        char GetNextUTF8Char(LPCSTR* ppszBuffer) const
        {
            // add an assert because caller does the argument check, and does not expect failure of function.
            //
            ATLASSERT(ppszBuffer);
            ATLASSERT(*ppszBuffer);

            BYTE b1 = **ppszBuffer;
            // Top bit is 0; Unicode Code Point is in the range 0x0000 to 0x007f; single byte character
            //
            if(b1 <= 0x7F)
            { 
                *ppszBuffer += 1;
                return b1;
            }

            // Top 3 bits are 110 ; Unicode Code Point is in the range 0x0080 to 0x07FF; two byte character
            // Check that b1 > 0xC1 to ensure that we don't have a code point in the 0x00-0x7F range encoded in two bytes
            //
            if((b1 & 0xE0) == 0xC0 && b1 > 0xC1 && !IsEOF(*ppszBuffer + 1))
            {
                BYTE b2 = *(*ppszBuffer + 1);
                if((b2 & 0xC0) == 0x80)
                {
                    *ppszBuffer += 2;
                    return ((b1 & 0x1F) << 6) | (b2 & 0x3F);
                }
            }

            // Top 4 bits are 1110; Unicode Code Point is in the range 0x0800 to 0xFFFF, excluding D800-DFFF; three byte character
            //
            if((b1 & 0xF0) == 0xE0 && !IsEOF(*ppszBuffer + 2))
            {
                BYTE b2 = *(*ppszBuffer + 1);
                BYTE b3 = *(*ppszBuffer + 2);

                // Top 2 bits of b2 and b3 are 10.
                // Check that either b1 > 0xE0 or b2 > 0x9F to ensure we don't have a code point in the 0x0000-0x07FF range encoded in three bytes
                //
                if((b2 & 0xC0) == 0x80 && (b3 & 0xC0) == 0x80 && (b1 > 0xE0 || b2 > 0x9F))
                {
                    char wc = ((b1 & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);

                    // If the top five bits are 11011, then the value is in the D800-DFFF range, which is not allowed, otherwise, we return the value
                    //
                    if((wc & 0xF800) != 0xD800)
                    {
                        *ppszBuffer += 3;
                        return wc;
                    }
                }
            }

            // We found a byte or sequence of bytes that we couldn't decode.
            //
            *ppszBuffer += 1;
            return 0;
        }

        char PeekChar(const CharType** ppszBuffer) const
        {
            if(ppszBuffer == nullptr || m_pszCharBuffer == nullptr) return 0; 

            if(*ppszBuffer == nullptr) 
            { 
                *ppszBuffer = m_pszCharBuffer; 
            }

            if(*ppszBuffer < m_pszCharBufferStart) return 0; 

            if(IsEOF(*ppszBuffer)) return 0; 

            return JsonBufferManagerTraits<CharType>::GetNextChar(*this, ppszBuffer);
        }

        static char GetNextUTF16Char(LPCSTR* ppszBuffer)
        {
            // add an assert because caller does the argument check, and does not expect failure of function.
            //
            //
            ATLASSERT(ppszBuffer);
            ATLASSERT(*ppszBuffer);

            char wc = *(*ppszBuffer)++;
            return (wc & 0xF800) == 0xD800 ? 0 : wc;
        }

        bool IsEOF(const CharType* pszStart) const
        { 
            return pszStart >= m_pszCharBufferStart + m_nBufferLength; 
        }

    private:
        const CharType*  m_pszCharBuffer;
        const CharType*  m_pszCharBufferStart;
        DWORD            m_nBufferLength;
    };
    //-------------------------------------------------------------------------------------------------------------------------------------------
    template<>
    struct JsonBufferManagerTraits<char>
    {
        static char GetNextChar(const JsonBufferManager2<char>& buffer, LPCSTR* ppszBuffer)
        {
            // add an assert because caller does the argument check, and does not expect failure of function.
            //
            ASSERT_IF(!ppszBuffer, "Invalid buffer");
            ASSERT_IF(!(*ppszBuffer), "Invalid buffer");

            BYTE b1 = **ppszBuffer;
            // Top bit is 0; Unicode Code Point is in the range 0x0000 to 0x007f; single byte character
            //
            if(b1 <= 0x7F)
            { 
                *ppszBuffer += 1;
                return b1;
            }

            // Top 3 bits are 110 ; Unicode Code Point is in the range 0x0080 to 0x07FF; two byte character
            // Check that b1 > 0xC1 to ensure that we don't have a code point in the 0x00-0x7F range encoded in two bytes
            //
            if((b1 & 0xE0) == 0xC0 && b1 > 0xC1 && !buffer.IsEOF(*ppszBuffer + 1))
            {
                BYTE b2 = *(*ppszBuffer + 1);
                if((b2 & 0xC0) == 0x80)
                {
                    *ppszBuffer += 2;
                    return ((b1 & 0x1F) << 6) | (b2 & 0x3F);
                }
            }

            // Top 4 bits are 1110; Unicode Code Point is in the range 0x0800 to 0xFFFF, excluding D800-DFFF; three byte character
            //
            if((b1 & 0xF0) == 0xE0 && !buffer.IsEOF(*ppszBuffer + 2))
            {
                BYTE b2 = *(*ppszBuffer + 1);
                BYTE b3 = *(*ppszBuffer + 2);

                // Top 2 bits of b2 and b3 are 10.
                // Check that either b1 > 0xE0 or b2 > 0x9F to ensure we don't have a code point in the 0x0000-0x07FF range encoded in three bytes
                //
                if((b2 & 0xC0) == 0x80 && (b3 & 0xC0) == 0x80 && (b1 > 0xE0 || b2 > 0x9F))
                {
                    char wc = ((b1 & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);

                    // If the top five bits are 11011, then the value is in the D800-DFFF range, which is not allowed, otherwise, we return the value
                    //
                    if((wc & 0xF800) != 0xD800)
                    {
                        *ppszBuffer += 3;
                        return wc;
                    }
                }
            }

            // We found a byte or sequence of bytes that we couldn't decode.
            //
            *ppszBuffer += 1;
            return 0;
        }
    };
    //-------------------------------------------------------------------------------------------------------------------------------------------
    class JsonReader2
    {
    public:

        JsonReader2(LPCSTR pszInput, ULONG nLength)
            : m_currentTokenType(JsonTokenType_NotStarted), 
            m_dwCurrentTokenLength(0), 
            m_dwStringLength(0), 
            m_previousTokenType(JsonTokenType_NotStarted), 
            m_bufferManager(pszInput, nLength),
            m_contextSaved(false)
        {
        }

    public:
        HRESULT GetTokenType(JsonTokenType* pTokenType)
        {
            if(!pTokenType) return E_POINTER;

            *pTokenType = m_currentTokenType;
            return S_OK;
        }

        HRESULT GetNumberValue(DOUBLE* pdblValue)
        {
            if(!pdblValue) return E_POINTER;

            if (m_currentTokenType == JsonTokenType::JsonTokenType_String)
            {
                return GetNumberValueFromString(pdblValue);
            }

            *pdblValue = 0;
            HRESULT hr = S_OK;
            if(m_dwCurrentTokenLength >= _countof(m_szNumberConversionBuffer)) return JSON_E_NUMBER_TOO_LONG;

            void *pContext = 0;
            for(DWORD i=0;i<m_dwCurrentTokenLength;i++)
            {
                m_szNumberConversionBuffer[i] = m_bufferManager.Peek(&pContext);
            }

            m_szNumberConversionBuffer[m_dwCurrentTokenLength] = 0;
            hr = ValidateNumber();
            if(FAILED(hr)) return hr; 

            DOUBLE d;
            if (!StringUtility::TryFromWString(std::string(m_szNumberConversionBuffer), d))
            {
                return JSON_E_INVALID_NUMBER;
            }

            *pdblValue = d;
            return hr;
        }

        HRESULT GetNumberValueFromString(DOUBLE *pdblValue)
        {
            std::string strValue;
            HRESULT hr = GetStringValue(&strValue);
            if (FAILED(hr)) { return hr; }

            *pdblValue = 0;
            if (strValue.empty()) { return S_OK; }
            if (!StringUtility::TryFromWString<DOUBLE>(strValue, *pdblValue)) { return E_FAIL; }

            return S_OK;
        }

        HRESULT GetStringValue(std::string* pstrData)
        {
            ULONG nLength = 0;
            HRESULT hr = GetStringLength(&nLength);
            if (FAILED(hr)) { return hr; }

            ULONG bufferSize;
            hr = ULongAdd(nLength, 1, &bufferSize);
            if (FAILED(hr)) { return hr; }

            std::vector<char> buffer(bufferSize);
            hr = GetStringValue(buffer.data(), bufferSize);
            *pstrData = std::string(buffer.data());

            return hr;
        }

        HRESULT GetFieldNameLength(ULONG* pnLength)
        {
            if(!pnLength) return E_POINTER;

            return GetStringLength(pnLength);
        }

        HRESULT GetStringLength(ULONG* pnLength)
        {
            if(!pnLength) return E_POINTER;

            *pnLength = m_dwStringLength;
            return S_OK;
        }

        HRESULT GetFieldName(__in_ecount(nLength) LPSTR pszBuffer, __in ULONG nLength)
        {
            if(!pszBuffer) return E_POINTER;

            if(m_currentTokenType != JsonTokenType_FieldName) return JSON_E_NOT_FIELDNAME_TOKEN; 

            return GetStringValue(pszBuffer, nLength);
        }

        HRESULT GetStringValue(__in_ecount(nLength) LPSTR pszBuffer, __in ULONG nLength)
        {
            if(!pszBuffer) return E_POINTER; 

            if(nLength < m_dwStringLength  + 1) return JSON_E_BUFFER_TOO_SMALL; 

            void *pContext = 0;
            // Skip the opening quote
            char wcCurrent = m_bufferManager.Peek(&pContext);
            if (wcCurrent != '"') return JSON_E_INVALID_STRING_CHARACTER;

            for(DWORD i=0;i < m_dwStringLength;i++)
            {
                wcCurrent = m_bufferManager.Peek(&pContext);
                if(wcCurrent == '\\')
                {
                    wcCurrent = m_bufferManager.Peek(&pContext);

                    switch(wcCurrent)
                    {
                    case 'b':
                        wcCurrent = '\b';
                        break;
                    case 'f':
                        wcCurrent = '\f';
                        break;
                    case 'n':
                        wcCurrent = '\n';
                        break;
                    case 'r':
                        wcCurrent = '\r';
                        break;
                    case 't':
                        wcCurrent = '\t';
                        break;
                    case 'v':
                        wcCurrent = '\v';
                        break;
                    case 'u':
                        wcCurrent = 0;
                        for(UINT j = 0;j < 4;j++)
                        {
                            wcCurrent <<= 4;

                            char wc = m_bufferManager.Peek(&pContext);
                            if(wc >= '0' && wc <= '9')
                            {
                                wcCurrent += wc - '0';
                            }
                            else if (wc >= 'A' && wc <= 'F')
                            {
                                wcCurrent += 10 + wc - 'A';
                            }
                            else if(wc >= 'a' && wc <='f')
                            {
                                wcCurrent += 10 + wc - 'a';
                            }
                        }
                        break;
                    default:
                        // Nothing to do for \\, \/ and \"
                        break;
                    }
                }

                pszBuffer[i] = wcCurrent;
            }

            pszBuffer[m_dwStringLength] = 0; // nullptr terminate the string
            return S_OK;
        }

        HRESULT Read()
        {
            m_previousTokenType = m_currentTokenType;

            // move past the current token so we can start looking for the next one
            m_bufferManager.Skip(m_dwCurrentTokenLength);
            m_dwStringLength = 0; // reset string length

            // Skip past whitespace to the start of the next token (or to the end of the
            // buffer if the whitespace is trailing)
            while(IsWhitespace(m_bufferManager.Peek()))
            {
                m_bufferManager.Read();
            }

            switch(m_bufferManager.Peek())
            {
            case '\"':
                if(InObject() && (m_previousTokenType == JsonTokenType_BeginObject || m_previousTokenType == JsonTokenType_ValueSeparator))
                {
                    return ProcessFieldName();
                }
                else
                {
                    return ProcessString();
                }
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return ProcessNumber();
            case '[':
                return ProcessBeginArray();
            case ']':
                return ProcessEndArray();
            case '{':
                return ProcessBeginObject();
            case '}':
                return ProcessEndObject();
            case 't':
                return ProcessTrue();
            case 'f':
                return ProcessFalse();
            case 'n':
                return ProcessNull();
            case ',':
                return ProcessValueSeparator();
            case ':':
                return ProcessNameSeparator();
            case 0:
            case ' ':
            case '\t':
            case '\n':
            case '\r':
            case '\v':
            case '\b':
            case '\f':
                // We will only get here if this is trailing whitespace after the last real token or
                // we've reached the actual end of the buffer
                return ProcessEndOfBuffer();

            default:
                // We found a start token character which doesn't match any JSON token type
                Trace.WriteWarning("JsonReader", "Unexpected JSON token: '{0}'", m_bufferManager.Peek());
                return JSON_E_UNEXPECTED_TOKEN;
            }
        }

        HRESULT GetDepth(ULONG* pnDepth)
        {
            if(!pnDepth) return E_POINTER;
            *pnDepth = m_stack.GetDepth();
            return S_OK;
        }

        HRESULT SaveContext()
        {
            ASSERT_IF(m_contextSaved == true, "Attempting to overwrite existing context record");

            m_contextSaved = m_savedContext.Save(
                m_currentTokenType,
                m_bufferManager, 
                m_dwCurrentTokenLength, 
                m_dwStringLength, 
                m_previousTokenType,
                m_stack);

            return S_OK;
        }

        HRESULT RestoreContext()
        {
            ASSERT_IF(m_contextSaved == false, "No context saved previously");

            m_savedContext.Restore(
                m_currentTokenType,
                m_bufferManager,
                m_dwCurrentTokenLength,
                m_dwStringLength,
                m_previousTokenType,
                m_stack);

            m_contextSaved = false;
            return S_OK;
        }

        HRESULT SkipObject()
        {
            if (m_currentTokenType == JsonTokenType_BeginArray)
            {
                return SkipCurrentArray();
            }
            else if (m_currentTokenType == JsonTokenType_BeginObject)
            {
                return SkipCurrentObject();
            }

            return JSON_E_INVALID_TOKEN;
        }

    private:
        bool InArray() const
        { 
            return m_stack.Peek() == '['; 
        }

        bool IsClosingQuote(char wcCurrent, char wcPrevious) const
        { 
            return wcCurrent == '\"' && wcPrevious != '\\'; 
        }

        bool InObject() const
        { 
            return m_stack.Peek() == '{'; 
        }

        bool IsControlCharacter(char wc) const
        {
            return wc < '\t' || 
                wc == 0x0B || // Vertical Tab
                wc == 0x0C || // Form Feed
                (wc > '\r' && wc < ' ');
        }

        bool IsHexNumberCharacter(char wc) const
        {
            return isdigit(wc) || (wc >= 'a' && wc <= 'f') || (wc >= 'A' && wc <= 'F');        
        }

        bool IsNumberCharacter(char wc) const
        {
            return isdigit(wc) || wc == '.' || wc == 'e' || wc == 'E' || wc == '+' || wc == '-';
        }

        bool IsWhitespace(char wc) const
        { 
            return wc == ' ' || wc == '\t' || wc == '\r' || wc == '\n';
        }

        HRESULT CheckPrecedingTokenForValue() const
        {
            HRESULT hr = S_OK;
            if(InObject() && m_previousTokenType != JsonTokenType_NameSeparator)
            {
                hr = JSON_E_MISSING_NAME_SEPARATOR;
            }
            else if(InArray() && m_previousTokenType != JsonTokenType_BeginArray && m_previousTokenType != JsonTokenType_ValueSeparator)
            {
                hr = JSON_E_UNEXPECTED_TOKEN; // TODO: More specific error code?
            }
            else if(!InObject() && !InArray() && m_previousTokenType != JsonTokenType_NotStarted)
            {
                hr = JSON_E_UNEXPECTED_TOKEN; // TODO: More specific error code?
            }

            return hr;
        }

        DWORD GetLengthOfNumberToken() const
        {
            ASSERT_IF(m_bufferManager.Peek() != '-' && !isdigit(m_bufferManager.Peek()), "Invalid Number format");

            DWORD dwTokenLength = 0;

            void *pContext = 0;

            while(IsNumberCharacter(m_bufferManager.Peek(&pContext)))
            {
                dwTokenLength++;
            }

            return dwTokenLength;
        }

        // This method should be thought of as part of the implementation of ProcessString.
        // It sets the value of m_dwCurrentTokenLength and m_dwStringLength;
        HRESULT GetLengthOfStringToken()
        {
            ASSERT_IF(m_bufferManager.Peek() != '\"', "Invalid String format");
            DWORD dwTokenLength = (DWORD)0;
            DWORD dwStringLength = (DWORD)-1; // To account for the fact that the opening quote is NOT part of the string length
            char wcPrevious = '\\';  // This ensures that IsClosingQuote will return false initially

            void *pContext = 0;
            char wcCurrent = m_bufferManager.Peek(&pContext);

            while(!IsClosingQuote(wcCurrent, wcPrevious))
            {
                if(wcCurrent == 0) 
                { 
                    return JSON_E_MISSING_CLOSING_QUOTE; 
                }

                if(IsControlCharacter(wcCurrent)) 
                { 
                    return JSON_E_INVALID_STRING_CHARACTER; 
                }

                dwTokenLength++;
                dwStringLength++;

                // Process escaped characters
                if(wcCurrent == '\\')
                {
                    wcCurrent = m_bufferManager.Peek(&pContext);

                    switch (wcCurrent)
                    {
                    case '\\':
                        // we escaped a backslash. We must lie about wcCurrent
                        // so that if the next character is a quote we won't
                        // skip past it
                        wcCurrent = 0;
                        // fall through
                    case '\"':
                    case '/':
                    case 'b':
                    case 'f':
                    case 'n':
                    case 'r':
                    case 't':
                    case 'v':
                        dwTokenLength++;
                        break;
                    case 'u':
                        if(!isxdigit(m_bufferManager.Peek(&pContext)) ||
                            !isxdigit(m_bufferManager.Peek(&pContext)) ||
                            !isxdigit(m_bufferManager.Peek(&pContext)) ||
                            !isxdigit(m_bufferManager.Peek(&pContext)))
                        {
                            return JSON_E_INVALID_UNICODE_ESCAPE;
                        }

                        dwTokenLength += 5; // u plus 4 hex digits (we counted the backslash with dwTokenLength++ earlier in the while loop)
                        break;
                    default:
                        return JSON_E_INVALID_ESCAPED_CHARACTER;
                    }
                }

                wcPrevious = wcCurrent;
                wcCurrent = m_bufferManager.Peek(&pContext);
            }

            dwTokenLength++; // Closing quote
            m_dwStringLength = dwStringLength;
            m_dwCurrentTokenLength = dwTokenLength;
            return S_OK;
        }

        HRESULT ProcessBeginArray()
        {
            HRESULT hr = CheckPrecedingTokenForValue();
            m_currentTokenType = JsonTokenType_BeginArray;
            m_dwCurrentTokenLength = 1;
            if(m_stack.Push('[') != S_OK)
            {
                hr = JSON_E_MAX_NESTING_EXCEEDED;
            }

            return hr;
        }

        HRESULT ProcessBeginObject()
        {
            HRESULT hr = CheckPrecedingTokenForValue();
            m_currentTokenType = JsonTokenType_BeginObject;
            m_dwCurrentTokenLength = 1;
            if(m_stack.Push('{') != S_OK)
            {
                hr = JSON_E_MAX_NESTING_EXCEEDED;
            }

            return hr;
        }

        HRESULT ProcessEndArray()
        {
            HRESULT hr = S_OK;
            if(!InArray())
            {
                hr = JSON_E_UNEXPECTED_END_ARRAY;
            }
            else if(m_previousTokenType == JsonTokenType_NotStarted    ||
                m_previousTokenType == JsonTokenType_BeginObject   || // TODO: This case is actually caught by the InArray check. Remove?
                m_previousTokenType == JsonTokenType_FieldName     ||
                m_previousTokenType == JsonTokenType_NameSeparator ||
                m_previousTokenType == JsonTokenType_ValueSeparator)
            {
                hr = JSON_E_UNEXPECTED_END_ARRAY;
            }

            m_currentTokenType = JsonTokenType_EndArray;
            m_dwCurrentTokenLength = 1;
            char wTemp;
            m_stack.Pop(&wTemp);
            return hr;
        }

        HRESULT ProcessEndObject()
        {
            HRESULT hr = S_OK;
            if(!InObject())
            {
                hr = JSON_E_UNEXPECTED_END_OBJECT;
            }
            else if(m_previousTokenType == JsonTokenType_NotStarted    ||
                m_previousTokenType == JsonTokenType_BeginArray    || // TODO: This case is actually caught by the InObject check. Remove?
                m_previousTokenType == JsonTokenType_FieldName     ||
                m_previousTokenType == JsonTokenType_NameSeparator ||
                m_previousTokenType == JsonTokenType_ValueSeparator)
            {
                hr = JSON_E_UNEXPECTED_END_OBJECT;
            }

            m_currentTokenType = JsonTokenType_EndObject;
            m_dwCurrentTokenLength = 1;
            char wTemp;
            m_stack.Pop(&wTemp);
            return hr;
        }

        HRESULT ProcessEndOfBuffer()
        {
            if(InObject())
            {
                return JSON_E_MISSING_END_OBJECT;
            }
            else if(InArray())
            {
                return JSON_E_MISSING_END_ARRAY;
            }

            return S_FALSE;
        }

        HRESULT ProcessFalse()
        {
            HRESULT hr = CheckPrecedingTokenForValue();
            if(FAILED(hr)) return hr;

            void*pContext = 0;
            if(m_bufferManager.Peek(&pContext) != 'f' ||
                m_bufferManager.Peek(&pContext) != 'a' ||
                m_bufferManager.Peek(&pContext) != 'l' ||
                m_bufferManager.Peek(&pContext) != 's' ||
                m_bufferManager.Peek(&pContext) != 'e')
            {
                hr = JSON_E_INVALID_TOKEN;
            }
            else
            {
                m_currentTokenType = JsonTokenType_False;
                m_dwCurrentTokenLength = 5;
            }

            return hr;
        }

        HRESULT ProcessFieldName()
        {
            // We know we're 'InObject' and that the previous token type was Json_ValueSeparator otherwise we would be in ProcessString
            //
            ASSERT_IF(!InObject(), "Field name cannot be specified without an object begin");
            ASSERT_IF(
                ((m_previousTokenType != JsonTokenType_BeginObject) && (m_previousTokenType != JsonTokenType_ValueSeparator)),
                "Field name can only follow beginObject or a value separator");
            m_currentTokenType = JsonTokenType_FieldName;
            return GetLengthOfStringToken();
        }

        HRESULT ProcessNameSeparator()
        {
            if(!InObject()) // TODO: I suspect the else clause is sufficient here, the previous token will only be Json_FieldName if we are already InObject
            {
                return JSON_E_UNEXPECTED_NAME_SEPARATOR;
            }
            else if (m_previousTokenType != JsonTokenType_FieldName)
            {
                return JSON_E_UNEXPECTED_NAME_SEPARATOR;
            }

            m_currentTokenType = JsonTokenType_NameSeparator;
            m_dwCurrentTokenLength = 1;
            // We don't surface Json_NameSeparator tokens to the caller, so proceed to the next token
            //
            return Read();
        }

        HRESULT ProcessNull()
        {
            HRESULT hr = CheckPrecedingTokenForValue();
            if(FAILED(hr)) return hr;

            void *pContext = 0;
            if(m_bufferManager.Peek(&pContext) != 'n' ||
                m_bufferManager.Peek(&pContext) != 'u' ||
                m_bufferManager.Peek(&pContext) != 'l' ||
                m_bufferManager.Peek(&pContext) != 'l')
            {
                hr = JSON_E_INVALID_TOKEN;
            }
            else
            {
                m_currentTokenType = JsonTokenType_Null;
                m_dwCurrentTokenLength = 4;
            }

            return hr;
        }

        HRESULT ProcessNumber()
        {
            HRESULT hr = CheckPrecedingTokenForValue();
            m_currentTokenType = JsonTokenType_Number;
            m_dwCurrentTokenLength = GetLengthOfNumberToken();
            return hr;
        }

        HRESULT ProcessString()
        {
            HRESULT hr = CheckPrecedingTokenForValue();
            if(FAILED(hr)) return hr;

            m_currentTokenType = JsonTokenType_String;
            return GetLengthOfStringToken();
        }

        HRESULT ProcessTrue()
        {
            HRESULT hr = CheckPrecedingTokenForValue();
            if(FAILED(hr)) return hr;

            void *pContext = 0;

            if(m_bufferManager.Peek(&pContext) != 't' ||
                m_bufferManager.Peek(&pContext) != 'r' ||
                m_bufferManager.Peek(&pContext) != 'u' ||
                m_bufferManager.Peek(&pContext) != 'e')
            {
                hr = JSON_E_INVALID_TOKEN;
            }
            else
            {
                m_currentTokenType = JsonTokenType_True;
                m_dwCurrentTokenLength = 4;
            }

            return hr;
        }

        HRESULT ProcessValueSeparator()
        {
            if(!InObject() && !InArray())
            {
                return JSON_E_UNEXPECTED_VALUE_SEPARATOR;
            }
            else if(m_previousTokenType != JsonTokenType_True && m_previousTokenType != JsonTokenType_False &&
                m_previousTokenType != JsonTokenType_Null && m_previousTokenType != JsonTokenType_String &&
                m_previousTokenType != JsonTokenType_Number && m_previousTokenType != JsonTokenType_EndObject &&
                m_previousTokenType != JsonTokenType_EndArray)
            {
                return JSON_E_UNEXPECTED_VALUE_SEPARATOR;
            }

            m_currentTokenType = JsonTokenType_ValueSeparator;
            m_dwCurrentTokenLength = 1;

            // We don't surface Json_ValueSeparator tokens to the caller, so proceed to the next token
            return Read();
        }

        HRESULT ValidateNumber()
        {
            UINT nValue = 0;

            // First character was '-'
            if(m_szNumberConversionBuffer[nValue] == '-')
            {
                // No more characters in this token, - but no following characters, error
                if(nValue + 1 == m_dwCurrentTokenLength) 
                { 
                    return JSON_E_INVALID_NUMBER; 
                }

                nValue++;

                // There MUST be at least one digit after the -
                if(!isdigit(m_szNumberConversionBuffer[nValue])) 
                { 
                    return JSON_E_INVALID_NUMBER; 
                }
            }

            // First (or second) digit is 0, token must be -0, 0, 0.XXX, 0eXXX or 0EXXX
            if(m_szNumberConversionBuffer[nValue] == '0')
            {
                // Number is -0 or 0
                if(nValue + 1 == m_dwCurrentTokenLength) 
                { 
                    return S_OK; 
                }

                nValue++;
            }
            else // Number did not start with a zero
            {
                while(isdigit(m_szNumberConversionBuffer[nValue]))
                {
                    // No more characters in this token; there is no fractional part or exponent
                    if(nValue + 1 == m_dwCurrentTokenLength) 
                    { 
                        return S_OK; 
                    }

                    nValue++;
                }
            }

            // If the character following the mantissa is not ., e or E then the number is invalid
            if(m_szNumberConversionBuffer[nValue] != '.' &&
                m_szNumberConversionBuffer[nValue] != 'e' &&
                m_szNumberConversionBuffer[nValue] != 'E')
            {
                return JSON_E_INVALID_NUMBER;
            }

            // Process the fractional part
            if(m_szNumberConversionBuffer[nValue] == '.')
            {
                // No more characters in this token, . but no following characters, error
                if(nValue + 1 == m_dwCurrentTokenLength) 
                { 
                    return JSON_E_INVALID_NUMBER; 
                }

                nValue++;
                // There MUST be at least one digit after the .
                if(!isdigit(m_szNumberConversionBuffer[nValue])) 
                { 
                    return JSON_E_INVALID_NUMBER; 
                }

                while(isdigit(m_szNumberConversionBuffer[nValue]))
                {
                    // No more characters in this token; there is no exponent
                    if(nValue + 1 == m_dwCurrentTokenLength) 
                    { 
                        return S_OK; 
                    }

                    nValue++;
                }
            }

            // If the character following the mantiassa and optional fractional part is not e or E then the number is invalid
            if(m_szNumberConversionBuffer[nValue] != 'e' &&
                m_szNumberConversionBuffer[nValue] != 'E')
            {
                return JSON_E_INVALID_NUMBER;
            }

            // No more characters in this token, e or E but no following +, - or digits, error
            if(nValue + 1 == m_dwCurrentTokenLength) 
            { 
                return JSON_E_INVALID_NUMBER; 
            }

            nValue++;
            if(m_szNumberConversionBuffer[nValue] == '+' ||
                m_szNumberConversionBuffer[nValue] == '-')
            {
                // No more characters in this token, + or - but no following digits, error
                if(nValue + 1 == m_dwCurrentTokenLength) 
                { 
                    return JSON_E_INVALID_NUMBER; 
                }

                nValue++;
            }

            // There MUST be at least one digit after the e, E, +, -
            if(!isdigit(m_szNumberConversionBuffer[nValue])) 
            { 
                return JSON_E_INVALID_NUMBER; 
            }

            while(isdigit(m_szNumberConversionBuffer[nValue]))
            {
                // No more characters in this token; we've reach the end of the exponent
                if(nValue + 1 == m_dwCurrentTokenLength) 
                { 
                    return S_OK; 
                }

                nValue++;
            }

            // If we get here, we encountered a non-digit
            return JSON_E_INVALID_NUMBER;
        }

        HRESULT SkipCurrentArray()
        {
            if (!InArray())
            {
                return JSON_E_INVALID_TOKEN;
            }

            m_previousTokenType = m_currentTokenType;

            // move past the current token so we can start looking for the next one
            m_bufferManager.Skip(m_dwCurrentTokenLength);
            m_dwStringLength = 0; // reset string length
            HRESULT hr = S_OK;

            int numberOfBeginArrayTokens = 1; // Used to handle nested array's.
			for (;;)
            {
                switch (m_bufferManager.Peek())
                {
                    case '[':
                    {
                        numberOfBeginArrayTokens++;
                        hr = m_bufferManager.Read();
                        if (FAILED(hr)) { return hr; }
                        break;
                    }
                    case ']':
                    {
                        if (--numberOfBeginArrayTokens == 0)
                        {
                            return ProcessEndArray();
                        }
                        hr = m_bufferManager.Read();
                        if (FAILED(hr)) { return hr; }
                        break;
                    }

                    default:
                    {
                        // Skip the current token.
                        hr = m_bufferManager.Read();
                        if (FAILED(hr)) { return hr; }
                        break;
                    }
                }
            }
        }

        HRESULT SkipCurrentObject()
        {
            if (!InObject())
            {
                return JSON_E_INVALID_TOKEN;
            }

            m_previousTokenType = m_currentTokenType;

            // move past the current token so we can start looking for the next one
            m_bufferManager.Skip(m_dwCurrentTokenLength);
            m_dwStringLength = 0; // reset string length
            HRESULT hr = S_OK;

            int numberOfBeginObjectTokens = 1; // Used to handle nested objects.

			for (;;)
            {
                switch (m_bufferManager.Peek())
                {
                case '{':
                {
                    numberOfBeginObjectTokens++;
                    hr = m_bufferManager.Read();
                    if (FAILED(hr)) { return hr; }
                    break;
                }
                case '}':
                {
                    if (--numberOfBeginObjectTokens == 0)
                    {
                        return ProcessEndObject();
                    }
                    hr = m_bufferManager.Read();
                    if (FAILED(hr)) { return hr; }
                    break;
                }

                default:
                {
                    // Skip the current token.
                    hr = m_bufferManager.Read();
                    if (FAILED(hr)) { return hr; }
                }
                }
            }
        }

    private:
        JsonBufferManager2<char> m_bufferManager;
        JsonTokenType            m_currentTokenType;
        DWORD                    m_dwCurrentTokenLength;
        DWORD                    m_dwStringLength;
        JsonTokenType            m_previousTokenType;
        JSONStack                m_stack;
        char                    m_szNumberConversionBuffer[64];
        bool                     m_contextSaved;

        struct ContextRecord
        {
            JsonTokenType            m_currentTokenType;
            JsonBufferManager2<char> m_bufferManager;
            DWORD                    m_dwCurrentTokenLength;
            DWORD                    m_dwStringLength;
            JsonTokenType            m_previousTokenType;
            JSONStack                m_stack;

            bool Save(
                JsonTokenType            const &currentTokenType,
                JsonBufferManager2<char> const &bufferManager,
                DWORD                    dwCurrentTokenLength,
                DWORD                    dwStringLength,
                JsonTokenType            const &previousTokenType,
                JSONStack                const &stack)
            {
                m_currentTokenType = currentTokenType;
                m_bufferManager = bufferManager;
                m_dwCurrentTokenLength = dwCurrentTokenLength;
                m_dwStringLength = dwStringLength;
                m_previousTokenType = previousTokenType;
                m_stack = stack;
                return true;
            }

            void Restore(
                JsonTokenType            &currentTokenType,
                JsonBufferManager2<char> &bufferManager,
                DWORD                    &dwCurrentTokenLength,
                DWORD                    &dwStringLength,
                JsonTokenType            &previousTokenType,
                JSONStack                &stack)
            {
                currentTokenType = m_currentTokenType;
                bufferManager = m_bufferManager;
                dwCurrentTokenLength = m_dwCurrentTokenLength;
                dwStringLength = m_dwStringLength;
                previousTokenType = m_previousTokenType;
                stack = m_stack;
            }
        };

        ContextRecord            m_savedContext;
    };
    //-------------------------------------------------------------------------------------------------------------------------------------------
    // TODO: rename JsonBufferManager2 to JsonBufferManager when jpath.lib is removed
    class JsonBufferManager
    {
    public:
        JsonBufferManager(LPCSTR pszBuffer)
        {
            Initialize(pszBuffer, nullptr, (DWORD)strlen(pszBuffer));
        }

        JsonBufferManager(LPCSTR pszBuffer, DWORD nBufferLength)
        {
            Initialize(pszBuffer, nullptr, nBufferLength);
        }

        ~JsonBufferManager()
        {
        }

        // Returns the current character and advances to the next
        //
        char Read()
        {
            // Figure out whether we're working over a UTF-16 or UTF-8 buffer, check to make sure
            // we've not run off the end of the buffer and return the next character.
            //
            return (IsEOF(m_pszCharBuffer) ? 0 : GetNextUTF8Char(&m_pszCharBuffer));
        }

        // Returns the current character. Does NOT advance to the next character.
        //
        char Peek()
        {
            void *pContext = 0;
            return Peek(&pContext);
        }

        // ppContext MUST NOT be nullptr.
        // if *ppContext is nullptr, returns the current character and stores a value in *ppContext such
        // that subsequent calls to Peek with that same context will return consecutive characters
        //
        char Peek(void** ppContext)
        {
            if(ppContext == nullptr) return 0;

            return PeekChar((LPCSTR*)ppContext);
        }

        void Skip(DWORD dwCharsToSkip)
        {
            // TODO: Fast path for UTF-16
            while(dwCharsToSkip)
            {
                if(!Read()) break; // GetChar returns 0 if we hit the end of the buffer
                dwCharsToSkip--;
            }
        }

    private:
        char GetNextUTF8Char(LPCSTR* ppszBuffer)
        {
            // add an assert because caller does the argument check, and does not expect failure of function.
            //
            ASSERT_IF(!ppszBuffer, "Invalid data");
            ASSERT_IF(!(*ppszBuffer), "Invalid data");

            BYTE b1 = **ppszBuffer;
            // Top bit is 0; Unicode Code Point is in the range 0x0000 to 0x007f; single byte character
            //
            if(b1 <= 0x7F)
            { 
                *ppszBuffer += 1;
                return b1;
            }

            // Top 3 bits are 110 ; Unicode Code Point is in the range 0x0080 to 0x07FF; two byte character
            // Check that b1 > 0xC1 to ensure that we don't have a code point in the 0x00-0x7F range encoded in two bytes
            //
            if((b1 & 0xE0) == 0xC0 && b1 > 0xC1 && !IsEOF(*ppszBuffer + 1))
            {
                BYTE b2 = *(*ppszBuffer + 1);
                if((b2 & 0xC0) == 0x80)
                {
                    *ppszBuffer += 2;
                    return ((b1 & 0x1F) << 6) | (b2 & 0x3F);
                }
            }

            // Top 4 bits are 1110; Unicode Code Point is in the range 0x0800 to 0xFFFF, excluding D800-DFFF; three byte character
            //
            if((b1 & 0xF0) == 0xE0 && !IsEOF(*ppszBuffer + 2))
            {
                BYTE b2 = *(*ppszBuffer + 1);
                BYTE b3 = *(*ppszBuffer + 2);

                // Top 2 bits of b2 and b3 are 10.
                // Check that either b1 > 0xE0 or b2 > 0x9F to ensure we don't have a code point in the 0x0000-0x07FF range encoded in three bytes
                //
                if((b2 & 0xC0) == 0x80 && (b3 & 0xC0) == 0x80 && (b1 > 0xE0 || b2 > 0x9F))
                {
                    char wc = ((b1 & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);

                    // If the top five bits are 11011, then the value is in the D800-DFFF range, which is not allowed, otherwise, we return the value
                    //
                    if((wc & 0xF800) != 0xD800)
                    {
                        *ppszBuffer += 3;
                        return wc;
                    }
                }
            }

            // We found a byte or sequence of bytes that we couldn't decode.
            //
            *ppszBuffer += 1;
            return 0;
        }

        void Initialize(LPCSTR pszBuffer, LPCSTR pszWideCharBuffer, DWORD nBufferLength)
        {
            ASSERT_IF(
                ((pszBuffer == nullptr) && (pszWideCharBuffer == nullptr)),
                "Invalid data");
            m_pszCharBuffer = pszBuffer;
            m_pszCharBufferStart = pszBuffer;
            m_nBufferLength = nBufferLength;
        }

        char PeekChar(LPCSTR* ppszBuffer)
        {
            if(ppszBuffer == nullptr || m_pszCharBuffer == nullptr) return 0; 

            if(*ppszBuffer == nullptr) 
            { 
                *ppszBuffer = m_pszCharBuffer; 
            }

            if(*ppszBuffer < m_pszCharBufferStart) return 0; 

            if(IsEOF(*ppszBuffer)) return 0; 

            return GetNextUTF8Char(ppszBuffer);
        }

        bool IsEOF(LPCSTR pszStart) const
        { 
            return pszStart >= m_pszCharBufferStart + m_nBufferLength; 
        }

    private:
        // TODO: Discriminated union over various buffer pointers? Might save us 4 bytes...
        LPCSTR  m_pszCharBuffer;
        LPCSTR  m_pszCharBufferStart;
        DWORD   m_nBufferLength;
    };
    //-------------------------------------------------------------------------------------------------------------------------------------------
    class JsonReader: public IJsonReader, private Common::ComUnknownBase
    {
    public:
        struct InitParams
        {
            InitParams(LPCSTR pszInput) : m_pszInput(pszInput), m_nLength((ULONG)strlen(pszInput)) {}
            InitParams(LPCSTR pszInput, ULONG nLength) : m_pszInput(pszInput), m_nLength(nLength) {}
            LPCSTR m_pszInput;
            ULONG m_nLength;
        };

        JsonReader(const InitParams& params)
            : m_reader(params.m_pszInput, params.m_nLength)
        {
        }

    public:
        BEGIN_COM_INTERFACE_LIST(JsonReader)
            COM_INTERFACE_ITEM(__uuidof(IJsonReader), IJsonReader) 
            END_COM_INTERFACE_LIST()

    public:
        //IJsonReader
        STDMETHODIMP GetTokenType(JsonTokenType* pTokenType)
        {
            if(!pTokenType) return E_POINTER;

            return m_reader.GetTokenType(pTokenType);
        }

        STDMETHODIMP GetNumberValue(DOUBLE* pdblValue)
        {
            if(!pdblValue) return E_POINTER;
            *pdblValue = 0;

            return m_reader.GetNumberValue(pdblValue);
        }

        STDMETHODIMP GetFieldNameLength(ULONG* pnLength)
        {
            if(!pnLength) return E_POINTER;
            *pnLength = 0;

            return m_reader.GetFieldNameLength(pnLength);
        }

        STDMETHODIMP GetStringLength(ULONG* pnLength)
        {
            if(!pnLength) return E_POINTER;
            *pnLength = 0;

            return m_reader.GetStringLength(pnLength);
        }

        STDMETHODIMP GetFieldName(__in_ecount(nLength) LPSTR pszBuffer, __in ULONG nLength)
        {
            if(!pszBuffer) return E_POINTER;

            return m_reader.GetFieldName(pszBuffer, nLength);
        }

        STDMETHODIMP GetStringValue(__in_ecount(nLength) LPSTR pszBuffer, __in ULONG nLength)
        {
            if(!pszBuffer) return E_POINTER; 

            return m_reader.GetStringValue(pszBuffer, nLength);
        }

        STDMETHODIMP Read()
        {
            return m_reader.Read();
        }

        STDMETHODIMP GetDepth(ULONG* pnDepth)
        {
            if(!pnDepth) return E_POINTER;
            *pnDepth = 0;

            return m_reader.GetDepth(pnDepth);
        }

        STDMETHODIMP SkipObject()
        {
            return m_reader.SkipObject();
        }

        STDMETHODIMP SaveContext()
        {
            return m_reader.SaveContext();
        }

        STDMETHODIMP RestoreContext()
        {
            return m_reader.RestoreContext();
        }

    private:
        JsonReader2 m_reader;
    };
    //-------------------------------------------------------------------------------------------------------------------------------------------
}

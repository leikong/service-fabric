// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "pal_string_util.h"
#include <codecvt>
#include <locale>

using namespace std;
using namespace Pal;

// 
// char16_t needs to be used if -fshort-wchar is enabled
//
//using lchar_t = char16_t;
using lchar_t = char;

string Pal::utf8to16(const char *str)
{
    string_convert<codecvt_utf8_utf16<char>, char> conv;
    auto u16str = conv.from_bytes(str);
    basic_string<char> result;
    for(int index = 0; index < u16str.length(); index++)
    {
        result.push_back((char)u16str[index]);
    }
    return result;
}

string Pal::utf16to8(const char *wstr)
{
    string_convert<codecvt_utf8_utf16<char>, char> conv;
    return conv.to_bytes((const char *) wstr);
}

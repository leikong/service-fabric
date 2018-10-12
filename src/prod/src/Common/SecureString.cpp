// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace std;
using namespace Common;

GlobalString SecureString::TracePlaceholder = make_global<std::string>("<SecureString>");

SecureString::SecureString()
    : value_()
{    
}

SecureString::SecureString(string const & secureValue)
    : value_(secureValue)
{    
}

SecureString::SecureString(string && secureValue)
    : value_(move(secureValue))
{    
}

SecureString::~SecureString()
{
    this->Clear();
}

bool SecureString::operator == (SecureString const & other) const
{    
    return this->value_ == other.value_;    
}

bool SecureString::operator != (SecureString const & other) const
{
    return !(*this == other);
}

SecureString const & SecureString::operator = (SecureString const & other)
{
    if (this != & other)
    {
        this->value_ = other.value_;
    }

    return *this;
}

SecureString const & SecureString::operator = (SecureString && other)
{
    if (this != & other)
    {
        this->value_ = move(other.value_);
    }

    return *this;
}

string const & SecureString::GetPlaintext() const
{
    return value_;
}

bool SecureString::IsEmpty() const
{
    return value_.empty();
}

void SecureString::Append(string const & value)
{    
    value_.append(value);
}

void SecureString::Append(SecureString const & value)
{    
    value_.append(value.GetPlaintext());
}

void SecureString::TrimTrailing(string const & trimChars)
{    
    StringUtility::TrimTrailing<string>(value_, trimChars);
}

void SecureString::Replace(std::string const & stringToReplace, std::string const & stringToInsert)
{    
    StringUtility::Replace<string>(value_, stringToReplace, stringToInsert);
}

void SecureString::WriteTo(TextWriter& w, FormatOptions const &) const
{
    w.Write(*TracePlaceholder);
}

void SecureString::Clear() const
{
    SecureZeroMemory((void *) value_.c_str(), value_.size() * sizeof(char));
}

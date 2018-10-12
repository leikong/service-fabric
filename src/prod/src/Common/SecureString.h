// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class SecureString
    {
        DEFAULT_COPY_CONSTRUCTOR(SecureString)

    public:
        static GlobalString TracePlaceholder;

    public:
        SecureString();
        explicit SecureString(std::string const & secureValue);
        explicit SecureString(std::string && secureValue);
        ~SecureString();

        bool operator == (SecureString const & other) const;
        bool operator != (SecureString const & other) const;

        SecureString const & operator = (SecureString const & other);
        SecureString const & operator = (SecureString && other);

        std::string const & GetPlaintext() const;  

        bool IsEmpty() const;
        void Append(std::string const &);
        void Append(SecureString const &);
        void TrimTrailing(std::string const & trimChars);
        void Replace(std::string const & stringToReplace, std::string const & stringToInsert);

        void WriteTo(Common::TextWriter& w, Common::FormatOptions const &) const;

    private:
        void Clear() const;

        std::string value_;
    };
}

// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class ThumbprintSet
    {
    public:
        Common::ErrorCode Set(std::string const & thumbprints);
        Common::ErrorCode Add(std::string const & thumbprintString);
        Common::ErrorCode Add(LPCSTR thumbprintString);

        void Add(Thumbprint const & thumbprint);
        void Add(ThumbprintSet const & other);

        std::set<Common::Thumbprint> const & Value() const;

        bool Contains(Common::Thumbprint const & thumbprint) const;
        bool Contains(ThumbprintSet const & other) const;

        bool operator == (ThumbprintSet const & rhs) const;
        bool operator != (ThumbprintSet const & rhs) const;

        void WriteTo(Common::TextWriter & w, Common::FormatOptions const &) const;
        std::string ToString() const;

    private:
        std::set<Common::Thumbprint> value_;
    };
}

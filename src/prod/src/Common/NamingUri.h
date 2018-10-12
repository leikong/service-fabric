// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class NamingUri : public Common::Uri
    {
    public:
        // NamingUri constants. Keep these in the same file to avoid
        // static initialization order issues since some of these
        // constants are interdependent.
        //
        static Common::GlobalString NameUriScheme;
        static Common::GlobalString RootAuthority;
        static Common::GlobalString RootNamingUriString;
        static Common::GlobalString InvalidRootNamingUriString;
        static Common::GlobalString NamingUriConcatenationReservedTokenDelimiter;
        static Common::GlobalString ReservedTokenDelimiters;
        static Common::Global<NamingUri> RootNamingUri;
        static Common::GlobalString SegmentDelimiter;
        static Common::GlobalString HierarchicalNameDelimiter;

        struct Hasher
        {
            size_t operator() (NamingUri const & key) const
            {
                return (!key.HasQueryOrFragment ? key.GetHash() : key.GetTrimQueryAndFragmentHash());
            }
            bool operator() (NamingUri const & left, NamingUri const & right) const
            {
                if (!left.HasQueryOrFragment && !right.HasQueryOrFragment)
                {
                    return left == right;
                }
                else
                {
                    return left.GetTrimQueryAndFragment() == right.GetTrimQueryAndFragment();
                }
            }
        };

        NamingUri() : Uri(NameUriScheme, "")
        {
        }

        explicit NamingUri(std::string const & path) : Uri(NameUriScheme, "", path)
        {
            ASSERT_IFNOT(IsNamingUri(*this), "{0} does not make a valid NamingUri", *this);
        }

        // Returns actual name after stripping off query strings, etc.
        __declspec(property(get=get_Name)) std::string Name;
        std::string get_Name() const { return ToString(); }

        __declspec(property(get=get_IsRootNamingUri)) bool IsRootNamingUri;
        bool get_IsRootNamingUri() const { return *this == RootNamingUri; }

        NamingUri GetAuthorityName() const;

        NamingUri GetParentName() const;

        NamingUri GetTrimQueryAndFragmentName() const;

        size_t GetHash() const;

        size_t GetTrimQueryAndFragmentHash() const;

        bool TryCombine(std::string const & path, __out NamingUri &) const;

        // Validates that the name can be created or deleted in naming for user operations.
        // Names can't be root URI, or have reserved characters (?).
        // If validateNameFragment is true, the name should not have characters reserved for service groups (#).
        // The API takes the URI as well as the string representation of the URI, to optimize
        // the cases when the string is cached and should not be re-evaluated (see Naming StoreService operations).
        static ErrorCode ValidateName(NamingUri const & uri, std::string const & uriString, bool allowFragment);

        static NamingUri Combine(NamingUri const &, std::string const & path);

        bool IsPrefixOf(NamingUri const &) const;

        static HRESULT TryParse(
            FABRIC_URI name,
            std::string const & traceId,
            __out NamingUri & nameUri);

        static ErrorCode TryParse(
            FABRIC_URI name,
            Common::StringLiteral const & parameterName,
            __inout NamingUri & nameUri);

        static ErrorCode TryParse(
            std::string const & nameText,
            std::string const & traceId,
            __out NamingUri & nameUri);

        static bool TryParse(std::string const & nameText, __out NamingUri & nameUri);

        // string has an implicit ctor that will get bound to if FABRIC_URI is nullptr,
        // resulting in an AV. The TryParse() functions that take an additional traceId/parameterName
        // for debugging are preferred, but create this overload to avoid binding to the
        // TryParse(string const &, _out NamingUri const &) function above by accident.
        //
        static bool TryParse(FABRIC_URI name, __out NamingUri & nameUri);

        static bool IsNamingUri(Common::Uri const & question);

        //
        // Routines to convert fabric namespace names to an ID.
        // Fabric namespace names are of the form <scheme>:/<path>.
        // The scheme can be either 'fabric' or 'sys'. The name to ID conversion
        // removes the scheme portion and replaces / with ~.
        //

        static ErrorCode FabricNameToId(std::string const & name, __inout std::string & escapedId);
        static ErrorCode FabricNameToId(std::string const & name, bool useDelimiter, __inout std::string & escapedId);

        static ErrorCode IdToFabricName(std::string const& scheme, std::string const& id, __inout std::string &name);

        static ErrorCode EscapeString(std::string const & input, __inout std::string & output);
        static ErrorCode UnescapeString(std::string const & input, __inout std::string & output);

        // This method calls EscapeString internally
        // Escape string converts all non ascii characters into %xx representations.
        // However, this method is not actually URL standards compliant as it does not encode values such
        // as the comma or semi colon, which is deemed as reserved characters in a URL.
        //
        // This method encodes the following characters in addition to what EscapeString encodes:
        // dollar("$")
        // plus sign("+")
        // comma(",")
        // colon(":")
        // semi - colon(";")
        // equals("=")
        // 'At' symbol("@")
        //
        // The following characters are already encoded by EscapeString. 
        // We encode them again in case that implementation changes. This way, we also ensure that
        // this method encodes URLs safely without OS restrictions - see the forward slash for example.
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
        // percent("%")
        // space(" ")
        //
        // To unescape this, use UnescapeString. EscapeString will not encode the characters above, but
        // the matching function will decode.
        static ErrorCode UrlEscapeString(std::string const & input, __out std::string & output);

    private:
        NamingUri(
            std::string const & scheme,
            std::string const & authority,
            std::string const & path,
            std::string const & query,
            std::string const & fragment);
        static ErrorCode ParseUnsafeUri(std::string const & input, std::string & protocol, std::string & host, std::string & path, std::string & queryFragment);
        static void ParseHost(std::string & input, std::string & host, std::string & remain);
        static void ParsePath(std::string & input, std::string & path, std::string & remain);
    };
}

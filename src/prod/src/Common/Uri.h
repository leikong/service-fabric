// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    // We do not support URI-reference, so NoScheme is not included.
    namespace UriType 
    { 
        enum Enum { AuthorityAbEmpty, Absolute, Rootless, Empty/*, NoScheme*/ }; 
        void WriteToTextWriter(Common::TextWriter &, Enum const &);
    }

    // We do not support IPvFuture, so that is not included.
    namespace UriHostType 
    { 
        enum Enum { None, IPv4, IPv6/*, IPvFuture*/, RegName };
        void WriteToTextWriter(Common::TextWriter &, Enum const &);
    }

    class Uri : public Serialization::FabricSerializable, public ISizeEstimator
    {
        DEFAULT_COPY_CONSTRUCTOR(Uri)
        DEFAULT_COPY_ASSIGNMENT(Uri)

    public:
        // TODO: make constructors protected after unit test class can be made a friend (switched to TAEF)
        Uri(): type_(UriType::AuthorityAbEmpty), hostType_(UriHostType::None), port_(0) { }
        Uri(Uri && other);
        explicit Uri(std::string const & scheme);
        Uri(std::string const & scheme, std::string const & authority);
        Uri(std::string const & scheme, std::string const & authority, std::string const & path);
        Uri(
            std::string const & scheme,
            std::string const & authority,
            std::string const & path,
            std::string const & query,
            std::string const & fragment);

        virtual ~Uri();
        
        bool operator == (Uri const & uri2) const;
        bool operator != (Uri const & uri2) const;
        bool operator < (Uri const & uri2) const;
        Uri & operator = (Uri && other);

        __declspec(property(get=get_Type)) UriType::Enum Type;
        __declspec(property(get=get_Scheme)) std::string const & Scheme;
        __declspec(property(get=get_Authority)) std::string const & Authority;
        __declspec(property(get=get_HostType)) UriHostType::Enum HostType;
        __declspec(property(get=get_Host)) std::string const & Host;
        __declspec(property(get=get_Port)) int Port;
        __declspec(property(get=get_Path)) std::string const & Path;
        __declspec(property(get=get_Query)) std::string const & Query;
        __declspec(property(get=get_Fragment)) std::string const & Fragment;
        __declspec(property(get=get_Segments)) std::vector<std::string> const & Segments;
        __declspec(property(get=get_HasQueryOrFragment)) bool HasQueryOrFragment;

        UriType::Enum get_Type() const { return type_; }
        std::string const & get_Scheme() const { return scheme_; }
        std::string const & get_Authority() const { return authority_; }
        UriHostType::Enum get_HostType() const { return hostType_; }
        std::string const & get_Host() const { return host_; }
        int get_Port() const { return port_; }
        std::string const & get_Path() const { return path_; }
        std::string const & get_Query() const { return query_; }
        std::string const & get_Fragment() const { return fragment_; }
        std::vector<std::string> const & get_Segments() const { return pathSegments_; }
        bool get_HasQueryOrFragment() const { return (!query_.empty() || !fragment_.empty()); }

        int Compare(Uri const & other) const;

        Uri GetTrimQueryAndFragment() const;

        std::string ToString() const;
        void WriteTo(__in Common::TextWriter & w, Common::FormatOptions const &) const;
        void WriteToEtw(uint16 contextSequenceId) const;

        static bool TryParse(std::string const & uriText, __out Uri & uri);
        static bool TryParseAndTraceOnFailure(std::string const & uriText, __out Uri & uri);

        bool IsPrefixOf(Uri const & other) const;

        FABRIC_FIELDS_10(type_, scheme_, authority_, hostType_, host_, port_, path_, query_, fragment_, pathSegments_);

        BEGIN_DYNAMIC_SIZE_ESTIMATION()
            DYNAMIC_SIZE_ESTIMATION_MEMBER(scheme_)
            DYNAMIC_SIZE_ESTIMATION_MEMBER(authority_)
            DYNAMIC_SIZE_ESTIMATION_MEMBER(host_)
            DYNAMIC_SIZE_ESTIMATION_MEMBER(path_)
            DYNAMIC_SIZE_ESTIMATION_MEMBER(query_)
            DYNAMIC_SIZE_ESTIMATION_MEMBER(fragment_)
            DYNAMIC_SIZE_ESTIMATION_MEMBER(pathSegments_)
        END_DYNAMIC_SIZE_ESTIMATION()

        static StringLiteral TraceCategory;

    private:
        class Parser;
        static bool TryParse(std::string const & uriText, bool traceOnFailure, __out Uri & uri);


        std::vector<std::string> GetSegments(std::string const & path);
        void Normalize();
        static std::string FixPathForConstructor(std::string const & path);

        UriType::Enum type_;
        std::string scheme_;
        std::string authority_;
        UriHostType::Enum hostType_;
        std::string host_;
        int port_;
        std::string path_;
        std::string query_;
        std::string fragment_;
        std::vector<std::string> pathSegments_;
    };
}

DEFINE_USER_ARRAY_UTILITY(Common::Uri);

// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class UriBuilder
    {
    public:
        UriBuilder();

        UriBuilder(Uri const & uri);

        void ToUri(Uri & uri);

        __declspec(property(get=get_Scheme, put=set_Scheme)) std::string const & Scheme;
        __declspec(property(get=get_Host, put=set_Host)) std::string const & Host;
        __declspec(property(get=get_Port, put=set_Port)) int Port;
        __declspec(property(get=get_Path, put=set_Path)) std::string const & Path;
        __declspec(property(get=get_Query, put=set_Query)) std::string const & Query;
        __declspec(property(get=get_Fragment, put=set_Fragment)) std::string const & Fragment;

        std::string const & get_Scheme() const { return scheme_; }
        std::string const & get_Host() const { return host_; }
        int get_Port() const { return port_; }
        std::string const & get_Path() const { return path_; }
        std::string const & get_Query() const { return query_; }
        std::string const & get_Fragment() const { return fragment_; }

        UriBuilder & set_Scheme(std::string const & scheme) { scheme_ = scheme; return *this; }
        UriBuilder & set_Host(std::string const & host) { host_ = host; return *this; }
        UriBuilder & set_Port(int port) { port_ = port; return *this; }
        UriBuilder & set_Path(std::string const & path) { path_ = path; return *this; }
        UriBuilder & set_Query(std::string const & query) { query_ = query; return *this; }
        UriBuilder & set_Fragment(std::string const & fragment) { fragment_ = fragment; return *this; }

    private:
        std::string scheme_;
        std::string host_;
        int port_;
        std::string path_;
        std::string query_;
        std::string fragment_;
    };
}

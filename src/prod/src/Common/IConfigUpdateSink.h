// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class IConfigUpdateSink
    {
    public:
        virtual ~IConfigUpdateSink() = 0 {}

        virtual const std::string & GetTraceId() const = 0;
        virtual bool OnUpdate(std::string const & section, std::string const & key) = 0;
        virtual bool CheckUpdate(std::string const & section, std::string const & key, std::string const & value, bool isEncrypted) = 0;
    };

    typedef std::shared_ptr<IConfigUpdateSink> IConfigUpdateSinkSPtr;
    typedef std::weak_ptr<IConfigUpdateSink> IConfigUpdateSinkWPtr;
    typedef std::vector<IConfigUpdateSinkWPtr> IConfigUpdateSinkList;
}

// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Transport
{
    typedef std::map<std::string, std::weak_ptr<IDatagramTransport> > TransportLookupTable;

    class TransportTable
    {
    public:
        TransportTable();

        ~TransportTable();

        void Add(std::string const & name, std::weak_ptr<IDatagramTransport> const & item);

        bool Contains(std::string const & name);

        bool Remove(std::string const & name);

        bool TryGet(std::string const & name, std::weak_ptr<IDatagramTransport> & item);

        size_t Count()
        {
            return this->transportTable_.size();
        }

    private:
        TransportLookupTable transportTable_;
        Common::ExclusiveLock transportTableLock_;
    };
}

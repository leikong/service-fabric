// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Transport
{
    class UnreliableTransportSpecification
    {
        DENY_COPY(UnreliableTransportSpecification);

    public:
        UnreliableTransportSpecification(
            std::string const & name,
            std::string const & source,
            std::string const & destination, 
            std::string const & actionFilter, 
            double probabilityToApply, 
            Common::TimeSpan delay, 
            Common::TimeSpan delaySpan,
            int priority, 
            int applyCount = -1,
            Common::StringMap const & filters = Common::StringMap());

        bool Match(std::string const & source, std::string const & destination, std::string const & action);
        
        bool Match(std::string const & source, std::string const & destination, Transport::Message const & message);

        Common::TimeSpan GetDelay();

        int GetPriority() const { return this->priority_; }

        void WriteTo(Common::TextWriter & w, Common::FormatOptions const &) const;

        std::string const & GetName() const { return name_; }

        static Common::StringLiteral const MatchAll;

    private:
        std::string name_;
        std::string source_;
        std::string destination_;
        std::string action_;
        double probabilityToApply_;
        Common::TimeSpan delay_;
        double delaySpan_;
        int priority_;
        LONG applyCount_;
        bool countActive_;
        Common::Random random_;
        Common::StringMap filters_;
        RWLOCK(UnreliableTransportSpecification, lock_);
    };

    typedef std::unique_ptr<UnreliableTransportSpecification> UnreliableTransportSpecificationUPtr;
}

// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once 

namespace Common
{
    class ExtendedEventMetadata
    {
        DENY_COPY(ExtendedEventMetadata);

    public:
        ExtendedEventMetadata(std::string const& publicEventName, std::string const& category);

        static std::unique_ptr<ExtendedEventMetadata> Create(std::string const& publicEventName, std::string const& category);

        static std::string AddMetadataFields(TraceEvent & traceEvent, size_t & fieldsCount);

        std::string const& GetPublicEventNameField() const
        {
            return publicEventName_;
        }

        std::string const& GetCategoryField() const
        {
            return category_;
        }

        void AppendFields(TraceEventContext & context) const;
        void AppendFields(StringWriter & writer) const;

    private:
        static StringLiteral const& GetMetadataFieldsFormat();

        std::string publicEventName_;
        std::string category_;
    };

}

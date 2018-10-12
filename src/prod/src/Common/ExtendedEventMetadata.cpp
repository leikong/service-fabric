// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

namespace Common
{
    ExtendedEventMetadata::ExtendedEventMetadata(std::string const& publicEventName, std::string const& category)
        : publicEventName_(publicEventName)
        , category_(category)
    {
    }

    std::unique_ptr<ExtendedEventMetadata> ExtendedEventMetadata::Create(std::string const& publicEventName, std::string const& category)
    {
        return std::make_unique<ExtendedEventMetadata>(publicEventName, category);
    }

    std::string ExtendedEventMetadata::AddMetadataFields(TraceEvent & traceEvent, size_t & fieldsCount)
    {
        size_t index = 0;
        traceEvent.AddEventField<std::string>("eventName", index);
        traceEvent.AddEventField<std::string>("category", index);
        // Number of appended fields.
        fieldsCount = index;
        return std::string(GetMetadataFieldsFormat().begin());
    }

    void ExtendedEventMetadata::AppendFields(TraceEventContext & context) const
    {
        context.Write(GetPublicEventNameField());
        context.Write(GetCategoryField());
    }

    void ExtendedEventMetadata::AppendFields(StringWriter & writer) const
    {
        writer.Write(GetMetadataFieldsFormat(), GetPublicEventNameField(), GetCategoryField());
    }

    StringLiteral const& ExtendedEventMetadata::GetMetadataFieldsFormat()
    {
        static StringLiteral const format("EventName: {0} Category: {1} ");
        return format;
    }
}

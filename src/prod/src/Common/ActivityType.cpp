//------------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
//------------------------------------------------------------------------------

#include "stdafx.h"

namespace Common
{
    namespace ActivityType
    {
        void WriteToTextWriter(Common::TextWriter & w, Enum const & val)
        {
            switch (val)
            {
            case Enum::Unknown:
                w << "Unknown"; return;
            case Enum::ClientReportFaultEvent:
                w << "ClientReportFaultEvent"; return;
            case Enum::ServiceReportFaultEvent:
                w << "ServiceReportFaultEvent"; return;
            case Enum::ServicePackageEvent:
                w << "ServicePackageEvent"; return;
            case Enum::ReplicaEvent:
                w << "ReplicaEvent"; return;
            default: 
                Common::Assert::CodingError("Unknown activity type.");
            };
        }

        ENUM_STRUCTURED_TRACE(ActivityType, Unknown, LastValidEnum);
    }
}

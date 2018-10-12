// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"


namespace Common
{
    namespace PerformanceCounterSetInstanceType
    {
        void WriteToTextWriter(Common::TextWriter & w, Enum const & e)
        {
            switch (e)
            {
            case Single : w << "Single"; return;
            case Multiple : w << "Multiple"; return;
            case GlobalAggregate : w << "GlobalAggregate"; return;
            case GlobalAggregateWithHistory : w << "GlobalAggregateWithHistory"; return;
            case MultipleAggregate : w << "MultipleAggregate"; return;
            case InstanceAggregate : w << "InstanceAggregate"; return;
            }

            w << "PerformanceCounterSetInstanceType(" << static_cast<int>(e) << ')';
        }
    }
}

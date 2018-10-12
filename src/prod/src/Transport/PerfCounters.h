// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Transport
{
    class PerfCounters
    {
        DENY_COPY(PerfCounters)

    public:

        BEGIN_COUNTER_SET_DEFINITION(
            "cf1be4cf-e721-48d9-9b76-ae5f9148634a",
            "Transport",
            "Counters for Transport",
            Common::PerformanceCounterSetInstanceType::Multiple)
            COUNTER_DEFINITION(
                1,
                Common::PerformanceCounterType::RawData32,
                "# of Active Callback",
                "Counter for active threadpool callback")
            COUNTER_DEFINITION(
                2,
                Common::PerformanceCounterType::AverageBase,
                "Avg. TCP send size Base",
                "Base Counter for measuring the average TCP send size in bytes",
                noDisplay)
            COUNTER_DEFINITION_WITH_BASE(
                3,
                2,
                Common::PerformanceCounterType::AverageCount64,
                "Avg. TCP send size (bytes)",
                "Counter for measuring the average TCP send size in bytes")
        END_COUNTER_SET_DEFINITION()

        DECLARE_COUNTER_INSTANCE(NumberOfActiveCallbacks)
        DECLARE_COUNTER_INSTANCE(AverageTcpSendSizeBase)
        DECLARE_COUNTER_INSTANCE(AverageTcpSendSize)

        BEGIN_COUNTER_SET_INSTANCE(PerfCounters)
            DEFINE_COUNTER_INSTANCE(
                NumberOfActiveCallbacks,
                1)
                DEFINE_COUNTER_INSTANCE(
                AverageTcpSendSizeBase,
                2)
                DEFINE_COUNTER_INSTANCE(
                AverageTcpSendSize,
                3)
        END_COUNTER_SET_INSTANCE()
    };
}

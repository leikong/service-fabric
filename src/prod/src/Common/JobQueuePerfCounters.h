// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class JobQueuePerfCounters
    {
        DENY_COPY(JobQueuePerfCounters)
    public:
        BEGIN_COUNTER_SET_DEFINITION(
            "3A7DC752-152C-4725-B739-F393793B76A4",
            "Component JobQueue",
            "Counters for JobQueue used by Service Fabric Components",
            PerformanceCounterSetInstanceType::Multiple)
            COUNTER_DEFINITION(
                1,
                PerformanceCounterType::RateOfCountPerSecond64,
                "# Items in the JobQueue",
                "Number of items in the JobQueue")
            COUNTER_DEFINITION(
                2,
                PerformanceCounterType::RateOfCountPerSecond64,
                "# Items/Second failed to be inserted into the JobQueue",
                "Number of items failed to be inserted into the JobQueue per second, because the JobQueue was full")
            COUNTER_DEFINITION(
                3,
                PerformanceCounterType::AverageBase,
                "Avg. time in ms an item spends in the queue/Operation Base",
                "Base counter for measuring the average time each job queue item spends in the queue",
                noDisplay)
            COUNTER_DEFINITION_WITH_BASE(
                4,
                3,
                PerformanceCounterType::AverageCount64,
                "Avg. time in ms an item spends in the queue/Operation",
                "Counter for measuring the average time each job queue item spends in the queue")
            COUNTER_DEFINITION(
                5,
                PerformanceCounterType::RateOfCountPerSecond64,
                "# Items/Second inserted into the job queue",
                "Number of items inserted into the job queue per second")
        END_COUNTER_SET_DEFINITION()

            DECLARE_COUNTER_INSTANCE(NumberOfItems)
            DECLARE_COUNTER_INSTANCE(NumberOfDroppedItems)
			DECLARE_COUNTER_INSTANCE(AverageTimeSpentInQueueBase)
			DECLARE_COUNTER_INSTANCE(AverageTimeSpentInQueue)
            DECLARE_COUNTER_INSTANCE(NumberOfItemsInsertedPerSecond)

        BEGIN_COUNTER_SET_INSTANCE(JobQueuePerfCounters)
            DEFINE_COUNTER_INSTANCE(NumberOfItems,                  1)
            DEFINE_COUNTER_INSTANCE(NumberOfDroppedItems,           2)
			DEFINE_COUNTER_INSTANCE(AverageTimeSpentInQueueBase,    3)
            DEFINE_COUNTER_INSTANCE(AverageTimeSpentInQueue,        4)
            DEFINE_COUNTER_INSTANCE(NumberOfItemsInsertedPerSecond, 5)
        END_COUNTER_SET_INSTANCE()

        static std::shared_ptr<JobQueuePerfCounters> CreateInstance(
            std::string const &jobQueueName,
            std::string const &uniqueId);
    };

    typedef std::shared_ptr<JobQueuePerfCounters> JobQueuePerfCountersSPtr;
}

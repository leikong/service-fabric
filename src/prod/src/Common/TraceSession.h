// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    static const int ProvidersPerSession = 11;

    namespace TraceSessionKind
    {
        enum Enum
        {
            Invalid     = 0,
            Fabric      = 1,
            Lease       = 2,
            AppInfo     = 3,
            Query       = 4,
            Operational = 5,
            SFBDMiniport = 6
        };
    }

    class TraceSession :
        public Common::TextTraceComponent<Common::TraceTaskCodes::Common>
    {
    public:

        DENY_COPY(TraceSession);

        typedef std::shared_ptr<TraceSession> TraceSessionSPtr;
        static TraceSessionSPtr Instance();

        Common::ErrorCode StartTraceSessions();
        Common::ErrorCode StopTraceSessions();

        Common::ErrorCode StartTraceSession(TraceSessionKind::Enum traceSessionKind);
        Common::ErrorCode StopTraceSession(TraceSessionKind::Enum traceSessionKind);

    private:

        struct DataCollectorSetInfo
        {
            TraceSessionKind::Enum TraceSessionKind;
            std::string SessionName;
            std::string TraceFolderName;
            std::string TraceFileName;
            Common::Guid ProviderGuids[ProvidersPerSession];
            UCHAR TraceLevel;
            ULONGLONG Keyword[ProvidersPerSession];
            ULONGLONG KeywordAll[ProvidersPerSession];
            ULONG BufferSizeInKB;
            ULONG FileSizeInMB;
            ULONG FlushIntervalInSeconds;
        };

        TraceSession();

        unique_ptr<EVENT_TRACE_PROPERTIES, decltype(free)*> GetSessionProperties(DataCollectorSetInfo const & info, uint additionalBytes);

        HRESULT StartTraceSession(DataCollectorSetInfo const & info);
        HRESULT StopTraceSession(DataCollectorSetInfo const & info);

        HRESULT TraceResult(
            std::string const & method,
            HRESULT exitCode);

        HRESULT TraceSession::Prepare(DataCollectorSetInfo const & info, BOOLEAN *runningLatestStatus);

        HRESULT TraceSession::GetTraceSessionStatus(DataCollectorSetInfo const & info, BOOLEAN *runningLatestStatus);

        std::string GetSessionName(std::string const & sessionName);

        string TraceSession::GetTraceFolder(DataCollectorSetInfo const & info);

        int TraceSession::TraceSessionFilePathCompare(std::string const & a, std::string const & b);

    private:
        static const ULONG DefaultTraceBufferSizeInKB = 128;
        static const ULONG DefaultTraceFileSizeInMB = 64;
        static const ULONG DefaultTraceFlushIntervalInSeconds = 60;
        static const ULONG DefaultAppInfoTraceBufferSizeInKB = 8;
        static const ULONG DefaultAppInfoTraceFlushIntervalInSeconds = 60;
        static const ULONG DefaultCircularFileSizeMultiplier = 4;
        static const ULONG DefaultAdditionalBytesForName = 1024;

        // Trace Provider GUIDS
        static Common::Global<Common::Guid> LeaseDriverGuid;
        static Common::Global<Common::Guid> SFBDMiniportDriverGuid;
        static Common::Global<Common::Guid> KTLGuid;
        static Common::Global<Common::Guid> FabricGuid;
        static Common::Global<Common::Guid> MSFIGuid;
        static Common::Global<Common::Guid> ProgrammingModel_ActorGuid;
        static Common::Global<Common::Guid> SF_ClientLib_HttpGuid;
        static Common::Global<Common::Guid> SFPOSCoordinatorServiceGuid;
        static Common::Global<Common::Guid> SFPOSNodeAgentServiceGuid;
        static Common::Global<Common::Guid> SFPOSNodeAgentNTServiceGuid;
        static Common::Global<Common::Guid> SFPOSNodeAgentSFUtilityGuid;
        static Common::Global<Common::Guid> ProgrammingModel_ServicesGuid;
        static Common::Global<Common::Guid> ReliableCollectionGuid;
        static Common::Global<Common::Guid> AzureFilesVolumeDriverGuid;

        static Common::GlobalString FabricTraceFileName;
        static Common::GlobalString SFBDMiniportTraceFileName;
        static Common::GlobalString LeaseTraceFileName;
        static Common::GlobalString AppInfoTraceFileName;
        static Common::GlobalString AppInfoTraceFolderName;
        static Common::GlobalString QueryTraceFileName;
        static Common::GlobalString QueryTraceFolderName;
        static Common::GlobalString OperationalTraceFileName;
        static Common::GlobalString OperationalTraceFolderName;

        static Common::GlobalString FabricTraceSessionName;
        static Common::GlobalString LeaseTraceSessionName;
        static Common::GlobalString SFBDMiniportTraceSessionName;
        static Common::GlobalString AppInfoTraceSessionName;
        static Common::GlobalString QueryTraceSessionName;
        static Common::GlobalString FabricCountersSessionName;
        static Common::GlobalString OperationalTraceSessionName;

        static Common::GlobalString FabricDataRoot;

        static Common::GlobalString CurrentExeVersion;

        std::vector<DataCollectorSetInfo> dataCollectorSets;
        std::string testKeyword_;
        std::string tracePath_;
        BOOLEAN enableCircularTraceSession_;
    };
}

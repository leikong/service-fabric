// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace ServiceModel
{
    // "Test mutable constants" are used in product code like regular static const members
    // (i.e. Constants::MemberName) and are not exposed in any way through config.
    // However, you can change their values for testing purposes using Constants::Test_SetMemberName().
    //
#define DECLARE_TEST_MUTABLE_CONSTANT(type, name) \
private: \
    static type name##_; \
public: \
    typedef struct name##_t \
    { \
    inline operator type() { return name##_; } \
    void WriteTo(Common::TextWriter &, Common::FormatOptions const &) const; \
} name##_t; \
    static name##_t name; \
    static void Test_Set##name(type const & value) { name##_ = value; } \

#define DEFINE_TEST_MUTABLE_CONSTANT(type, name, value) \
    void Constants::name##_t::WriteTo(Common::TextWriter & w, Common::FormatOptions const &) const { w << Constants::name##_; } \
    type Constants::name##_(value); \
    Constants::name##_t Constants::name = Constants::name##_t(); \

    class Constants
    {
    public:

        //
        // Naming Service
        //
        static __int64 const UninitializedVersion;

        //
        // JSON serialization - Field Names
        //
        static Common::StringLiteral const Name;
        static Common::StringLiteral const DeploymentName;
        static Common::StringLiteral const HealthStateFilter;
        static Common::StringLiteral const TypeName;
        static Common::StringLiteral const TypeVersion;
        static Common::StringLiteral const ParameterList;
        static Common::StringLiteral const DefaultParameterList;
        static Common::StringLiteral const ApplicationHealthPolicy;
        static Common::StringLiteral const ApplicationHealthPolicyMap;
        static Common::StringLiteral const ApplicationHealthPolicies;
        static Common::StringLiteral const instances;
        static Common::StringLiteral const instanceNames;
        static Common::StringLiteral const MaxPercentServicesUnhealthy;
        static Common::StringLiteral const MaxPercentDeployedApplicationsUnhealthy;
        static Common::StringLiteral const Value;
        static Common::StringLiteral const Status;
        static Common::StringLiteral const Parameters;
        static Common::StringLiteral const HealthState;
        static Common::StringLiteral const AggregatedHealthState;
        static Common::StringLiteral const PartitionStatus;
        static Common::StringLiteral const LastQuorumLossDurationInSeconds;
        static Common::StringLiteral const ServiceManifestVersion;
        static Common::StringLiteral const TargetApplicationTypeVersion;
        static Common::StringLiteral const TargetApplicationParameterList;
        static Common::StringLiteral const UpgradeKind;
        static Common::StringLiteral const UpgradeDescription;
        static Common::StringLiteral const UpdateDescription;
        static Common::StringLiteral const RollingUpgradeMode;
        static Common::StringLiteral const UpgradeReplicaSetCheckTimeoutInSeconds;
        static Common::StringLiteral const ReplicaSetCheckTimeoutInMilliseconds;
        static Common::StringLiteral const ForceRestart;
        static Common::StringLiteral const MonitoringPolicy;
        static Common::StringLiteral const ClusterHealthPolicy;
        static Common::StringLiteral const EnableDeltaHealthEvaluation;
        static Common::StringLiteral const ClusterUpgradeHealthPolicy;
        static Common::StringLiteral const MaxPercentNodesUnhealthyPerUpgradeDomain;
        static Common::StringLiteral const MaxPercentDeployedApplicationsUnhealthyPerUpgradeDomain;
        static Common::StringLiteral const DefaultApplicationHealthAggregationPolicy;
        static Common::StringLiteral const ApplicationHealthAggregationPolicyOverrides;
        static Common::StringLiteral const MaxPercentNodesUnhealthy;
        static Common::StringLiteral const MaxPercentDeltaUnhealthyNodes;
        static Common::StringLiteral const MaxPercentUpgradeDomainDeltaUnhealthyNodes;
        static Common::StringLiteral const EntryPointLocation;
        static Common::StringLiteral const ProcessId;
        static Common::StringLiteral const HostProcessId;
        static Common::StringLiteral const LastExitCode;
        static Common::StringLiteral const LastActivationTime;
        static Common::StringLiteral const LastExitTime;
        static Common::StringLiteral const LastSuccessfulActivationTime;
        static Common::StringLiteral const LastSuccessfulExitTime;
        static Common::StringLiteral const ActivationFailureCount;
        static Common::StringLiteral const ContinuousActivationFailureCount;
        static Common::StringLiteral const ExitFailureCount;
        static Common::StringLiteral const ContinuousExitFailureCount;
        static Common::StringLiteral const ActivationCount;
        static Common::StringLiteral const ExitCount;
        static Common::StringLiteral const SetupEntryPoint;
        static Common::StringLiteral const MainEntryPoint;
        static Common::StringLiteral const HasSetupEntryPoint;
        static Common::StringLiteral const ServiceKind;
        static Common::StringLiteral const ReplicaId;
        static Common::StringLiteral const ReplicaOrInstanceId;
        static Common::StringLiteral const ReplicaRole;
        static Common::StringLiteral const PreviousConfigurationRole;
        static Common::StringLiteral const InstanceId;
        static Common::StringLiteral const ReplicaStatus;
        static Common::StringLiteral const ReplicaState;
        static Common::StringLiteral const LastInBuildDurationInSeconds;
        static Common::StringLiteral const PartitionId;
        static Common::StringLiteral const ActivationId;
        static Common::StringLiteral const ServiceName;
        static Common::StringLiteral const Address;
        static Common::StringLiteral const CodePackageName;
        static Common::StringLiteral const ServiceTypeName;
        static Common::StringLiteral const ServiceGroupMemberDescription;
        static Common::StringLiteral const IsServiceGroup;
        static Common::StringLiteral const CodeVersion;
        static Common::StringLiteral const ConfigVersion;
        static Common::StringLiteral const Details;
        static Common::StringLiteral const RunFrequencyInterval;
        static Common::StringLiteral const ConsiderWarningAsError;
        static Common::StringLiteral const IgnoreExpiredEvents;
        static Common::StringLiteral const ApplicationName;
        static Common::StringLiteral const InitializationData;
        static Common::StringLiteral const PartitionDescription;
        static Common::StringLiteral const TargetReplicaSetSize;
        static Common::StringLiteral const MinReplicaSetSize;
        static Common::StringLiteral const HasPersistedState;
        static Common::StringLiteral const InstanceCount;
        static Common::StringLiteral const PlacementConstraints;
        static Common::StringLiteral const CorrelationScheme;
        static Common::StringLiteral const ServiceLoadMetrics;
        static Common::StringLiteral const DefaultMoveCost;
        static Common::StringLiteral const IsDefaultMoveCostSpecified;
        static Common::StringLiteral const ServicePackageActivationMode;
        static Common::StringLiteral const ServicePlacementPolicies;
        static Common::StringLiteral const Flags;
        static Common::StringLiteral const ReplicaRestartWaitDurationSeconds;
        static Common::StringLiteral const QuorumLossWaitDurationSeconds;
        static Common::StringLiteral const StandByReplicaKeepDurationSeconds;
        static Common::StringLiteral const ReplicaRestartWaitDurationInMilliseconds;
        static Common::StringLiteral const QuorumLossWaitDurationInMilliseconds;
        static Common::StringLiteral const StandByReplicaKeepDurationInMilliseconds;
        static Common::StringLiteral const Id;
        static Common::StringLiteral const IpAddressOrFQDN;
        static Common::StringLiteral const Type;
        static Common::StringLiteral const Version;
        static Common::StringLiteral const NodeStatus;
        static Common::StringLiteral const NodeDeactivationIntent;
        static Common::StringLiteral const NodeDeactivationStatus;
        static Common::StringLiteral const NodeDeactivationTask;
        static Common::StringLiteral const NodeDeactivationTaskId;
        static Common::StringLiteral const NodeDeactivationTaskType;
        static Common::StringLiteral const NodeDeactivationTaskIdPrefixRM;
        static Common::StringLiteral const NodeDeactivationTaskIdPrefixWindowsAzure;
        static Common::StringLiteral const NodeDeactivationInfo;
        static Common::StringLiteral const NodeUpTimeInSeconds;
        static Common::StringLiteral const NodeDownTimeInSeconds;
        static Common::StringLiteral const NodeUpAt;
        static Common::StringLiteral const NodeDownAt;
        static Common::StringLiteral const UpgradeDomain;
        static Common::StringLiteral const FaultDomain;
        static Common::StringLiteral const PartitionScheme;
        static Common::StringLiteral const Count;
        static Common::StringLiteral const Names;
        static Common::StringLiteral const LowKey;
        static Common::StringLiteral const HighKey;
        static Common::StringLiteral const MaxPercentReplicasUnhealthy;
        static Common::StringLiteral const Scheme;
        static Common::StringLiteral const Weight;
        static Common::StringLiteral const PrimaryDefaultLoad;
        static Common::StringLiteral const SecondaryDefaultLoad;
        static Common::StringLiteral const DefaultLoad;
        static Common::StringLiteral const FailureAction;
        static Common::StringLiteral const HealthCheckWaitDurationInMilliseconds;
        static Common::StringLiteral const HealthCheckStableDurationInMilliseconds;
        static Common::StringLiteral const HealthCheckRetryTimeoutInMilliseconds;
        static Common::StringLiteral const UpgradeTimeoutInMilliseconds;
        static Common::StringLiteral const UpgradeDomainTimeoutInMilliseconds;
        static Common::StringLiteral const UpgradeDuration;
        static Common::StringLiteral const UpgradeDurationInMilliseconds;
        static Common::StringLiteral const CurrentUpgradeDomainDuration;
        static Common::StringLiteral const UpgradeDomainDurationInMilliseconds;
        static Common::StringLiteral const MaxPercentPartitionsUnhealthy;
        static Common::StringLiteral const RepartitionDescription;
        static Common::StringLiteral const PartitionKind;
        static Common::StringLiteral const NamesToAdd;
        static Common::StringLiteral const NamesToRemove;
        static Common::StringLiteral const ServicePartitionKind;
        static Common::StringLiteral const PartitionInformation;
        static Common::StringLiteral const InQuorumLoss;
        static Common::StringLiteral const DomainName;
        static Common::StringLiteral const ManifestVersion;
        static Common::StringLiteral const NodeName;
        static Common::StringLiteral const NodeId;
        static Common::StringLiteral const NodeInstanceId;
        static Common::StringLiteral const Content;
        static Common::StringLiteral const UriPath;
        static Common::StringLiteral const HttpVerb;
        static Common::StringLiteral const HttpStatus;
        static Common::StringLiteral const HttpRequestBody;
        static Common::StringLiteral const HttpResponseBody;
        static Common::StringLiteral const HttpContentType;
        static Common::StringLiteral const HttpContentEncoding;
        static Common::StringLiteral const CreateFabricDump;
        static Common::StringLiteral const IsStateful;
        static Common::StringLiteral const UseImplicitHost;
        static Common::StringLiteral const UseImplicitFactory;
        static Common::StringLiteral const UseServiceFabricReplicatedStore;
        static Common::StringLiteral const Extensions;
        static Common::StringLiteral const LoadMetrics;
        static Common::StringLiteral const ServiceTypeDescription;
        static Common::StringLiteral const ServiceGroupTypeDescription;
        static Common::StringLiteral const ServiceGroupTypeMemberDescription;
        static Common::StringLiteral const IsSeedNode;
        static Common::StringLiteral const ReplicaRestartWaitDurationInMilliSeconds;
        static Common::StringLiteral const RunAsUserName;
        static Common::StringLiteral const CodePackageEntryPointStatistics;
        static Common::StringLiteral const Manifest;
        static Common::StringLiteral const ServiceManifestName;
        static Common::StringLiteral const ServicePackageActivationId;
        static Common::StringLiteral const HostType;
        static Common::StringLiteral const HostIsolationMode;
        static Common::StringLiteral const ApplicationTypeBuildPath;
        static Common::StringLiteral const ApplicationPackageDownloadUri;
        static Common::StringLiteral const ApplicationPackageCleanupPolicy;
        static Common::StringLiteral const Async;
        static Common::StringLiteral const NextUpgradeDomain;
        static Common::StringLiteral const State;
        static Common::StringLiteral const CodeFilePath;
        static Common::StringLiteral const ClusterManifestFilePath;
        static Common::StringLiteral const DeactivationIntent;
        static Common::StringLiteral const ApplicationTypeVersion;
        static Common::StringLiteral const UpgradeDomains;
        static Common::StringLiteral const UpgradeState;
        static Common::StringLiteral const ProgressStatus;
        static Common::StringLiteral const CodePackageInstanceId;
        static Common::StringLiteral const UpgradeDomainName;
        static Common::StringLiteral const MaxPercentUnhealthyServices;
        static Common::StringLiteral const MaxPercentUnhealthyPartitionsPerService;
        static Common::StringLiteral const MaxPercentUnhealthyReplicasPerPartition;
        static Common::StringLiteral const MaxPercentUnhealthyDeployedApplications;
        static Common::StringLiteral const DefaultServiceTypeHealthPolicy;
        static Common::StringLiteral const ServiceTypeHealthPolicyMap;
        static Common::StringLiteral const MaxPercentUnhealthyNodes;
        static Common::StringLiteral const MaxPercentUnhealthyApplications;
        static Common::StringLiteral const TotalCount;
        static Common::StringLiteral const BaselineTotalCount;
        static Common::StringLiteral const BaselineErrorCount;
        static Common::StringLiteral const SourceId;
        static Common::StringLiteral const Property;
        static Common::StringLiteral const Description;
        static Common::StringLiteral const SequenceNumber;
        static Common::StringLiteral const RemoveWhenExpired;
        static Common::StringLiteral const TimeToLiveInMs;
        static Common::StringLiteral const SourceUtcTimestamp;
        static Common::StringLiteral const LastModifiedUtcTimestamp;
        static Common::StringLiteral const LastOkTransitionAt;
        static Common::StringLiteral const LastWarningTransitionAt;
        static Common::StringLiteral const LastErrorTransitionAt;
        static Common::StringLiteral const IsExpired;
        static Common::StringLiteral const HealthEvents;
        static Common::StringLiteral const HealthEvent;
        static Common::StringLiteral const OkCount;
        static Common::StringLiteral const WarningCount;
        static Common::StringLiteral const ErrorCount;
        static Common::StringLiteral const EntityKind;
        static Common::StringLiteral const HealthStateCount;
        static Common::StringLiteral const HealthStatistics;
        static Common::StringLiteral const HealthStateCountList;
        static Common::StringLiteral const ExcludeHealthStatistics;
        static Common::StringLiteral const IncludeSystemApplicationHealthStatistics;
        static Common::StringLiteral const ServiceAggregatedHealthStates;
        static Common::StringLiteral const DeployedApplicationsAggregatedHealthStates;
        static Common::StringLiteral const PartitionAggregatedHealthState;
        static Common::StringLiteral const ReplicaAggregatedHealthState;
        static Common::StringLiteral const DeployedServicePackageAggregatedHealthState;
        static Common::StringLiteral const NextActivationTime;
        static Common::StringLiteral const NodeAggregatedHealthStates;
        static Common::StringLiteral const ApplicationAggregatedHealthStates;
        static Common::StringLiteral const SystemApplicationAggregatedHealthState;
        static Common::StringLiteral const ServiceManifestNameFilter;
        static Common::StringLiteral const ServicePackageActivationIdFilter;
        static Common::StringLiteral const ApplicationNameFilter;
        static Common::StringLiteral const ApplicationTypeNameFilter;
        static Common::StringLiteral const ApplicationDefinitionKind;
        static Common::StringLiteral const ApplicationTypeDefinitionKind;
        static Common::StringLiteral const DeployedServicePackagesFilter;
        static Common::StringLiteral const NodeNameFilter;
        static Common::StringLiteral const ReplicaOrInstanceIdFilter;
        static Common::StringLiteral const PartitionIdFilter;
        static Common::StringLiteral const ReplicasFilter;
        static Common::StringLiteral const PartitionsFilter;
        static Common::StringLiteral const ServiceNameFilter;
        static Common::StringLiteral const DeployedApplicationsFilter;
        static Common::StringLiteral const ServicesFilter;
        static Common::StringLiteral const ApplicationsFilter;
        static Common::StringLiteral const HealthPolicy;
        static Common::StringLiteral const NodesFilter;
        static Common::StringLiteral const DeployedServicePackageFilters;
        static Common::StringLiteral const ReplicaFilters;
        static Common::StringLiteral const PartitionFilters;
        static Common::StringLiteral const DeployedApplicationFilters;
        static Common::StringLiteral const ServiceFilters;
        static Common::StringLiteral const ApplicationFilters;
        static Common::StringLiteral const NodeFilters;
        static Common::StringLiteral const ReplicaHealthStateChunks;
        static Common::StringLiteral const PartitionHealthStateChunks;
        static Common::StringLiteral const DeployedServicePackageHealthStateChunks;
        static Common::StringLiteral const DeployedApplicationHealthStateChunks;
        static Common::StringLiteral const ServiceHealthStateChunks;
        static Common::StringLiteral const ApplicationHealthStateChunks;
        static Common::StringLiteral const NodeHealthStateChunks;
        static Common::StringLiteral const Metadata;
        static Common::StringLiteral const ServiceDnsName;
        static Common::StringLiteral const ScalingPolicies;
        static Common::StringLiteral const Key;
        static Common::StringLiteral const LastBalancingStartTimeUtc;
        static Common::StringLiteral const LastBalancingEndTimeUtc;
        static Common::StringLiteral const LoadMetricInformation;
        static Common::StringLiteral const IsBalancedBefore;
        static Common::StringLiteral const IsBalancedAfter;
        static Common::StringLiteral const DeviationBefore;
        static Common::StringLiteral const DeviationAfter;
        static Common::StringLiteral const ActivityThreshold;
        static Common::StringLiteral const BalancingThreshold;
        static Common::StringLiteral const ClusterCapacity;
        static Common::StringLiteral const ClusterLoad;
        static Common::StringLiteral const RemainingUnbufferedCapacity;
        static Common::StringLiteral const NodeBufferPercentage;
        static Common::StringLiteral const BufferedCapacity;
        static Common::StringLiteral const RemainingBufferedCapacity;
        static Common::StringLiteral const IsClusterCapacityViolation;
        static Common::StringLiteral const Action;
        static Common::StringLiteral const NodeCapacity;
        static Common::StringLiteral const NodeLoad;
        static Common::StringLiteral const NodeRemainingCapacity;
        static Common::StringLiteral const IsCapacityViolation;
        static Common::StringLiteral const NodeBufferedCapacity;
        static Common::StringLiteral const NodeRemainingBufferedCapacity;
        static Common::StringLiteral const CurrentNodeLoad;
        static Common::StringLiteral const NodeCapacityRemaining;
        static Common::StringLiteral const BufferedNodeCapacityRemaining;
        static Common::StringLiteral const NodeLoadMetricInformation;
        static Common::StringLiteral const LoadMetricReports;
        static Common::StringLiteral const PrimaryLoadMetricReports;
        static Common::StringLiteral const SecondaryLoadMetricReports;
        static Common::StringLiteral const NodeCapacities;
        static Common::StringLiteral const ServiceStatus;
        static Common::StringLiteral const Kind;
        static Common::StringLiteral const KindLowerCase;
        static Common::StringLiteral const UnhealthyEvent;
        static Common::StringLiteral const unhealthyEvaluation;
        static Common::StringLiteral const UnhealthyEvaluations;
        static Common::StringLiteral const ApplicationUnhealthyEvaluations;
        static Common::StringLiteral const HealthEvaluation;
        static Common::StringLiteral const UpgradePhase;
        static Common::StringLiteral const SafetyCheckKind;
        static Common::StringLiteral const SafetyCheck;
        static Common::StringLiteral const PendingSafetyChecks;
        static Common::StringLiteral const NodeUpgradeProgressList;
        static Common::StringLiteral const CurrentUpgradeDomainProgress;
        static Common::StringLiteral const StartTimestampUtc;
        static Common::StringLiteral const FailureTimestampUtc;
        static Common::StringLiteral const FailureReason;
        static Common::StringLiteral const UpgradeDomainProgressAtFailure;
        static Common::StringLiteral const UpgradeStatusDetails;
        static Common::StringLiteral const ApplicationUpgradeStatusDetails;
        static Common::StringLiteral const WorkDirectory;
        static Common::StringLiteral const TempDirectory;
        static Common::StringLiteral const LogDirectory;
        static Common::StringLiteral const CurrentServiceOperation;
        static Common::StringLiteral const CurrentServiceOperationStartTimeUtc;
        static Common::StringLiteral const ReportedLoad;
        static Common::StringLiteral const CurrentValue;
        static Common::StringLiteral const LastReportedUtc;
        static Common::StringLiteral const CurrentReplicatorOperation;
        static Common::StringLiteral const CurrentReplicatorOperationStartTimeUtc;
        static Common::StringLiteral const ReadStatus;
        static Common::StringLiteral const WriteStatus;
        static Common::StringLiteral const ApplicationTypeName;
        static Common::StringLiteral const CodePackageIds;
        static Common::StringLiteral const ConfigPackageIds;
        static Common::StringLiteral const DataPackageIds;
        static Common::StringLiteral const ServicePackageInstanceId;
        static Common::StringLiteral const ReplicaInstanceId;
        static Common::StringLiteral const PackageSharingScope;
        static Common::StringLiteral const SharedPackageName;
        static Common::StringLiteral const PackageSharingPolicy;
        static Common::StringLiteral const FMVersion;
        static Common::StringLiteral const StoreVersion;
        static Common::StringLiteral const GenerationNumber;
        static Common::StringLiteral const Generation;
        static Common::StringLiteral const Endpoints;
        static Common::StringLiteral const PreviousRspVersion;
        static Common::StringLiteral const CurrentConfigurationEpoch;
        static Common::StringLiteral const PrimaryEpoch;
        static Common::StringLiteral const ConfigurationVersion;
        static Common::StringLiteral const DataLossVersion;
        static Common::StringLiteral const ContinuationToken;
        static Common::StringLiteral const MaxResults;
        static Common::StringLiteral const Items;
        static Common::StringLiteral const ApplicationTypeHealthPolicyMap;
        static Common::StringLiteral const ApplicationCapacity;
        static Common::StringLiteral const MinimumNodes;
        static Common::StringLiteral const MaximumNodes;
        static Common::StringLiteral const ApplicationMetrics;
        static Common::StringLiteral const ReservationCapacity;
        static Common::StringLiteral const MaximumCapacity;
        static Common::StringLiteral const TotalApplicationCapacity;
        static Common::StringLiteral const RemoveApplicationCapacity;
        static Common::StringLiteral const ApplicationLoad;
        static Common::StringLiteral const ApplicationLoadMetricInformation;
        static Common::StringLiteral const NodeCount;
        static Common::StringLiteral const IsStopped;
        static Common::StringLiteral const IsConsistent;
        static Common::StringLiteral const SubNames;
        static Common::StringLiteral const PropertyName;
        static Common::StringLiteral const TypeId;
        static Common::StringLiteral const CustomTypeId;
        static Common::StringLiteral const Parent;
        static Common::StringLiteral const SizeInBytes;
        static Common::StringLiteral const Properties;
        static Common::StringLiteral const Data;
        static Common::StringLiteral const IncludeValue;
        static Common::StringLiteral const Exists;
        static Common::StringLiteral const Operations;
        static Common::StringLiteral const ErrorMessage;
        static Common::StringLiteral const OperationIndex;

        static Common::StringLiteral const QueryPagingDescription;

        static Common::StringLiteral const ChaosEvent;
        static Common::StringLiteral const ChaosEventsFilter;
        static Common::StringLiteral const ChaosParameters;
        static Common::StringLiteral const ClientType;
        static Common::StringLiteral const Reason;
        static Common::StringLiteral const Faults;
        static Common::StringLiteral const MaxClusterStabilizationTimeoutInSeconds;
        static Common::StringLiteral const MaxConcurrentFaults;
        static Common::StringLiteral const WaitTimeBetweenIterationsInSeconds;
        static Common::StringLiteral const WaitTimeBetweenFaultsInSeconds;
        static Common::StringLiteral const TimeToRunInSeconds;
        static Common::StringLiteral const EnableMoveReplicaFaults;
        static Common::StringLiteral const Context;
        static Common::StringLiteral const Map;
        static Common::StringLiteral const TimeStampUtc;
        static Common::StringLiteral const NodeTypeInclusionList;
        static Common::StringLiteral const ApplicationInclusionList;
        static Common::StringLiteral const ChaosTargetFilter;
        static Common::StringLiteral const Schedule;
        static Common::StringLiteral const ChaosStatus;
        static Common::StringLiteral const ChaosScheduleStatus;
        static Common::StringLiteral const StartDate;
        static Common::StringLiteral const ExpiryDate;
        static Common::StringLiteral const StartTime;
        static Common::StringLiteral const EndTime;
        static Common::StringLiteral const ChaosParametersMap;
        static Common::StringLiteral const Jobs;
        static Common::StringLiteral const Days;
        static Common::StringLiteral const Times;
        static Common::StringLiteral const Hour;
        static Common::StringLiteral const Minute;
        static Common::StringLiteral const Sunday;
        static Common::StringLiteral const Monday;
        static Common::StringLiteral const Tuesday;
        static Common::StringLiteral const Wednesday;
        static Common::StringLiteral const Thursday;
        static Common::StringLiteral const Friday;
        static Common::StringLiteral const Saturday;

        static Common::StringLiteral const Scope;
        static Common::StringLiteral const TaskId;
        static Common::StringLiteral const Target;
        static Common::StringLiteral const Executor;
        static Common::StringLiteral const ExecutorData;
        static Common::StringLiteral const ResultStatus;
        static Common::StringLiteral const ResultCode;
        static Common::StringLiteral const ResultDetails;
        static Common::StringLiteral const History;
        static Common::StringLiteral const CreatedUtcTimestamp;
        static Common::StringLiteral const ClaimedUtcTimestamp;
        static Common::StringLiteral const PreparingUtcTimestamp;
        static Common::StringLiteral const ApprovedUtcTimestamp;
        static Common::StringLiteral const ExecutingUtcTimestamp;
        static Common::StringLiteral const RestoringUtcTimestamp;
        static Common::StringLiteral const CompletedUtcTimestamp;
        static Common::StringLiteral const PreparingHealthCheckStartUtcTimestamp;
        static Common::StringLiteral const PreparingHealthCheckEndUtcTimestamp;
        static Common::StringLiteral const RestoringHealthCheckStartUtcTimestamp;
        static Common::StringLiteral const RestoringHealthCheckEndUtcTimestamp;
        static Common::StringLiteral const PreparingHealthCheckState;
        static Common::StringLiteral const RestoringHealthCheckState;
        static Common::StringLiteral const PerformPreparingHealthCheck;
        static Common::StringLiteral const PerformRestoringHealthCheck;
        static Common::StringLiteral const Impact;
        static Common::StringLiteral const ImpactLevel;
        static Common::StringLiteral const NodeImpactList;
        static Common::StringLiteral const NodeNames;
        static Common::StringLiteral const RequestAbort;
        static Common::StringLiteral const MinNodeLoadValue;
        static Common::StringLiteral const MinNodeLoadId;
        static Common::StringLiteral const MaxNodeLoadValue;
        static Common::StringLiteral const MaxNodeLoadId;
        static Common::StringLiteral const CurrentClusterLoad;
        static Common::StringLiteral const BufferedClusterCapacityRemaining;
        static Common::StringLiteral const ClusterCapacityRemaining;
        static Common::StringLiteral const MaximumNodeLoad;
        static Common::StringLiteral const MinimumNodeLoad;
        static Common::StringLiteral const OnlyQueryPrimaries;
        static Common::StringLiteral const UnplacedReplicaDetails;
        static Common::StringLiteral const Error;
        static Common::StringLiteral const Code;
        static Common::StringLiteral const Message;
        static Common::StringLiteral const ReconfigurationPhase;
        static Common::StringLiteral const ReconfigurationType;
        static Common::StringLiteral const ReconfigurationStartTimeUtc;
        static Common::StringLiteral const ReconfigurationInformation;
        static Common::StringLiteral const DeployedServiceReplica;
        static Common::StringLiteral const DeployedServiceReplicaInstance;

        //Auto scaling
        static Common::StringLiteral const MetricName;
        static Common::StringLiteral const MinInstanceCount;
        static Common::StringLiteral const MaxInstanceCount;
        static Common::StringLiteral const MaxPartitionCount;
        static Common::StringLiteral const MinPartitionCount;
        static Common::StringLiteral const LowerLoadThreshold;
        static Common::StringLiteral const UpperLoadThreshold;
        static Common::StringLiteral const UseOnlyPrimaryLoad;
        static Common::StringLiteral const ScaleIntervalInSeconds;
        static Common::StringLiteral const ScaleIncrement;
        static Common::StringLiteral const ScalingTrigger;
        static Common::StringLiteral const ScalingMechanism;

        // Replicator Query Specific
        static Common::StringLiteral const ReplicatorStatus;

        static Common::StringLiteral const PrimaryReplicatorStatus;

        static Common::StringLiteral const SecondaryReplicatorStatus;
        static Common::StringLiteral const CopyQueueStatus;
        static Common::StringLiteral const LastReplicationOperationReceivedTimeUtc;
        static Common::StringLiteral const LastCopyOperationReceivedTimeUtc;
        static Common::StringLiteral const LastAcknowledgementSentTimeUtc;

        static Common::StringLiteral const RemoteReplicators;
        static Common::StringLiteral const LastReceivedReplicationSequenceNumber;
        static Common::StringLiteral const LastAppliedReplicationSequenceNumber;
        static Common::StringLiteral const IsInBuild;
        static Common::StringLiteral const LastReceivedCopySequenceNumber;
        static Common::StringLiteral const LastAppliedCopySequenceNumber;
        static Common::StringLiteral const LastAcknowledgementProcessedTimeUtc;

        static Common::StringLiteral const RemoteReplicatorAckStatus;
        static Common::StringLiteral const AverageReceiveDuration;
        static Common::StringLiteral const AverageApplyDuration;
        static Common::StringLiteral const NotReceivedCount;
        static Common::StringLiteral const ReceivedAndNotAppliedCount;
        static Common::StringLiteral const CopyStreamAcknowledgementDetail;
        static Common::StringLiteral const ReplicationStreamAcknowledgementDetail;

        static Common::StringLiteral const ReplicationQueueStatus;
        static Common::StringLiteral const QueueUtilizationPercentage;
        static Common::StringLiteral const FirstSequenceNumber;
        static Common::StringLiteral const CompletedSequenceNumber;
        static Common::StringLiteral const CommittedSequenceNumber;
        static Common::StringLiteral const LastSequenceNumber;
        static Common::StringLiteral const QueueMemorySize;

        static Common::StringLiteral const VersionNumber;
        static Common::StringLiteral const EpochDataLossNumber;
        static Common::StringLiteral const EpochConfigurationNumber;
        static Common::StringLiteral const StoreFiles;
        static Common::StringLiteral const StoreFolders;
        static Common::StringLiteral const StoreRelativePath;
        static Common::StringLiteral const FileCount;
        static Common::StringLiteral const FileSize;
        static Common::StringLiteral const FileVersion;
        static Common::StringLiteral const ModifiedDate;
        static Common::StringLiteral const RemoteLocation;
        static Common::StringLiteral const RemoteSource;
        static Common::StringLiteral const RemoteDestination;
        static Common::StringLiteral const SkipFiles;
        static Common::StringLiteral const CopyFlag;
        static Common::StringLiteral const CheckMarkFile;
        static Common::StringLiteral const StartPosition;
        static Common::StringLiteral const EndPosition;
        static Common::StringLiteral const SessionId;
        static Common::StringLiteral const ExpectedRanges;
        static Common::StringLiteral const UploadSessions;
        static Common::StringLiteral const IsRecursive;
        static Common::StringLiteral const ChunkContent;

        static Common::GlobalString EventSystemSourcePrefix;

        static Common::StringLiteral const Port;
        static Common::StringLiteral const ClusterConnectionPort;

        static Common::StringLiteral const ApplicationIds;

        static Common::StringLiteral const TestCommandState;
        static Common::StringLiteral const TestCommandType;

        //
        // ServiceTypes
        //
        static Common::GlobalString InfrastructureServicePrimaryCountName;
        static Common::GlobalString InfrastructureServiceReplicaCountName;

        //
        // Token validation service action.
        //
        static Common::GlobalString const ValidateTokenAction;

        //
        // Health reporting.
        //

        // Sources
        static Common::GlobalString const HealthReportFMMSource;
        static Common::GlobalString const HealthReportFMSource;
        static Common::GlobalString const HealthReportPLBSource;
        static Common::GlobalString const HealthReportCRMSource;
        static Common::GlobalString const HealthReportReplicatorSource;
        static Common::GlobalString const HealthReportReplicatedStoreSource;
        static Common::GlobalString const HealthReportRASource;
        static Common::GlobalString const HealthReportRAPSource;
        static Common::GlobalString const HealthReportFabricNodeSource;
        static Common::GlobalString const HealthReportFederationSource;
        static Common::GlobalString const HealthReportHostingSource;
        static Common::GlobalString const HealthReportCMSource;
        static Common::GlobalString const HealthReportTestabilitySource;
        static Common::GlobalString const HealthReportNamingServiceSource;
        static Common::GlobalString const HealthReportHMSource;
        static Common::GlobalString const HealthReportTransactionalReplicatorSource;

        // Properties
        static Common::GlobalString const HealthStateProperty;
        static Common::GlobalString const HealthActivationProperty;
        static Common::GlobalString const HealthCapacityProperty;
        static Common::GlobalString const ServiceReplicaUnplacedHealthProperty;
        static Common::GlobalString const ReplicaConstraintViolationProperty;
        static Common::GlobalString const FabricCertificateProperty;
        static Common::GlobalString const FabricCertificateRevocationListProperty;
        static Common::GlobalString const SecurityApiProperty;
        static Common::GlobalString const NeighborhoodProperty;
        static Common::GlobalString const HealthReplicaOpenStatusProperty;
        static Common::GlobalString const HealthReplicaCloseStatusProperty;
        static Common::GlobalString const HealthReplicaServiceTypeRegistrationStatusProperty;
        static Common::GlobalString const HealthRAStoreProvider;
        static Common::GlobalString const HealthReplicaChangeRoleStatusProperty;
        static Common::GlobalString const UpgradePrimarySwapHealthProperty;
        static Common::GlobalString const BalancingUnsuccessfulProperty;
        static Common::GlobalString const ConstraintFixUnsuccessfulProperty;
        static Common::GlobalString const ServiceDescriptionHealthProperty;
        static Common::GlobalString const CommitPerformanceHealthProperty;
        static Common::GlobalString const MovementEfficacyProperty;
        static Common::GlobalString const DurationHealthProperty;
        static Common::GlobalString const AuthorityReportProperty;
        static Common::GlobalString const ResourceGovernanceReportProperty;
        static Common::GlobalString const ReconfigurationProperty;
        static Common::GlobalString const RebuildProperty;
        static Common::GlobalString const HealthReportCountProperty;

        // KVS replica query
        static Common::StringLiteral const DatabaseRowCountEstimate;
        static Common::StringLiteral const DatabaseLogicalSizeEstimate;
        static Common::StringLiteral const CopyNotificationCurrentKeyFilter;
        static Common::StringLiteral const CopyNotificationCurrentProgress;
        static Common::StringLiteral const StatusDetails;
        static Common::StringLiteral const MigrationStatus;
        static Common::StringLiteral const CurrentPhase;
        static Common::StringLiteral const NextPhase;
        static Common::StringLiteral const ProviderKind;

        //
        // Reliability
        //
        static Common::Guid const FMServiceGuid;

        static Common::GlobalString const EmptyString;

        //
        // FaultAnalysisService
        //
        static Common::StringLiteral const OperationId;
        static Common::StringLiteral const PartitionSelectorType;
        static Common::StringLiteral const PartitionKey;
        static Common::StringLiteral const RestartPartitionMode;
        static Common::StringLiteral const DataLossMode;
        static Common::StringLiteral const QuorumLossMode;
        static Common::StringLiteral const QuorumLossDurationInSeconds;

        static Common::StringLiteral const ErrorCode;

        static Common::StringLiteral const InvokeDataLossResult;
        static Common::StringLiteral const InvokeQuorumLossResult;
        static Common::StringLiteral const RestartPartitionResult;
        static Common::StringLiteral const NodeTransitionResult;

        static Common::StringLiteral const SelectedPartition;
        static Common::StringLiteral const Force;

        static Common::StringLiteral const RestartNodeCommand;
        static Common::StringLiteral const StopNodeCommand;

        //
        // UpgradeOrchestrationService
        //
        static Common::StringLiteral const ClusterConfig;
        static Common::StringLiteral const RollbackOnFailure;
        static Common::StringLiteral const HealthCheckRetryTimeout;
        static Common::StringLiteral const HealthCheckWaitDurationInSeconds;
        static Common::StringLiteral const HealthCheckStableDurationInSeconds;
        static Common::StringLiteral const UpgradeDomainTimeoutInSeconds;
        static Common::StringLiteral const UpgradeTimeoutInSeconds;
        static Common::StringLiteral const ClusterConfiguration;
        static Common::StringLiteral const ServiceState;

        static Common::StringLiteral const CurrentCodeVersion;
        static Common::StringLiteral const CurrentManifestVersion;
        static Common::StringLiteral const TargetCodeVersion;
        static Common::StringLiteral const TargetManifestVersion;
        static Common::StringLiteral const PendingUpgradeType;

        static Common::StringLiteral const NodeResult;

        //
        // Secret Store Service
        //
        static int const SecretNameMaxLength = 256;
        static int const SecretVersionMaxLength = 256;
        static int const SecretValueMaxSize = 4 * 1024 * 1024; // 4MB

        //
        // Compose deployment
        //
        static Common::StringLiteral const RegistryUserName;
        static Common::StringLiteral const RegistryPassword;
        static Common::StringLiteral const PasswordEncrypted;
        static Common::StringLiteral const ComposeFileContent;
        static Common::StringLiteral const RegistryCredential;
        // Deprecated
        static Common::StringLiteral const RepositoryUserName;
        static Common::StringLiteral const RepositoryPassword;
        static Common::StringLiteral const RepositoryCredential;

        static Common::StringLiteral const ComposeDeploymentStatus;

        //
        // Single Instance
        //
        static Common::StringLiteral const deploymentName;
        static Common::StringLiteral const applicationName;
        static Common::StringLiteral const applicationUri;
        static Common::StringLiteral const status;
        static Common::StringLiteral const statusDetails;
        static Common::StringLiteral const instanceCount;
        static Common::StringLiteral const properties;
        static Common::StringLiteral const containerRegistryServer;
        static Common::StringLiteral const containerRegistryUserName;
        static Common::StringLiteral const containerRegistryPassword;
        static Common::StringLiteral const restartPolicy;
        static Common::StringLiteral const volumes;
        static Common::StringLiteral const image;
        static Common::StringLiteral const command;
        static Common::StringLiteral const ports;
        static Common::StringLiteral const environmentVariables;
        static Common::StringLiteral const instanceView;
        static Common::StringLiteral const restartCount;
        static Common::StringLiteral const currentState;
        static Common::StringLiteral const previousState;
        static Common::StringLiteral const state;
        static Common::StringLiteral const exitCode;
        static Common::StringLiteral const resources;
        static Common::StringLiteral const resourcesRequests;
        static Common::StringLiteral const resourcesLimits;
        static Common::StringLiteral const volumeMounts;
        static Common::StringLiteral const memoryInGB;
        static Common::StringLiteral const cpu;
        static Common::StringLiteral const mountPath;
        static Common::StringLiteral const readOnly;
        static Common::StringLiteral const imageRegistryCredentials;
        static Common::StringLiteral const valueCamelCase;
        static Common::StringLiteral const port;
        static Common::StringLiteral const creationParameters;
        static Common::StringLiteral const nameCamelCase;
        static Common::StringLiteral const azureFile;
        static Common::StringLiteral const shareName;
        static Common::StringLiteral const storageAccountName;
        static Common::StringLiteral const storageAccountKey;
        static Common::StringLiteral const accountName;
        static Common::StringLiteral const accountKey;
        static Common::StringLiteral const sizeDisk;
        static Common::StringLiteral const volumeName;
        static Common::StringLiteral const volumeDescription;
        static Common::StringLiteral const volumeDescriptionForImageBuilder;
        static Common::StringLiteral const volumeParameters;
        static Common::StringLiteral const osType;

        static Common::StringLiteral const RemoveServiceFabricRuntimeAccess;
        static Common::StringLiteral const AzureFilePluginName;
        static Common::GlobalString const ModelV2ReservedDnsNameCharacters;

        static Common::Global<std::vector<std::string>> const ValidNodeIdGeneratorVersionList;

        // Continuation Token
        //
        // A continuation token may contain more than one segment. For example, it may contain
        // and application name and an application version. This delimiter is used to denote
        // the separation between different parts of the continuation token.
        static std::string const ContinuationTokenParserDelimiter;
        static std::string const ContinuationTokenCreationFailedMessage;

        // Resource Governance
        //
        static Common::GlobalString const SystemMetricNameCpuCores;
        static Common::GlobalString const SystemMetricNameMemoryInMB;
        static uint const ResourceGovernanceCpuCorrectionFactor;

        DECLARE_TEST_MUTABLE_CONSTANT(size_t, NamedPropertyMaxValueSize)
    };

    class StringConstants
    {
    public:
        static const Common::GlobalString HttpVerb;
        static const Common::GlobalString UriPath;
        static const Common::GlobalString HttpContentType;
        static const Common::GlobalStringT HttpContentEncoding;
        static const Common::GlobalString HttpRequestBody;
    };
}

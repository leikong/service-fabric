// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"
using namespace Common;
using namespace std;

namespace ServiceModel
{
    __int64 const Constants::UninitializedVersion(-1);

    Common::StringLiteral const Constants::Name("Name");
    Common::StringLiteral const Constants::DeploymentName("DeploymentName");
    Common::StringLiteral const Constants::TypeName("TypeName");
    Common::StringLiteral const Constants::HealthStateFilter("HealthStateFilter");
    Common::StringLiteral const Constants::TypeVersion("TypeVersion");
    Common::StringLiteral const Constants::ParameterList("ParameterList");
    Common::StringLiteral const Constants::DefaultParameterList("DefaultParameterList");
    Common::StringLiteral const Constants::ApplicationHealthPolicy("ApplicationHealthPolicy");
    Common::StringLiteral const Constants::ApplicationHealthPolicyMap("ApplicationHealthPolicyMap");
    Common::StringLiteral const Constants::ApplicationHealthPolicies("ApplicationHealthPolicies");
    Common::StringLiteral const Constants::instances("instances");
    Common::StringLiteral const Constants::instanceNames("instanceNames");
    Common::StringLiteral const Constants::MaxPercentServicesUnhealthy("MaxPercentServicesUnhealthy");
    Common::StringLiteral const Constants::MaxPercentDeployedApplicationsUnhealthy("MaxPercentDeployedApplicationsUnhealthy");
    Common::StringLiteral const Constants::Value("Value");
    Common::StringLiteral const Constants::Status("Status");
    Common::StringLiteral const Constants::Parameters("Parameters");
    Common::StringLiteral const Constants::HealthState("HealthState");
    Common::StringLiteral const Constants::AggregatedHealthState("AggregatedHealthState");
    Common::StringLiteral const Constants::PartitionStatus("PartitionStatus");
    Common::StringLiteral const Constants::LastQuorumLossDurationInSeconds("LastQuorumLossDurationInSeconds");
    Common::StringLiteral const Constants::ServiceManifestVersion("ServiceManifestVersion");
    Common::StringLiteral const Constants::TargetApplicationTypeVersion("TargetApplicationTypeVersion");
    Common::StringLiteral const Constants::TargetApplicationParameterList("TargetApplicationParameterList");
    Common::StringLiteral const Constants::UpgradeKind("UpgradeKind");
    Common::StringLiteral const Constants::UpgradeDescription("UpgradeDescription");
    Common::StringLiteral const Constants::UpdateDescription("UpdateDescription");
    Common::StringLiteral const Constants::RollingUpgradeMode("RollingUpgradeMode");
    Common::StringLiteral const Constants::UpgradeReplicaSetCheckTimeoutInSeconds("UpgradeReplicaSetCheckTimeoutInSeconds");
    Common::StringLiteral const Constants::ReplicaSetCheckTimeoutInMilliseconds("ReplicaSetCheckTimeoutInMilliseconds");
    Common::StringLiteral const Constants::ForceRestart("ForceRestart");
    Common::StringLiteral const Constants::MonitoringPolicy("MonitoringPolicy");
    Common::StringLiteral const Constants::ClusterHealthPolicy("ClusterHealthPolicy");
    Common::StringLiteral const Constants::EnableDeltaHealthEvaluation("EnableDeltaHealthEvaluation");
    Common::StringLiteral const Constants::ClusterUpgradeHealthPolicy("ClusterUpgradeHealthPolicy");
    Common::StringLiteral const Constants::MaxPercentNodesUnhealthyPerUpgradeDomain("MaxPercentNodesUnhealthyPerUpgradeDomain");
    Common::StringLiteral const Constants::MaxPercentDeployedApplicationsUnhealthyPerUpgradeDomain("MaxPercentDeployedApplicationsUnhealthyPerUpgradeDomain");
    Common::StringLiteral const Constants::DefaultApplicationHealthAggregationPolicy("DefaultApplicationHealthAggregationPolicy");
    Common::StringLiteral const Constants::ApplicationHealthAggregationPolicyOverrides("ApplicationHealthAggregationPolicyOverrides");
    Common::StringLiteral const Constants::MaxPercentNodesUnhealthy("MaxPercentNodesUnhealthy");
    Common::StringLiteral const Constants::MaxPercentDeltaUnhealthyNodes("MaxPercentDeltaUnhealthyNodes");
    Common::StringLiteral const Constants::MaxPercentUpgradeDomainDeltaUnhealthyNodes("MaxPercentUpgradeDomainDeltaUnhealthyNodes");
    Common::StringLiteral const Constants::EntryPointLocation("EntryPointLocation");
    Common::StringLiteral const Constants::ProcessId("ProcessId");
    Common::StringLiteral const Constants::HostProcessId("HostProcessId");
    Common::StringLiteral const Constants::LastExitCode("LastExitCode");
    Common::StringLiteral const Constants::LastActivationTime("LastActivationTime");
    Common::StringLiteral const Constants::LastExitTime("LastExitTime");
    Common::StringLiteral const Constants::LastSuccessfulActivationTime("LastSuccessfulActivationTime");
    Common::StringLiteral const Constants::LastSuccessfulExitTime("LastSuccessfulExitTime");
    Common::StringLiteral const Constants::ActivationFailureCount("ActivationFailureCount");
    Common::StringLiteral const Constants::ContinuousActivationFailureCount("ContinuousActivationFailureCount");
    Common::StringLiteral const Constants::ExitFailureCount("ExitFailureCount");
    Common::StringLiteral const Constants::ContinuousExitFailureCount("ContinuousExitFailureCount");
    Common::StringLiteral const Constants::ActivationCount("ActivationCount");
    Common::StringLiteral const Constants::ExitCount("ExitCount");
    Common::StringLiteral const Constants::SetupEntryPoint("SetupEntryPoint");
    Common::StringLiteral const Constants::MainEntryPoint("MainEntryPoint");
    Common::StringLiteral const Constants::HasSetupEntryPoint("HasSetupEntryPoint");
    Common::StringLiteral const Constants::ServiceKind("ServiceKind");
    Common::StringLiteral const Constants::ReplicaId("ReplicaId");
    Common::StringLiteral const Constants::ReplicaOrInstanceId("ReplicaOrInstanceId");
    Common::StringLiteral const Constants::ReplicaRole("ReplicaRole");
    Common::StringLiteral const Constants::PreviousConfigurationRole("PreviousConfigurationRole");
    Common::StringLiteral const Constants::InstanceId("InstanceId");
    Common::StringLiteral const Constants::ReplicaStatus("ReplicaStatus");
    Common::StringLiteral const Constants::ReplicaState("ReplicaState");
    Common::StringLiteral const Constants::LastInBuildDurationInSeconds("LastInBuildDurationInSeconds");
    Common::StringLiteral const Constants::PartitionId("PartitionId");
    Common::StringLiteral const Constants::ActivationId("ActivationId");
    Common::StringLiteral const Constants::ServiceName("ServiceName");
    Common::StringLiteral const Constants::Address("Address");
    Common::StringLiteral const Constants::CodePackageName("CodePackageName");
    Common::StringLiteral const Constants::ServiceTypeName("ServiceTypeName");
    Common::StringLiteral const Constants::CodePackageInstanceId("CodePackageInstanceId");
    Common::StringLiteral const Constants::ServiceGroupMemberDescription("ServiceGroupMemberDescription");
    Common::StringLiteral const Constants::IsServiceGroup("IsServiceGroup");
    Common::StringLiteral const Constants::CodeVersion("CodeVersion");
    Common::StringLiteral const Constants::ConfigVersion("ConfigVersion");
    Common::StringLiteral const Constants::Details("Details");
    Common::StringLiteral const Constants::RunFrequencyInterval("RunFrequencyInterval");
    Common::StringLiteral const Constants::ConsiderWarningAsError("ConsiderWarningAsError");
    Common::StringLiteral const Constants::IgnoreExpiredEvents("IgnoreExpiredEvents");
    Common::StringLiteral const Constants::ApplicationName("ApplicationName");
    Common::StringLiteral const Constants::InitializationData("InitializationData");
    Common::StringLiteral const Constants::PartitionDescription("PartitionDescription");
    Common::StringLiteral const Constants::TargetReplicaSetSize("TargetReplicaSetSize");
    Common::StringLiteral const Constants::MinReplicaSetSize("MinReplicaSetSize");
    Common::StringLiteral const Constants::HasPersistedState("HasPersistedState");
    Common::StringLiteral const Constants::InstanceCount("InstanceCount");
    Common::StringLiteral const Constants::PlacementConstraints("PlacementConstraints");
    Common::StringLiteral const Constants::CorrelationScheme("CorrelationScheme");
    Common::StringLiteral const Constants::ServiceLoadMetrics("ServiceLoadMetrics");
    Common::StringLiteral const Constants::DefaultMoveCost("DefaultMoveCost");
    Common::StringLiteral const Constants::IsDefaultMoveCostSpecified("IsDefaultMoveCostSpecified");
    Common::StringLiteral const Constants::ServicePackageActivationMode("ServicePackageActivationMode");
    Common::StringLiteral const Constants::ServicePlacementPolicies("ServicePlacementPolicies");
    Common::StringLiteral const Constants::Flags("Flags");
    Common::StringLiteral const Constants::ReplicaRestartWaitDurationSeconds("ReplicaRestartWaitDurationSeconds");
    Common::StringLiteral const Constants::QuorumLossWaitDurationSeconds("QuorumLossWaitDurationSeconds");
    Common::StringLiteral const Constants::StandByReplicaKeepDurationSeconds("StandByReplicaKeepDurationSeconds");
    Common::StringLiteral const Constants::ReplicaRestartWaitDurationInMilliseconds("ReplicaRestartWaitDurationInMilliseconds");
    Common::StringLiteral const Constants::QuorumLossWaitDurationInMilliseconds("QuorumLossWaitDurationInMilliseconds");
    Common::StringLiteral const Constants::StandByReplicaKeepDurationInMilliseconds("StandByReplicaKeepDurationInMilliseconds");
    Common::StringLiteral const Constants::Id("Id");
    Common::StringLiteral const Constants::IpAddressOrFQDN("IpAddressOrFQDN");
    Common::StringLiteral const Constants::Type("Type");
    Common::StringLiteral const Constants::Version("Version");
    Common::StringLiteral const Constants::NodeStatus("NodeStatus");
    Common::StringLiteral const Constants::NodeDeactivationIntent("NodeDeactivationIntent");
    Common::StringLiteral const Constants::NodeDeactivationStatus("NodeDeactivationStatus");
    Common::StringLiteral const Constants::NodeDeactivationTask("NodeDeactivationTask");
    Common::StringLiteral const Constants::NodeDeactivationTaskId("NodeDeactivationTaskId");
    Common::StringLiteral const Constants::NodeDeactivationTaskType("NodeDeactivationTaskType");
    Common::StringLiteral const Constants::NodeDeactivationTaskIdPrefixRM("RM/");
    Common::StringLiteral const Constants::NodeDeactivationTaskIdPrefixWindowsAzure("WindowsAzure");
    Common::StringLiteral const Constants::NodeDeactivationInfo("NodeDeactivationInfo");
    Common::StringLiteral const Constants::NodeUpTimeInSeconds("NodeUpTimeInSeconds");
    Common::StringLiteral const Constants::NodeDownTimeInSeconds("NodeDownTimeInSeconds");
    Common::StringLiteral const Constants::NodeUpAt("NodeUpAt");
    Common::StringLiteral const Constants::NodeDownAt("NodeDownAt");
    Common::StringLiteral const Constants::UpgradeDomain("UpgradeDomain");
    Common::StringLiteral const Constants::FaultDomain("FaultDomain");
    Common::StringLiteral const Constants::PartitionScheme("PartitionScheme");
    Common::StringLiteral const Constants::Count("Count");
    Common::StringLiteral const Constants::Names("Names");
    Common::StringLiteral const Constants::LowKey("LowKey");
    Common::StringLiteral const Constants::HighKey("HighKey");
    Common::StringLiteral const Constants::MaxPercentReplicasUnhealthy("MaxPercentReplicasUnhealthy");
    Common::StringLiteral const Constants::Scheme("Scheme");
    Common::StringLiteral const Constants::Weight("Weight");
    Common::StringLiteral const Constants::PrimaryDefaultLoad("PrimaryDefaultLoad");
    Common::StringLiteral const Constants::SecondaryDefaultLoad("SecondaryDefaultLoad");
    Common::StringLiteral const Constants::DefaultLoad("DefaultLoad");
    Common::StringLiteral const Constants::FailureAction("FailureAction");
    Common::StringLiteral const Constants::HealthCheckWaitDurationInMilliseconds("HealthCheckWaitDurationInMilliseconds");
    Common::StringLiteral const Constants::HealthCheckStableDurationInMilliseconds("HealthCheckStableDurationInMilliseconds");
    Common::StringLiteral const Constants::HealthCheckRetryTimeoutInMilliseconds("HealthCheckRetryTimeoutInMilliseconds");
    Common::StringLiteral const Constants::UpgradeTimeoutInMilliseconds("UpgradeTimeoutInMilliseconds");
    Common::StringLiteral const Constants::UpgradeDomainTimeoutInMilliseconds("UpgradeDomainTimeoutInMilliseconds");
    Common::StringLiteral const Constants::UpgradeDuration("UpgradeDuration");
    Common::StringLiteral const Constants::UpgradeDurationInMilliseconds("UpgradeDurationInMilliseconds");
    Common::StringLiteral const Constants::CurrentUpgradeDomainDuration("CurrentUpgradeDomainDuration");
    Common::StringLiteral const Constants::UpgradeDomainDurationInMilliseconds("UpgradeDomainDurationInMilliseconds");
    Common::StringLiteral const Constants::MaxPercentPartitionsUnhealthy("MaxPercentPartitionsUnhealthy");
    Common::StringLiteral const Constants::RepartitionDescription("RepartitionDescription");
    Common::StringLiteral const Constants::PartitionKind("PartitionKind");
    Common::StringLiteral const Constants::NamesToAdd("NamesToAdd");
    Common::StringLiteral const Constants::NamesToRemove("NamesToRemove");
    Common::StringLiteral const Constants::ServicePartitionKind("ServicePartitionKind");
    Common::StringLiteral const Constants::PartitionInformation("PartitionInformation");
    Common::StringLiteral const Constants::InQuorumLoss("InQuorumLoss");
    Common::StringLiteral const Constants::DomainName("DomainName");
    Common::StringLiteral const Constants::ManifestVersion("ManifestVersion");
    Common::StringLiteral const Constants::NodeName("NodeName");
    Common::StringLiteral const Constants::NodeId("NodeId");
    Common::StringLiteral const Constants::NodeInstanceId("NodeInstanceId");
    Common::StringLiteral const Constants::Content("Content");
    StringLiteral const Constants::HttpVerb("HttpVerb");
    GlobalString const StringConstants::HttpVerb = make_global<string>(Constants::HttpVerb.cbegin());
    StringLiteral const Constants::UriPath("UriPath");
    GlobalString const StringConstants::UriPath = make_global<string>(Constants::UriPath.cbegin());
    StringLiteral const Constants::HttpContentType("Content-Type");
    GlobalString const StringConstants::HttpContentType = make_global<string>(Constants::HttpContentType.cbegin());
    StringLiteral const Constants::HttpContentEncoding("Content-Encoding");
#ifdef PLATFORM_UNIX
    GlobalString const StringConstants::HttpContentEncoding = make_global<string>("Content-Encoding");
#else
    GlobalString const StringConstants::HttpContentEncoding = make_global<string>(Constants::HttpContentEncoding.cbegin());
#endif
    StringLiteral const Constants::HttpRequestBody("Body");
    GlobalString const StringConstants::HttpRequestBody = make_global<string>(Constants::HttpRequestBody.cbegin());
    StringLiteral const Constants::HttpResponseBody("Body");
    StringLiteral const Constants::HttpStatus("Status");
    Common::StringLiteral const Constants::CreateFabricDump("CreateFabricDump");
    Common::StringLiteral const Constants::IsStateful("IsStateful");
    Common::StringLiteral const Constants::UseImplicitHost("UseImplicitHost");
    Common::StringLiteral const Constants::UseServiceFabricReplicatedStore("UseServiceFabricReplicatedStore");
    Common::StringLiteral const Constants::UseImplicitFactory("UseImplicitFactory");
    Common::StringLiteral const Constants::Extensions("Extensions");
    Common::StringLiteral const Constants::LoadMetrics("LoadMetrics");
    Common::StringLiteral const Constants::ServiceTypeDescription("ServiceTypeDescription");
    Common::StringLiteral const Constants::ServiceGroupTypeDescription("ServiceGroupTypeDescription");
    Common::StringLiteral const Constants::ServiceGroupTypeMemberDescription("ServiceGroupTypeMemberDescription");
    Common::StringLiteral const Constants::IsSeedNode("IsSeedNode");
    Common::StringLiteral const Constants::ReplicaRestartWaitDurationInMilliSeconds("ReplicaRestartWaitDurationInMilliSeconds");
    Common::StringLiteral const Constants::RunAsUserName("RunAsUserName");
    Common::StringLiteral const Constants::CodePackageEntryPointStatistics("CodePackageEntryPointStatistics");
    Common::StringLiteral const Constants::Manifest("Manifest");
    Common::StringLiteral const Constants::ServiceManifestName("ServiceManifestName");
    Common::StringLiteral const Constants::ServicePackageActivationId("ServicePackageActivationId");
    Common::StringLiteral const Constants::HostType("HostType");
    Common::StringLiteral const Constants::HostIsolationMode("HostIsolationMode");
    Common::StringLiteral const Constants::ApplicationTypeBuildPath("ApplicationTypeBuildPath");
    Common::StringLiteral const Constants::Async("Async");
    Common::StringLiteral const Constants::ApplicationPackageCleanupPolicy("ApplicationPackageCleanupPolicy");
    Common::StringLiteral const Constants::ApplicationPackageDownloadUri("ApplicationPackageDownloadUri");
    Common::StringLiteral const Constants::NextUpgradeDomain("NextUpgradeDomain");
    Common::StringLiteral const Constants::State("State");
    Common::StringLiteral const Constants::CodeFilePath("CodeFilePath");
    Common::StringLiteral const Constants::ClusterManifestFilePath("ClusterManifestFilePath");
    Common::StringLiteral const Constants::DeactivationIntent("DeactivationIntent");
    Common::StringLiteral const Constants::ApplicationTypeVersion("ApplicationTypeVersion");
    Common::StringLiteral const Constants::UpgradeDomains("UpgradeDomains");
    Common::StringLiteral const Constants::UpgradeState("UpgradeState");
    Common::StringLiteral const Constants::ProgressStatus("ProgressStatus");
    Common::StringLiteral const Constants::UpgradeDomainName("UpgradeDomainName");
    Common::StringLiteral const Constants::MaxPercentUnhealthyServices("MaxPercentUnhealthyServices");
    Common::StringLiteral const Constants::MaxPercentUnhealthyPartitionsPerService("MaxPercentUnhealthyPartitionsPerService");
    Common::StringLiteral const Constants::MaxPercentUnhealthyReplicasPerPartition("MaxPercentUnhealthyReplicasPerPartition");
    Common::StringLiteral const Constants::MaxPercentUnhealthyDeployedApplications("MaxPercentUnhealthyDeployedApplications");
    Common::StringLiteral const Constants::DefaultServiceTypeHealthPolicy("DefaultServiceTypeHealthPolicy");
    Common::StringLiteral const Constants::ServiceTypeHealthPolicyMap("ServiceTypeHealthPolicyMap");
    Common::StringLiteral const Constants::MaxPercentUnhealthyNodes("MaxPercentUnhealthyNodes");
    Common::StringLiteral const Constants::MaxPercentUnhealthyApplications("MaxPercentUnhealthyApplications");
    Common::StringLiteral const Constants::TotalCount("TotalCount");
    Common::StringLiteral const Constants::BaselineErrorCount("BaselineErrorCount");
    Common::StringLiteral const Constants::BaselineTotalCount("BaselineTotalCount");
    Common::StringLiteral const Constants::SourceId("SourceId");
    Common::StringLiteral const Constants::Property("Property");
    Common::StringLiteral const Constants::Description("Description");
    Common::StringLiteral const Constants::SequenceNumber("SequenceNumber");
    Common::StringLiteral const Constants::RemoveWhenExpired("RemoveWhenExpired");
    Common::StringLiteral const Constants::TimeToLiveInMs("TimeToLiveInMilliSeconds");
    Common::StringLiteral const Constants::SourceUtcTimestamp("SourceUtcTimestamp");
    Common::StringLiteral const Constants::LastModifiedUtcTimestamp("LastModifiedUtcTimestamp");
    Common::StringLiteral const Constants::LastOkTransitionAt("LastOkTransitionAt");
    Common::StringLiteral const Constants::LastWarningTransitionAt("LastWarningTransitionAt");
    Common::StringLiteral const Constants::LastErrorTransitionAt("LastErrorTransitionAt");
    Common::StringLiteral const Constants::IsExpired("IsExpired");
    Common::StringLiteral const Constants::HealthEvent("HealthEvent");
    Common::StringLiteral const Constants::HealthEvents("HealthEvents");
    Common::StringLiteral const Constants::OkCount("OkCount");
    Common::StringLiteral const Constants::WarningCount("WarningCount");
    Common::StringLiteral const Constants::ErrorCount("ErrorCount");
    Common::StringLiteral const Constants::EntityKind("EntityKind");
    Common::StringLiteral const Constants::HealthStateCount("HealthStateCount");
    Common::StringLiteral const Constants::HealthStatistics("HealthStatistics");
    Common::StringLiteral const Constants::HealthStateCountList("HealthStateCountList");
    Common::StringLiteral const Constants::ExcludeHealthStatistics("ExcludeHealthStatistics");
    Common::StringLiteral const Constants::IncludeSystemApplicationHealthStatistics("IncludeSystemApplicationHealthStatistics");
    Common::StringLiteral const Constants::ServiceAggregatedHealthStates("ServiceHealthStates");
    Common::StringLiteral const Constants::DeployedApplicationsAggregatedHealthStates("DeployedApplicationHealthStates");
    Common::StringLiteral const Constants::PartitionAggregatedHealthState("PartitionHealthStates");
    Common::StringLiteral const Constants::ReplicaAggregatedHealthState("ReplicaHealthStates");
    Common::StringLiteral const Constants::DeployedServicePackageAggregatedHealthState("DeployedServicePackageHealthStates");
    Common::StringLiteral const Constants::NextActivationTime("NextActivationTime");
    Common::StringLiteral const Constants::NodeAggregatedHealthStates("NodeHealthStates");
    Common::StringLiteral const Constants::ApplicationAggregatedHealthStates("ApplicationHealthStates");
    Common::StringLiteral const Constants::SystemApplicationAggregatedHealthState("SystemApplicationAggregatedHealthState");
    Common::StringLiteral const Constants::ServiceManifestNameFilter("ServiceManifestNameFilter");
    Common::StringLiteral const Constants::ServicePackageActivationIdFilter("ServicePackageActivationIdFilter");
    Common::StringLiteral const Constants::ApplicationNameFilter("ApplicationNameFilter");
    Common::StringLiteral const Constants::ApplicationTypeNameFilter("ApplicationTypeNameFilter");
    Common::StringLiteral const Constants::ApplicationDefinitionKind("ApplicationDefinitionKind");
    Common::StringLiteral const Constants::ApplicationTypeDefinitionKind("ApplicationTypeDefinitionKind");
    Common::StringLiteral const Constants::DeployedServicePackagesFilter("DeployedServicePackagesFilter");
    Common::StringLiteral const Constants::NodeNameFilter("NodeNameFilter");
    Common::StringLiteral const Constants::ReplicaOrInstanceIdFilter("ReplicaOrInstanceIdFilter");
    Common::StringLiteral const Constants::PartitionIdFilter("PartitionIdFilter");
    Common::StringLiteral const Constants::ReplicasFilter("ReplicasFilter");
    Common::StringLiteral const Constants::PartitionsFilter("PartitionsFilter");
    Common::StringLiteral const Constants::ServiceNameFilter("ServiceNameFilter");
    Common::StringLiteral const Constants::DeployedApplicationsFilter("DeployedApplicationsFilter");
    Common::StringLiteral const Constants::ServicesFilter("ServicesFilter");
    Common::StringLiteral const Constants::ApplicationsFilter("ApplicationsFilter");
    Common::StringLiteral const Constants::HealthPolicy("HealthPolicy");
    Common::StringLiteral const Constants::NodesFilter("NodesFilter");
    Common::StringLiteral const Constants::DeployedServicePackageFilters("DeployedServicePackageFilters");
    Common::StringLiteral const Constants::ReplicaFilters("ReplicaFilters");
    Common::StringLiteral const Constants::PartitionFilters("PartitionFilters");
    Common::StringLiteral const Constants::DeployedApplicationFilters("DeployedApplicationFilters");
    Common::StringLiteral const Constants::ServiceFilters("ServiceFilters");;
    Common::StringLiteral const Constants::ApplicationFilters("ApplicationFilters");
    Common::StringLiteral const Constants::NodeFilters("NodeFilters");
    Common::StringLiteral const Constants::ReplicaHealthStateChunks("ReplicaHealthStateChunks");
    Common::StringLiteral const Constants::PartitionHealthStateChunks("PartitionHealthStateChunks");
    Common::StringLiteral const Constants::DeployedServicePackageHealthStateChunks("DeployedServicePackageHealthStateChunks");
    Common::StringLiteral const Constants::DeployedApplicationHealthStateChunks("DeployedApplicationHealthStateChunks");
    Common::StringLiteral const Constants::ServiceHealthStateChunks("ServiceHealthStateChunks");
    Common::StringLiteral const Constants::ApplicationHealthStateChunks("ApplicationHealthStateChunks");
    Common::StringLiteral const Constants::NodeHealthStateChunks("NodeHealthStateChunks");
    Common::StringLiteral const Constants::Metadata("Metadata");
    Common::StringLiteral const Constants::ServiceDnsName("ServiceDnsName");
    Common::StringLiteral const Constants::ScalingPolicies("ScalingPolicies");
    Common::StringLiteral const Constants::Key("Key");
    Common::StringLiteral const Constants::LastBalancingStartTimeUtc("LastBalancingStartTimeUtc");
    Common::StringLiteral const Constants::LastBalancingEndTimeUtc("LastBalancingEndTimeUtc");
    Common::StringLiteral const Constants::LoadMetricInformation("LoadMetricInformation");
    Common::StringLiteral const Constants::IsBalancedBefore("IsBalancedBefore");
    Common::StringLiteral const Constants::IsBalancedAfter("IsBalancedAfter");
    Common::StringLiteral const Constants::DeviationBefore("DeviationBefore");
    Common::StringLiteral const Constants::DeviationAfter("DeviationAfter");
    Common::StringLiteral const Constants::ActivityThreshold("ActivityThreshold");
    Common::StringLiteral const Constants::BalancingThreshold("BalancingThreshold");
    Common::StringLiteral const Constants::ClusterCapacity("ClusterCapacity");
    Common::StringLiteral const Constants::ClusterLoad("ClusterLoad");
    Common::StringLiteral const Constants::RemainingUnbufferedCapacity("RemainingUnbufferedCapacity");
    Common::StringLiteral const Constants::NodeBufferPercentage("NodeBufferPercentage");
    Common::StringLiteral const Constants::BufferedCapacity("BufferedCapacity");
    Common::StringLiteral const Constants::RemainingBufferedCapacity("RemainingBufferedCapacity");
    Common::StringLiteral const Constants::IsClusterCapacityViolation("IsClusterCapacityViolation");
    Common::StringLiteral const Constants::NodeCapacity("NodeCapacity");
    Common::StringLiteral const Constants::NodeRemainingCapacity("NodeRemainingCapacity");
    Common::StringLiteral const Constants::NodeLoad("NodeLoad");
    Common::StringLiteral const Constants::IsCapacityViolation("IsCapacityViolation");
    Common::StringLiteral const Constants::NodeBufferedCapacity("NodeBufferedCapacity");
    Common::StringLiteral const Constants::NodeRemainingBufferedCapacity("NodeRemainingBufferedCapacity");
    Common::StringLiteral const Constants::CurrentNodeLoad("CurrentNodeLoad");
    Common::StringLiteral const Constants::NodeCapacityRemaining("NodeCapacityRemaining");
    Common::StringLiteral const Constants::BufferedNodeCapacityRemaining("BufferedNodeCapacityRemaining");
    Common::StringLiteral const Constants::NodeLoadMetricInformation("NodeLoadMetricInformation");
    Common::StringLiteral const Constants::Action("Action");
    Common::StringLiteral const Constants::LoadMetricReports("LoadMetricReports");
    Common::StringLiteral const Constants::PrimaryLoadMetricReports("PrimaryLoadMetricReports");
    Common::StringLiteral const Constants::SecondaryLoadMetricReports("SecondaryLoadMetricReports");
    Common::StringLiteral const Constants::CurrentConfigurationEpoch("CurrentConfigurationEpoch");
    Common::StringLiteral const Constants::PrimaryEpoch("PrimaryEpoch");
    Common::StringLiteral const Constants::ConfigurationVersion("ConfigurationVersion");
    Common::StringLiteral const Constants::DataLossVersion("DataLossVersion");
    Common::StringLiteral const Constants::ContinuationToken("ContinuationToken");
    Common::StringLiteral const Constants::MaxResults("MaxResults");
    Common::StringLiteral const Constants::Items("Items");
    Common::StringLiteral const Constants::ApplicationTypeHealthPolicyMap("ApplicationTypeHealthPolicyMap");
    Common::StringLiteral const Constants::ApplicationCapacity("ApplicationCapacity");
    Common::StringLiteral const Constants::MinimumNodes("MinimumNodes");
    Common::StringLiteral const Constants::MaximumNodes("MaximumNodes");
    Common::StringLiteral const Constants::ApplicationMetrics("ApplicationMetrics");
    Common::StringLiteral const Constants::ReservationCapacity("ReservationCapacity");
    Common::StringLiteral const Constants::MaximumCapacity("MaximumCapacity");
    Common::StringLiteral const Constants::TotalApplicationCapacity("TotalApplicationCapacity");
    Common::StringLiteral const Constants::RemoveApplicationCapacity("RemoveApplicationCapacity");
    Common::StringLiteral const Constants::ApplicationLoad("ApplicationLoad");
    Common::StringLiteral const Constants::ApplicationLoadMetricInformation("ApplicationLoadMetricInformation");
    Common::StringLiteral const Constants::NodeCount("NodeCount");
    Common::StringLiteral const Constants::IsStopped("IsStopped");
    Common::StringLiteral const Constants::IsConsistent("IsConsistent");
    Common::StringLiteral const Constants::SubNames("SubNames");
    Common::StringLiteral const Constants::PropertyName("PropertyName");
    Common::StringLiteral const Constants::TypeId("TypeId");
    Common::StringLiteral const Constants::CustomTypeId("CustomTypeId");
    Common::StringLiteral const Constants::Parent("Parent");
    Common::StringLiteral const Constants::SizeInBytes("SizeInBytes");
    Common::StringLiteral const Constants::Properties("Properties");
    Common::StringLiteral const Constants::Data("Data");
    Common::StringLiteral const Constants::IncludeValue("IncludeValue");
    Common::StringLiteral const Constants::Exists("Exists");
    Common::StringLiteral const Constants::Operations("Operations");
    Common::StringLiteral const Constants::ErrorMessage("ErrorMessage");
    Common::StringLiteral const Constants::OperationIndex("OperationIndex");

    Common::StringLiteral const Constants::QueryPagingDescription("QueryPagingDescription");

    Common::StringLiteral const Constants::ChaosEvent("ChaosEvent");
    Common::StringLiteral const Constants::ChaosEventsFilter("Filter");
    Common::StringLiteral const Constants::ChaosParameters("ChaosParameters");
    Common::StringLiteral const Constants::ClientType("ClientType");
    Common::StringLiteral const Constants::Reason("Reason");
    Common::StringLiteral const Constants::Faults("Faults");
    Common::StringLiteral const Constants::MaxClusterStabilizationTimeoutInSeconds("MaxClusterStabilizationTimeoutInSeconds");
    Common::StringLiteral const Constants::MaxConcurrentFaults("MaxConcurrentFaults");
    Common::StringLiteral const Constants::WaitTimeBetweenIterationsInSeconds("WaitTimeBetweenIterationsInSeconds");
    Common::StringLiteral const Constants::WaitTimeBetweenFaultsInSeconds("WaitTimeBetweenFaultsInSeconds");
    Common::StringLiteral const Constants::TimeToRunInSeconds("TimeToRunInSeconds");
    Common::StringLiteral const Constants::EnableMoveReplicaFaults("EnableMoveReplicaFaults");
    Common::StringLiteral const Constants::Context("Context");
    Common::StringLiteral const Constants::Map("Map");
    Common::StringLiteral const Constants::TimeStampUtc("TimeStampUtc");
    Common::StringLiteral const Constants::NodeTypeInclusionList("NodeTypeInclusionList");
    Common::StringLiteral const Constants::ApplicationInclusionList("ApplicationInclusionList");
    Common::StringLiteral const Constants::ChaosTargetFilter("ChaosTargetFilter");
    Common::StringLiteral const Constants::Schedule("Schedule");
    Common::StringLiteral const Constants::ChaosStatus("Status");
    Common::StringLiteral const Constants::ChaosScheduleStatus("ScheduleStatus");
    Common::StringLiteral const Constants::StartDate("StartDate");
    Common::StringLiteral const Constants::ExpiryDate("ExpiryDate");
    Common::StringLiteral const Constants::StartTime("StartTime");
    Common::StringLiteral const Constants::EndTime("EndTime");
    Common::StringLiteral const Constants::ChaosParametersMap("ChaosParametersDictionary");
    Common::StringLiteral const Constants::Jobs("Jobs");
    Common::StringLiteral const Constants::Days("Days");
    Common::StringLiteral const Constants::Times("Times");
    Common::StringLiteral const Constants::Hour("Hour");
    Common::StringLiteral const Constants::Minute("Minute");
    Common::StringLiteral const Constants::Sunday("Sunday");
    Common::StringLiteral const Constants::Monday("Monday");
    Common::StringLiteral const Constants::Tuesday("Tuesday");
    Common::StringLiteral const Constants::Wednesday("Wednesday");
    Common::StringLiteral const Constants::Thursday("Thursday");
    Common::StringLiteral const Constants::Friday("Friday");
    Common::StringLiteral const Constants::Saturday("Saturday");

    Common::StringLiteral const Constants::ServiceStatus("ServiceStatus");
    Common::StringLiteral const Constants::Kind("Kind");
    Common::StringLiteral const Constants::KindLowerCase("kind");
    Common::StringLiteral const Constants::UnhealthyEvent("UnhealthyEvent");
    Common::StringLiteral const Constants::unhealthyEvaluation("unhealthyEvaluation");
    Common::StringLiteral const Constants::UnhealthyEvaluations("UnhealthyEvaluations");
    Common::StringLiteral const Constants::ApplicationUnhealthyEvaluations("ApplicationUnhealthyEvaluations");
    Common::StringLiteral const Constants::HealthEvaluation("HealthEvaluation");
    Common::StringLiteral const Constants::UpgradePhase("UpgradePhase");
    Common::StringLiteral const Constants::SafetyCheckKind("SafetyCheckKind");
    Common::StringLiteral const Constants::SafetyCheck("SafetyCheck");
    Common::StringLiteral const Constants::PendingSafetyChecks("PendingSafetyChecks");
    Common::StringLiteral const Constants::NodeUpgradeProgressList("NodeUpgradeProgressList");
    Common::StringLiteral const Constants::CurrentUpgradeDomainProgress("CurrentUpgradeDomainProgress");
    Common::StringLiteral const Constants::StartTimestampUtc("StartTimestampUtc");
    Common::StringLiteral const Constants::FailureTimestampUtc("FailureTimestampUtc");
    Common::StringLiteral const Constants::FailureReason("FailureReason");
    Common::StringLiteral const Constants::UpgradeDomainProgressAtFailure("UpgradeDomainProgressAtFailure");
    Common::StringLiteral const Constants::UpgradeStatusDetails("UpgradeStatusDetails");
    Common::StringLiteral const Constants::ApplicationUpgradeStatusDetails("ApplicationUpgradeStatusDetails");
    Common::StringLiteral const Constants::WorkDirectory("WorkDirectory");
    Common::StringLiteral const Constants::LogDirectory("LogDirectory");
    Common::StringLiteral const Constants::TempDirectory("TempDirectory");

    Common::StringLiteral const Constants::MetricName("MetricName");
    Common::StringLiteral const Constants::MaxInstanceCount("MaxInstanceCount");
    Common::StringLiteral const Constants::MaxPartitionCount("MaxPartitionCount");
    Common::StringLiteral const Constants::MinInstanceCount("MinInstanceCount");
    Common::StringLiteral const Constants::MinPartitionCount("MinPartitionCount");
    Common::StringLiteral const Constants::LowerLoadThreshold("LowerLoadThreshold");
    Common::StringLiteral const Constants::UpperLoadThreshold("UpperLoadThreshold");
    Common::StringLiteral const Constants::UseOnlyPrimaryLoad("UseOnlyPrimaryLoad");
    Common::StringLiteral const Constants::ScaleIntervalInSeconds("ScaleIntervalInSeconds");
    Common::StringLiteral const Constants::ScaleIncrement("ScaleIncrement");
    Common::StringLiteral const Constants::ScalingTrigger("ScalingTrigger");
    Common::StringLiteral const Constants::ScalingMechanism("ScalingMechanism");

    StringLiteral const Constants::ReplicatorStatus("ReplicatorStatus");

    StringLiteral const Constants::LastReplicationOperationReceivedTimeUtc("LastReplicationOperationReceivedTimeUtc");
    StringLiteral const Constants::LastCopyOperationReceivedTimeUtc("LastCopyOperationReceivedTimeUtc");
    StringLiteral const Constants::LastAcknowledgementSentTimeUtc("LastAcknowledgementSentTimeUtc");

    StringLiteral const Constants::RemoteReplicators("RemoteReplicators");
    StringLiteral const Constants::LastReceivedReplicationSequenceNumber("LastReceivedReplicationSequenceNumber");
    StringLiteral const Constants::LastAppliedReplicationSequenceNumber("LastAppliedReplicationSequenceNumber");
    StringLiteral const Constants::IsInBuild("IsInBuild");
    StringLiteral const Constants::LastReceivedCopySequenceNumber("LastReceivedCopySequenceNumber");
    StringLiteral const Constants::LastAppliedCopySequenceNumber("LastAppliedCopySequenceNumber");
    StringLiteral const Constants::LastAcknowledgementProcessedTimeUtc("LastAcknowledgementProcessedTimeUtc");

    StringLiteral const Constants::RemoteReplicatorAckStatus("RemoteReplicatorAcknowledgementStatus");
    StringLiteral const Constants::AverageReceiveDuration("AverageReceiveDuration");
    StringLiteral const Constants::AverageApplyDuration("AverageApplyDuration");
    StringLiteral const Constants::NotReceivedCount("NotReceivedCount");
    StringLiteral const Constants::ReceivedAndNotAppliedCount("ReceivedAndNotAppliedCount");
    StringLiteral const Constants::CopyStreamAcknowledgementDetail("CopyStreamAcknowledgementDetail");
    StringLiteral const Constants::ReplicationStreamAcknowledgementDetail("ReplicationStreamAcknowledgementDetail");

    StringLiteral const Constants::ReplicationQueueStatus("ReplicationQueueStatus");
    StringLiteral const Constants::CopyQueueStatus("CopyQueueStatus");
    StringLiteral const Constants::QueueUtilizationPercentage("QueueUtilizationPercentage");
    StringLiteral const Constants::FirstSequenceNumber("FirstSequenceNumber");
    StringLiteral const Constants::CompletedSequenceNumber("CompletedSequenceNumber");
    StringLiteral const Constants::CommittedSequenceNumber("CommittedSequenceNumber");
    StringLiteral const Constants::LastSequenceNumber("LastSequenceNumber");
    StringLiteral const Constants::QueueMemorySize("QueueMemorySize");

    StringLiteral const Constants::CurrentValue("CurrentValue");
    StringLiteral const Constants::LastReportedUtc("LastReportedUtc");
    StringLiteral const Constants::CurrentServiceOperation("CurrentServiceOperation");
    StringLiteral const Constants::CurrentServiceOperationStartTimeUtc("CurrentServiceOperationStartTimeUtc");
    StringLiteral const Constants::ReportedLoad("ReportedLoad");
    StringLiteral const Constants::CurrentReplicatorOperation("CurrentReplicatorOperation");
    StringLiteral const Constants::CurrentReplicatorOperationStartTimeUtc("CurrentReplicatorOperationStartTimeUtc");
    StringLiteral const Constants::ReadStatus("ReadStatus");
    StringLiteral const Constants::WriteStatus("WriteStatus");
    StringLiteral const Constants::ServicePackageInstanceId("ServicePackageInstanceId");
    StringLiteral const Constants::ReplicaInstanceId("ReplicaInstanceId");

    GlobalString Constants::InfrastructureServicePrimaryCountName = make_global<std::string>("__InfrastructureServicePrimaryCount__");
    GlobalString Constants::InfrastructureServiceReplicaCountName = make_global<std::string>("__InfrastructureServiceReplicaCount__");

    GlobalString const Constants::ValidateTokenAction = make_global<std::string>("ValidateToken");

    GlobalString Constants::EventSystemSourcePrefix = make_global<std::string>("System");

    StringLiteral const Constants::Port("Port");
    StringLiteral const Constants::ClusterConnectionPort("ClusterConnectionPort");

    StringLiteral const Constants::ApplicationIds("ApplicationIds");
    StringLiteral const Constants::ApplicationTypeName("ApplicationTypeName");
    StringLiteral const Constants::CodePackageIds("CodePackageIds");
    StringLiteral const Constants::ConfigPackageIds("ConfigPackageIds");
    StringLiteral const Constants::DataPackageIds("DataPackageIds");

    StringLiteral const Constants::PackageSharingScope("PackageSharingScope");
    StringLiteral const Constants::SharedPackageName("SharedPackageName");
    StringLiteral const Constants::PackageSharingPolicy("PackageSharingPolicy");
    StringLiteral const Constants::FMVersion("FMVersion");
    StringLiteral const Constants::StoreVersion("StoreVersion");
    StringLiteral const Constants::GenerationNumber("GenerationNumber");
    StringLiteral const Constants::Generation("Generation");
    StringLiteral const Constants::Endpoints("Endpoints");
    StringLiteral const Constants::PreviousRspVersion("PreviousRspVersion");

    Common::StringLiteral const Constants::Scope("Scope");
    Common::StringLiteral const Constants::TaskId("TaskId");
    Common::StringLiteral const Constants::Target("Target");
    Common::StringLiteral const Constants::Executor("Executor");
    Common::StringLiteral const Constants::ExecutorData("ExecutorData");
    Common::StringLiteral const Constants::ResultStatus("ResultStatus");
    Common::StringLiteral const Constants::ResultCode("ResultCode");
    Common::StringLiteral const Constants::ResultDetails("ResultDetails");
    Common::StringLiteral const Constants::History("History");
    Common::StringLiteral const Constants::CreatedUtcTimestamp("CreatedUtcTimestamp");
    Common::StringLiteral const Constants::ClaimedUtcTimestamp("ClaimedUtcTimestamp");
    Common::StringLiteral const Constants::PreparingUtcTimestamp("PreparingUtcTimestamp");
    Common::StringLiteral const Constants::ApprovedUtcTimestamp("ApprovedUtcTimestamp");
    Common::StringLiteral const Constants::ExecutingUtcTimestamp("ExecutingUtcTimestamp");
    Common::StringLiteral const Constants::RestoringUtcTimestamp("RestoringUtcTimestamp");
    Common::StringLiteral const Constants::CompletedUtcTimestamp("CompletedUtcTimestamp");
    Common::StringLiteral const Constants::PreparingHealthCheckStartUtcTimestamp("PreparingHealthCheckStartUtcTimestamp");
    Common::StringLiteral const Constants::PreparingHealthCheckEndUtcTimestamp("PreparingHealthCheckEndUtcTimestamp");
    Common::StringLiteral const Constants::RestoringHealthCheckStartUtcTimestamp("RestoringHealthCheckStartUtcTimestamp");
    Common::StringLiteral const Constants::RestoringHealthCheckEndUtcTimestamp("RestoringHealthCheckEndUtcTimestamp");
    Common::StringLiteral const Constants::PreparingHealthCheckState("PreparingHealthCheckState");
    Common::StringLiteral const Constants::RestoringHealthCheckState("RestoringHealthCheckState");
    Common::StringLiteral const Constants::PerformPreparingHealthCheck("PerformPreparingHealthCheck");
    Common::StringLiteral const Constants::PerformRestoringHealthCheck("PerformRestoringHealthCheck");
    Common::StringLiteral const Constants::Impact("Impact");
    Common::StringLiteral const Constants::ImpactLevel("ImpactLevel");
    Common::StringLiteral const Constants::NodeImpactList("NodeImpactList");
    Common::StringLiteral const Constants::NodeNames("NodeNames");
    Common::StringLiteral const Constants::RequestAbort("RequestAbort");
    Common::StringLiteral const Constants::MinNodeLoadValue("MinNodeLoadValue");
    Common::StringLiteral const Constants::MinNodeLoadId("MinNodeLoadId");
    Common::StringLiteral const Constants::MaxNodeLoadValue("MaxNodeLoadValue");
    Common::StringLiteral const Constants::MaxNodeLoadId("MaxNodeLoadId");
    Common::StringLiteral const Constants::CurrentClusterLoad("CurrentClusterLoad");
    Common::StringLiteral const Constants::BufferedClusterCapacityRemaining("BufferedClusterCapacityRemaining");
    Common::StringLiteral const Constants::ClusterCapacityRemaining("ClusterCapacityRemaining");
    Common::StringLiteral const Constants::MaximumNodeLoad("MaximumNodeLoad");
    Common::StringLiteral const Constants::MinimumNodeLoad("MinimumNodeLoad");
    Common::StringLiteral const Constants::OnlyQueryPrimaries("OnlyQueryPrimaries");
    Common::StringLiteral const Constants::UnplacedReplicaDetails("UnplacedReplicaDetails");
    Common::StringLiteral const Constants::Error("Error");
    Common::StringLiteral const Constants::Code("Code");
    Common::StringLiteral const Constants::Message("Message");
    Common::StringLiteral const Constants::ReconfigurationPhase("ReconfigurationPhase");
    Common::StringLiteral const Constants::ReconfigurationType("ReconfigurationType");
    Common::StringLiteral const Constants::ReconfigurationStartTimeUtc("ReconfigurationStartTimeUtc");
    Common::StringLiteral const Constants::ReconfigurationInformation("ReconfigurationInformation");
    Common::StringLiteral const Constants::DeployedServiceReplica("DeployedServiceReplica");
    Common::StringLiteral const Constants::DeployedServiceReplicaInstance("DeployedServiceReplicaInstance");

    Common::StringLiteral const Constants::VersionNumber("VersionNumber");
    Common::StringLiteral const Constants::EpochDataLossNumber("EpochDataLossNumber");
    Common::StringLiteral const Constants::EpochConfigurationNumber("EpochConfigurationNumber");
    Common::StringLiteral const Constants::StoreFiles("StoreFiles");
    Common::StringLiteral const Constants::StoreFolders("StoreFolders");
    Common::StringLiteral const Constants::StoreRelativePath("StoreRelativePath");
    Common::StringLiteral const Constants::FileCount("FileCount");
    Common::StringLiteral const Constants::FileSize("FileSize");
    Common::StringLiteral const Constants::FileVersion("FileVersion");
    Common::StringLiteral const Constants::ModifiedDate("ModifiedDate");
    Common::StringLiteral const Constants::RemoteLocation("RemoteLocation");
    Common::StringLiteral const Constants::RemoteSource("RemoteSource");
    Common::StringLiteral const Constants::RemoteDestination("RemoteDestination");
    Common::StringLiteral const Constants::SkipFiles("SkipFiles");
    Common::StringLiteral const Constants::CopyFlag("CopyFlag");
    Common::StringLiteral const Constants::CheckMarkFile("CheckMarkFile");
    Common::StringLiteral const Constants::StartPosition("StartPosition");
    Common::StringLiteral const Constants::EndPosition("EndPosition");
    Common::StringLiteral const Constants::SessionId("SessionId");
    Common::StringLiteral const Constants::ExpectedRanges("ExpectedRanges");
    Common::StringLiteral const Constants::UploadSessions("UploadSessions");
    Common::StringLiteral const Constants::IsRecursive("IsRecursive");
    Common::StringLiteral const Constants::ChunkContent("ChunkContent");
    
    // Health Reporting
    // HealthReport SourceId
    GlobalString const Constants::HealthReportFMMSource = make_global<std::string>("System.FMM");
    GlobalString const Constants::HealthReportFMSource = make_global<std::string>("System.FM");
    GlobalString const Constants::HealthReportPLBSource = make_global<std::string>("System.PLB");
    GlobalString const Constants::HealthReportCRMSource = make_global<std::string>("System.CRM");
    GlobalString const Constants::HealthReportRASource = make_global<std::string>("System.RA");
    GlobalString const Constants::HealthReportRAPSource = make_global<std::string>("System.RAP");
    GlobalString const Constants::HealthReportFabricNodeSource = make_global<std::string>("System.FabricNode");
    GlobalString const Constants::HealthReportCMSource = make_global<std::string>("System.CM");
    GlobalString const Constants::HealthReportTestabilitySource = make_global<std::string>("System.Testability");
    GlobalString const Constants::HealthReportFederationSource = make_global<std::string>("System.Federation");
    GlobalString const Constants::HealthReportHostingSource = make_global<std::string>("System.Hosting");
    GlobalString const Constants::HealthReportReplicatorSource = make_global<std::string>("System.Replicator");
    GlobalString const Constants::HealthReportReplicatedStoreSource = make_global<std::string>("System.ReplicatedStore");
    GlobalString const Constants::HealthReportNamingServiceSource = make_global<std::string>("System.NamingService");
    GlobalString const Constants::HealthReportHMSource = make_global<std::string>("System.HM");
    GlobalString const Constants::HealthReportTransactionalReplicatorSource = make_global<std::string>("TransactionalReplicator");

    // HealthReport Property
    GlobalString const Constants::ReconfigurationProperty = make_global<std::string>("Reconfiguration");
    GlobalString const Constants::RebuildProperty = make_global<std::string>("Rebuild");
    GlobalString const Constants::HealthStateProperty = make_global<std::string>("State");
    GlobalString const Constants::HealthActivationProperty = make_global<std::string>("Activation");
    GlobalString const Constants::HealthCapacityProperty = make_global<std::string>("Capacity_");
    GlobalString const Constants::ServiceReplicaUnplacedHealthProperty = make_global<std::string>("ServiceReplicaUnplacedHealth_");
    GlobalString const Constants::ReplicaConstraintViolationProperty = make_global<std::string>("ReplicaConstraintViolation_");
    GlobalString const Constants::FabricCertificateProperty = make_global<std::string>("Certificate_");
    GlobalString const Constants::FabricCertificateRevocationListProperty = make_global<std::string>("CertificateRevocationList_");
    GlobalString const Constants::SecurityApiProperty = make_global<std::string>("SecurityApi_");
    GlobalString const Constants::NeighborhoodProperty = make_global<std::string>("Neighborhood_");
    GlobalString const Constants::HealthReplicaOpenStatusProperty = make_global<std::string>("ReplicaOpenStatus");
    GlobalString const Constants::HealthReplicaCloseStatusProperty = make_global<std::string>("ReplicaCloseStatus");
    GlobalString const Constants::HealthReplicaServiceTypeRegistrationStatusProperty = make_global<std::string>("ReplicaServiceTypeRegistrationStatus");
    GlobalString const Constants::HealthRAStoreProvider = make_global<std::string>("RAStoreProvider");
    GlobalString const Constants::HealthReplicaChangeRoleStatusProperty = make_global<std::string>("ReplicaChangeRoleStatus");
    GlobalString const Constants::UpgradePrimarySwapHealthProperty = make_global<std::string>("UpgradePrimarySwapFailure_");
    GlobalString const Constants::BalancingUnsuccessfulProperty = make_global<std::string>("BalancingUnsuccessful_");
    GlobalString const Constants::ConstraintFixUnsuccessfulProperty = make_global<std::string>("ConstraintFixUnsuccessful_");
    GlobalString const Constants::ServiceDescriptionHealthProperty = make_global<std::string>("ServiceDescription");
    GlobalString const Constants::CommitPerformanceHealthProperty = make_global<std::string>("CommitPerformance");
    GlobalString const Constants::MovementEfficacyProperty = make_global<std::string>("MovementEfficacy_");
    GlobalString const Constants::DurationHealthProperty = make_global<std::string>("Duration_");
    GlobalString const Constants::AuthorityReportProperty = make_global<std::string>("AuthorityReport");
    GlobalString const Constants::ResourceGovernanceReportProperty = make_global<std::string>("ResourceGovernance");
    GlobalString const Constants::HealthReportCountProperty = make_global<std::string>("HealthReportCount");

    // KVS replica query
    Common::StringLiteral const Constants::DatabaseRowCountEstimate("DatabaseRowCountEstimate");
    Common::StringLiteral const Constants::DatabaseLogicalSizeEstimate("DatabaseLogicalSizeEstimate");
    Common::StringLiteral const Constants::CopyNotificationCurrentKeyFilter("CopyNotificationCurrentKeyFilter");
    Common::StringLiteral const Constants::CopyNotificationCurrentProgress("CopyNotificationCurrentProgress");
    Common::StringLiteral const Constants::StatusDetails("StatusDetails");
    Common::StringLiteral const Constants::MigrationStatus("MigrationStatus");
    Common::StringLiteral const Constants::CurrentPhase("CurrentPhase");
    Common::StringLiteral const Constants::NextPhase("NextPhase");
    Common::StringLiteral const Constants::ProviderKind("ProviderKind");

    Common::StringLiteral const Constants::TestCommandState("TestCommandState");
    Common::StringLiteral const Constants::TestCommandType("TestCommandType");

    Guid const Constants::FMServiceGuid("00000000-0000-0000-0000-000000000001");

    GlobalString const Constants::EmptyString = make_global<std::string>();

    //
    // FaultAnalysisService
    //
    Common::StringLiteral const Constants::OperationId("OperationId");
    Common::StringLiteral const Constants::PartitionSelectorType("PartitionSelectorType");
    Common::StringLiteral const Constants::PartitionKey("PartitionKey");
    Common::StringLiteral const Constants::RestartPartitionMode("RestartPartitionMode");
    Common::StringLiteral const Constants::DataLossMode("DataLossMode");
    Common::StringLiteral const Constants::QuorumLossMode("QuorumLossMode");
    Common::StringLiteral const Constants::QuorumLossDurationInSeconds("QuorumLossDurationInSeconds");


    Common::StringLiteral const Constants::InvokeDataLossResult("InvokeDataLossResult");
    Common::StringLiteral const Constants::InvokeQuorumLossResult("InvokeQuorumLossResult");
    Common::StringLiteral const Constants::RestartPartitionResult("RestartPartitionResult");
    Common::StringLiteral const Constants::ErrorCode("ErrorCode");
    Common::StringLiteral const Constants::SelectedPartition("SelectedPartition");
    Common::StringLiteral const Constants::Force("Force");
    Common::StringLiteral const Constants::NodeTransitionResult("NodeTransitionResult");

    Common::StringLiteral const Constants::RestartNodeCommand("RestartNode");
    Common::StringLiteral const Constants::StopNodeCommand("StopNode");

    //
    // UpgradeOrchestrationService
    //
    Common::StringLiteral const Constants::ClusterConfig("ClusterConfig");
    Common::StringLiteral const Constants::HealthCheckRetryTimeout("HealthCheckRetryTimeout");
    Common::StringLiteral const Constants::HealthCheckWaitDurationInSeconds("HealthCheckWaitDurationInSeconds");
    Common::StringLiteral const Constants::HealthCheckStableDurationInSeconds("HealthCheckStableDurationInSeconds");
    Common::StringLiteral const Constants::UpgradeDomainTimeoutInSeconds("UpgradeDomainTimeoutInSeconds");
    Common::StringLiteral const Constants::UpgradeTimeoutInSeconds("UpgradeTimeoutInSeconds");
    Common::StringLiteral const Constants::RollbackOnFailure("RollbackOnFailure");
    Common::StringLiteral const Constants::ClusterConfiguration("ClusterConfiguration");
    Common::StringLiteral const Constants::ServiceState("ServiceState");

    Common::StringLiteral const Constants::CurrentCodeVersion("CurrentCodeVersion");
    Common::StringLiteral const Constants::CurrentManifestVersion("CurrentManifestVersion");
    Common::StringLiteral const Constants::TargetCodeVersion("TargetCodeVersion");
    Common::StringLiteral const Constants::TargetManifestVersion("TargetManifestVersion");
    Common::StringLiteral const Constants::PendingUpgradeType("PendingUpgradeType");

    Common::StringLiteral const Constants::NodeResult("NodeResult");

    //
    // Compose Deployment
    //
    Common::StringLiteral const Constants::RegistryUserName("RegistryUserName");
    Common::StringLiteral const Constants::RegistryPassword("RegistryPassword");
    Common::StringLiteral const Constants::PasswordEncrypted("PasswordEncrypted");
    Common::StringLiteral const Constants::ComposeFileContent("ComposeFileContent");
    Common::StringLiteral const Constants::RegistryCredential("RegistryCredential");
    // Deprecated
    Common::StringLiteral const Constants::RepositoryUserName("RepositoryUserName");
    Common::StringLiteral const Constants::RepositoryPassword("RepositoryPassword");
    Common::StringLiteral const Constants::RepositoryCredential("RepositoryCredential");

    Common::StringLiteral const Constants::ComposeDeploymentStatus("ComposeDeploymentStatus");

    //
    // Mesh
    //
    Common::StringLiteral const Constants::deploymentName("deploymentName");
    Common::StringLiteral const Constants::applicationName("applicationName");
    Common::StringLiteral const Constants::applicationUri("applicationUri");
    Common::StringLiteral const Constants::status("status");
    Common::StringLiteral const Constants::statusDetails("statusDetails");    
    Common::StringLiteral const Constants::properties("properties");
    Common::StringLiteral const Constants::instanceCount("instanceCount");
    Common::StringLiteral const Constants::containerRegistryServer("server");
    Common::StringLiteral const Constants::containerRegistryUserName("username");
    Common::StringLiteral const Constants::containerRegistryPassword("password");
    Common::StringLiteral const Constants::restartPolicy("restartPolicy");
    Common::StringLiteral const Constants::volumes("volumes");
    Common::StringLiteral const Constants::image("image");
    Common::StringLiteral const Constants::command("command");
    Common::StringLiteral const Constants::ports("ports");
    Common::StringLiteral const Constants::environmentVariables("environmentVariables");
    Common::StringLiteral const Constants::instanceView("instanceView");
    Common::StringLiteral const Constants::restartCount("restartCount");
    Common::StringLiteral const Constants::currentState("currentState");
    Common::StringLiteral const Constants::previousState("previousState");
    Common::StringLiteral const Constants::state("state");
    Common::StringLiteral const Constants::exitCode("exitCode");
    Common::StringLiteral const Constants::resources("resources");
    Common::StringLiteral const Constants::resourcesRequests("requests");
    Common::StringLiteral const Constants::resourcesLimits("limits");
    Common::StringLiteral const Constants::volumeMounts("volumeMounts");
    Common::StringLiteral const Constants::memoryInGB("memoryInGB");
    Common::StringLiteral const Constants::cpu("cpu");
    Common::StringLiteral const Constants::mountPath("mountPath");
    Common::StringLiteral const Constants::readOnly("readOnly");
    Common::StringLiteral const Constants::imageRegistryCredentials("imageRegistryCredentials");
    Common::StringLiteral const Constants::port("port");
    Common::StringLiteral const Constants::creationParameters("creationParameters");
    Common::StringLiteral const Constants::nameCamelCase("name");
    Common::StringLiteral const Constants::valueCamelCase("value");
    Common::StringLiteral const Constants::azureFile("azureFile");
    Common::StringLiteral const Constants::shareName("shareName");
    Common::StringLiteral const Constants::storageAccountName("storageAccountName");
    Common::StringLiteral const Constants::storageAccountKey("storageAccountKey");
    Common::StringLiteral const Constants::accountName("accountName");
    Common::StringLiteral const Constants::accountKey("accountKey");
    Common::StringLiteral const Constants::sizeDisk("sizeDisk");
    Common::StringLiteral const Constants::volumeName("volumeName");
    Common::StringLiteral const Constants::volumeDescription("description");
    Common::StringLiteral const Constants::volumeDescriptionForImageBuilder("volumeDescription");
    Common::StringLiteral const Constants::volumeParameters("parameters");
    Common::StringLiteral const Constants::osType("osType");

    //
    // In the mesh environment we use the given service name and application name to generate the dns name. The behavior of dns queries 
    // across operating system's when 'dots' are present in the dns names is not consistent. So we prevent users from specifying 'dot' in
    // the service name and application name.
    // Eg: Linux behavior can be read here(https://linux.die.net/man/5/resolv.conf).
    //
    Common::GlobalString const Constants::ModelV2ReservedDnsNameCharacters = make_global<std::string>(".");

    Common::StringLiteral const Constants::RemoveServiceFabricRuntimeAccess("RemoveServiceFabricRuntimeAccess");
    Common::StringLiteral const Constants::AzureFilePluginName("AzureFilePluginName");

    Common::GlobalString const Constants::SystemMetricNameCpuCores = make_global<std::string>("servicefabric:/_CpuCores");
    Common::GlobalString const Constants::SystemMetricNameMemoryInMB = make_global<std::string>("servicefabric:/_MemoryInMB");
    uint const Constants::ResourceGovernanceCpuCorrectionFactor(1000000);

    Common::Global<vector<string>> const Constants::ValidNodeIdGeneratorVersionList = [] {
        Global<vector<string>> v = make_global<vector<string>>();
        v->push_back("v3");
        v->push_back("V3");
        v->push_back("v4");
        v->push_back("V4");
        return v;
    } ();

    //
    // ContinuationToken Constants
    //
    string const Constants::ContinuationTokenParserDelimiter("+");
    string const Constants::ContinuationTokenCreationFailedMessage("ContinuationTokenUnableToBeCreated");

    DEFINE_TEST_MUTABLE_CONSTANT(size_t, NamedPropertyMaxValueSize, 1 * 1024 * 1024)
}

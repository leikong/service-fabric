// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

#include "Common/Common.h"
#include "Transport/Transport.h"
#include <schannel.h>
#include "LeaseAgentInstance.h"
#include "Lease/inc/public/leaselayerinc.h"

#define DISK_PROBE_FILE_NAME "LeaseHeartbeat.tmp"
#define DISK_PROBE_REOPEN_FILE_NAME "LeaseHeartbeatReopen.tmp"

// TODO: LeaseAgent and LeasePartner Source Code will be moving to ~src\Lease\Api directory later.

namespace LeaseWrapper
{
    class ILeasingApplication
    {

    public:

        ILeasingApplication( bool supportArbitrate = false ):
            supportArbitrate_ ( supportArbitrate )
        {
        }

        virtual ~ILeasingApplication()
        {
        }

        virtual void OnLeaseFailed() = 0;
        virtual void OnRemoteLeasingApplicationFailed( std::string const & id ) = 0;
        virtual void Arbitrate(
            LeaseAgentInstance const & local,
            Common::TimeSpan localTTL,
            LeaseAgentInstance const & remote,
            Common::TimeSpan remoteTTL,
            USHORT remoteVersion,
            LONGLONG monitorLeaseInstance,
            LONGLONG subjectLeaseInstance,
            LONG remoteArbitrationDurationUpperBound) = 0;

    private:
        bool supportArbitrate_;

    };

    struct LeaseAgentConfiguration
    {
    public:
        LeaseAgentConfiguration();

        LeaseAgentConfiguration(
            std::string const & leasingApplicationId,
            std::string const & localLeaseAddress, 
            bool enableArbitrate = false);

        LeaseAgentConfiguration(LeaseAgentConfiguration const & rhs);

        std::string leasingApplicationId_; // The ID of this Lease Agent that would be registered with the Lease Layer
        std::string localLeaseAddress_; // The socket address at which the listener for the lease agent is listening.
        Common::TimeSpan leaseSuspendTimeout_; // Short arbitration timeout
        Common::TimeSpan arbitrationTimeout_; // Long arbitration timeout
        int leaseRetryCount_; // Number of renew times in a lease duration
        int LeaseRenewBeginRatio_; // Starting point of lease retry; calculated as (lease duration / #)
        Common::TimeSpan leasingAppExpiryTimeout_; // remaining lease TTL defined in federation config
        bool arbitrationEnabled_; // Arbitration is now optional; This flag indicates whether or not to support it;
        Common::TimeSpan MaxIndirectLeaseTimeout_; // Upper limit that lease renewed by indirect lease
    };

#ifndef PLATFORM_UNIX
    struct LeaseCertHashStore
    {
        BYTE ShaHash[20];
        CHAR pwszStoreName[SCH_CRED_MAX_STORE_NAME_SIZE];
    };
#endif

    class LeasePartner;
    typedef std::shared_ptr<LeasePartner> LeasePartnerSPtr;

    class LeaseAgent: public Common::RootedObject, public Common::FabricComponent, public Common::TextTraceComponent<Common::TraceTaskCodes::LeaseAgent>
    {
    public:
        LeaseAgent(Common::ComponentRoot const & root, ILeasingApplication & leasingApp, LeaseAgentConfiguration const & leaseAgentConfig, Transport::SecuritySettings const& securitySettings);

        ~LeaseAgent()     { }

#if !defined(PLATFORM_UNIX)
        static DWORD LoadRemoteCertificate(DWORD cbCertificate, PBYTE pbCertificate, PCCERT_CONTEXT *ppCertContext);
#endif
        static void SetHealthReportCallback(std::function<void(int, string const &, string const &)> & callback);
#if !defined(PLATFORM_UNIX)
        static void InvokeHealthReportCallback(int reportCode, LPCSTR dynamicProperty, LPCSTR extraDescription);
#endif

        Common::ErrorCode OnOpen();
        Common::ErrorCode OnClose();
        void OnAbort();
        Common::ErrorCode Restart(std::string const & newLeasingApplicationId);

        __declspec (property(get=getInstanceId)) int64 InstanceId;
        int64 getInstanceId() const { return instanceId_; }

        Common::ErrorCode SetSecurity(Transport::SecuritySettings const & securitySettings);

        _Success_(return)
        static bool InitializeListenEndpoint(
            _Inout_opt_ LeaseAgent* leaseAgent,
            std::string const & localLeaseAddress,
            _Out_ TRANSPORT_LISTEN_ENDPOINT & endPoint);

        _Success_(return)
        bool InitializeListenEndpoint(std::string const & localLeaseAddress, _Out_ TRANSPORT_LISTEN_ENDPOINT & endPoint);

        void InitializeLeaseConfigDurations(LEASE_CONFIG_DURATIONS & durations);

        //
        // Operations
        //

        // Sync - Don't wait for Establish Reply
        void Establish(
            std::string const & remoteLeasingApplicationId,
            std::string const & remoteFaultDomain,
            std::string const & remoteLeaseAddress, 
            int64 remoteLeaseAgentInstanceId,
            LEASE_DURATION_TYPE leaseTimeoutType);

        // Async Op
        Common::AsyncOperationSPtr BeginEstablish(
            std::string const & remoteLeasingApplicationId,
            std::string const & remoteFaultDomain,
            std::string const & remoteLeaseAgentAddress,
            int64 remoteLeaseAgentInstanceId,
            LEASE_DURATION_TYPE leaseTimeoutType,
            Common::TimeSpan timeout,
            Common::AsyncCallback const & callback, 
            Common::AsyncOperationSPtr const & context);

        static Common::ErrorCode EndEstablish(__in Common::AsyncOperationSPtr const & operation);

        void Terminate(std::string const & remoteLeasingApplicationId);

        BOOL GetLeasingApplicationExpirationTime(PLONG milliseconds, PLONGLONG kernelCurrentTime) const;
        BOOL GetLeasingApplicationExpirationTimeFromIPC(PLONG milliseconds, PLONGLONG kernelCurrentTime) const;
        static BOOL GetLeasingApplicationExpirationTime(HANDLE appHandle, PLONG milliseconds, PLONGLONG kernelCurrentTime);
        BOOL SetGlobalLeaseExpirationTime(LONGLONG expireTime);

        BOOL GetRemoteLeaseExpirationTime(std::string const & remoteLeasingApplicationId, Common::StopwatchTime & monitorExpireTime, Common::StopwatchTime & subjectExpireTime);

        //
        // Callbacks from the driver.
        //
        void OnLeaseFailed();
        void OnRemoteLeasingApplicationFailed( std::string const & remoteLeasingApplicationId );
        void OnLeaseEstablished( std::string remoteLeasingApplicationId, HANDLE leaseHandle);
        void OnCompleteRemoteCertificateVerification(DWORD cbCertificate, PBYTE pbCertificate, PVOID RemoteCertVerifyCallbackOperation);
        void Arbitrate(
            HANDLE appHandle,
            int64 localInstance,
            long localTTL,
            std::string const & remoteLeaseAddress,
            int64 remoteInstance,
            long remoteTTL,
            USHORT remoteVersion,
            int64 monitorLeaseInstance,
            int64 subjectLeaseInstance,
            LONG remoteArbitrationDurationUpperBound);

        void RetryEstablish(LeasePartnerSPtr const & leaseParnter, LEASE_DURATION_TYPE leaseTimeoutType);

        void CompleteArbitrationSuccessProcessing(LONGLONG localInstance, __in LPCSTR remoteLeaseAddress, LONGLONG remoteInstance, Common::TimeSpan localTTL, Common::TimeSpan remoteTTL, BOOL isDelayed);
        // Chl = caller holding lock

        BOOL CompleteRemoteCertificateVerification(PVOID RemoteCertVerifyCallbackOperation, NTSTATUS verifyResult);
#if !defined(PLATFORM_UNIX)
        BOOL UpdateLeaseGlobalConfig();
#endif

        BOOL UpdateLeaseDuration();

        __declspec (property(get=getIsLeaseExpired)) bool IsLeaseExpired;
        bool getIsLeaseExpired();
        __declspec(property(get=get_LeaseHandle)) HANDLE LeaseHandle;
        HANDLE get_LeaseHandle() const { return appHandle_; }

        bool IsDummyLeaseDriverEnabled() const
        {
            return dummyLeaseDriverEnabled_;
        }

        Common::TimeSpan LeaseDuration() const;

        Common::TimeSpan LeaseSuspendTimeout() const
        {
            return leaseAgentConfig_.leaseSuspendTimeout_;
        }

        Common::TimeSpan ArbitrationTimeout() const
        {
            return leaseAgentConfig_.arbitrationTimeout_;
        }

        Common::ComponentRootSPtr const & GetOwner() { return pRoot_; }

        void WriteTo(Common::TextWriter&, Common::FormatOptions const &) const;

    private:
        typedef std::map<std::string, LeasePartnerSPtr> LeasePartnerMap;

        void Cleanup(bool isDelayed);
        HANDLE InternalClose();

        bool InitializeCertHashStore(Transport::SecuritySettings const & securitySettings);
        Transport::TransportSecuritySPtr LeaseSecurity();

        LeasePartner* GetLeasePartnerByAddress(std::string const & remoteLeaseAddress);

        // Wrappers around lease functions to call dummy or real lease agents dependign upon config.
        HANDLE RegisterLeasingApplication(Transport::SecuritySettings const & securitySettings);
        void UnregisterLeasingApplication(HANDLE handle, std::string const & leasingApplicationId, BOOL isDelayed);
        BOOL CompleteArbitrationSuccessProcessing(HANDLE appHandle, LONGLONG localInstance, __in LPCSTR remoteLeaseAddress, LONGLONG remoteInstance, LONG localTTL, LONG remoteTTL, BOOL isDelayed);

        // For security
        BOOL UpdateCertificate();
        BOOL SetSecurityDescriptor();

        void TryStartHeartbeat();

        // Corresponding to the Owner of this Lease Agent
        LeaseAgentConfiguration leaseAgentConfig_;
        ILeasingApplication & leasingApp_; // Owner; The Leasing Application to which this agent belongs.
        Common::ComponentRootSPtr pRoot_; // Required for controling the life-time of Leasing Application

        // Lease Agent functionality
        HANDLE appHandle_; // The handle to the drivers datastructure for this site.
        HANDLE leaseAgentHandle_; // The handle to the drivers lease agent for this site.
        int64 instanceId_; // Instance of kernel lease agent.
        mutable bool calledFromIPC_; // Ever called from IPC.
        LONGLONG expiryTime_; // The expiry time for this lease agent and hence this site node.
        Common::RwLock leaseAgentLock_; // Lock for ensuring serialization of data structure operations.
        static Common::Global<Common::RwLock> healthReportGlobalLock_;

        // Data-Structures
        LeasePartnerMap leases_; // The state of the various leases held by this agent

        bool dummyLeaseDriverEnabled_; // Dummy IOCTLs or Lease Driver IOCTLs

        Transport::ResolveOptions::Enum addressResolveType_;

        // Security
        Transport::SecuritySettings initialSecuritySetting_;
        Common::RwLock leaseSecurityLock_;
        Transport::TransportSecuritySPtr leaseSecurity_;

#ifndef PLATFORM_UNIX
        LeaseCertHashStore certHash_;
#endif

        Common::HHandler leaseDurationChangeHandler_;
        Common::HHandler leaseDurationAcrossFDChangeHandler_;
        Common::HHandler unresponsiveDurationChangeHandler_;
#if !defined(PLATFORM_UNIX)
        Common::HHandler maintenanceIntervalChangeHandler_;
        Common::HHandler processAssertExitTimeoutChangeHandler_;
        Common::HHandler delayLeaseAgentCloseIntervalChangeHandler_;
#endif
        Common::TimerSPtr heartbeatTimer_; // Timer for disk probing
        HANDLE diskProbeFileHandle_; // File handle for disk probing
        Common::ByteBuffer diskProbeBuffer_; // Disk probing buffer
        static std::function<void(int, string const &, string const &)> healthReportCallback_;
        Common::Random random_;
    };
}

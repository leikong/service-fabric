// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#ifdef UNIFY
#define UPASSTHROUGH 1
#endif

#include "KtlLogShimKernel.h"

OverlayLog::~OverlayLog()
{
#if !defined(PLATFORM_UNIX)
    KInvariant(! _PerfCounterSetInstance);
#endif
}

OverlayLog::OverlayLog(
    __in OverlayManager& OM,
    __in RvdLogManager& BaseLogManager,
    __in const KGuid& DiskId,
    __in KString const * const Path,
    __in const KtlLogContainerId& ContainerId,
    __in ThrottledKIoBufferAllocator& ThrottledAllocator
    ) : _OverlayManager(&OM),
        _StreamsTable(GetThisAllocator(),
                      GetThisAllocationTag()),
        _AliasToStreamTable(GetThisAllocator(),
                    GetThisAllocationTag()),
        _StreamToAliasTable(GetThisAllocator(),
                    GetThisAllocationTag()),
        _BaseLogManager(&BaseLogManager),
        _DiskId(DiskId),
        _Path(Path),
        _ContainerId(ContainerId),
        _ContainerUsers(KtlLogContainerKernel::OverlayLinkageOffset),
        _TotalDedicatedSpace(0),
#if !defined(PLATFORM_UNIX)
        _PerfCounterSetInstance(nullptr),
#endif
        _ThrottledAllocator(&ThrottledAllocator),
        _ThrottledWritesList(OverlayStream::_ThrottleLinkageOffset)
{
    NTSTATUS status;
    
    _ObjectState = Closed;
    
    //
    // Gather up resources needed such as the OpenLog context
    //
    status = _BaseLogManager->CreateAsyncOpenLogContext(_OpenLogContext);
    if (! NT_SUCCESS(status))
    {
        _OpenLogContext = nullptr;
        SetConstructorStatus(status);
        return; 
    }

    status = KInstrumentedComponent::Create(_LogInstrumentedComponent, GetThisAllocator(), GetThisAllocationTag());
    if (!NT_SUCCESS(status))
    {
        SetConstructorStatus(status);
        return;
    }    
    
    _LCMBInfo = nullptr;
    
    //
    // Allocate a waiter for the base container close event
    //
    status = _BaseLogShutdownEvent.CreateWaitContext(GetThisAllocationTag(),
                                                     GetThisAllocator(),
                                                     _BaseLogShutdownWait);
    if (! NT_SUCCESS(status))
    {
        _OpenLogContext = nullptr;
        _LCMBInfo = nullptr;
        SetConstructorStatus(status);
        return; 
    }
}

OverlayLog::OverlayLog(
    __in RvdLogManager& BaseLogManager,
    __in KGuid& DiskId,
    __in KString const * const Path,
    __in KtlLogContainerId& ContainerId,
    __in ThrottledKIoBufferAllocator& ThrottledAllocator
    ) : _OverlayManager(NULL),
        _StreamsTable(GetThisAllocator(),
                      GetThisAllocationTag()),
        _AliasToStreamTable(GetThisAllocator(),
                    GetThisAllocationTag()),
        _StreamToAliasTable(GetThisAllocator(),
                    GetThisAllocationTag()),
        _BaseLogManager(&BaseLogManager),
        _DiskId(DiskId),
        _Path(Path),
        _ContainerId(ContainerId),
        _ContainerUsers(KtlLogContainerKernel::OverlayLinkageOffset),
        _TotalDedicatedSpace(0),
#if !defined(PLATFORM_UNIX)
        _PerfCounterSetInstance(nullptr),
#endif
        _ThrottledAllocator(&ThrottledAllocator),
        _ThrottledWritesList(OverlayStream::_ThrottleLinkageOffset)
{
    NTSTATUS status;
    
    _ObjectState = Closed;

    status = KInstrumentedComponent::Create(_LogInstrumentedComponent, GetThisAllocator(), GetThisAllocationTag());
    if (!NT_SUCCESS(status))
    {
        SetConstructorStatus(status);
        return;
    }    
    
    //
    // Gather up resources needed such as the OpenLog context
    //
    status = _BaseLogManager->CreateAsyncOpenLogContext(_OpenLogContext);
    if (! NT_SUCCESS(status))
    {
        _OpenLogContext = nullptr;
        SetConstructorStatus(status);
        return; 
    }

    _LCMBInfo = nullptr;
    
    //
    // Allocate a waiter for the base container close event
    //
    status = _BaseLogShutdownEvent.CreateWaitContext(GetThisAllocationTag(),
                                                     GetThisAllocator(),
                                                     _BaseLogShutdownWait);
    if (! NT_SUCCESS(status))
    {
        _OpenLogContext = nullptr;
        _LCMBInfo = nullptr;
        SetConstructorStatus(status);
        return; 
    }
    
}

NTSTATUS
OverlayLog::CreateUnderlyingService(
    __out WrappedServiceBase::SPtr& Context,    
    __in ServiceWrapper& FreeService,
    __in ServiceWrapper::Operation& OperationOpen,
    __in KAllocator& Allocator,
    __in ULONG AllocationTag
)
{
    UNREFERENCED_PARAMETER(OperationOpen);
    
    NTSTATUS status;
    OverlayLogFreeService::OperationOpen* op;
    OverlayLogFreeService* oc;
    OverlayLog::SPtr context;

    oc = (OverlayLogFreeService*)&FreeService;
    op = (OverlayLogFreeService::OperationOpen*)&OperationOpen;
    
    context = _new(AllocationTag, Allocator) OverlayLog(oc->GetOverlayManager(),
                                                        *oc->GetBaseLogManager(),
                                                        op->GetDiskId(),
                                                        op->GetPath().RawPtr(),
                                                        oc->GetContainerId(),
                                                        *oc->GetOverlayManager().GetThrottledAllocator()
                                                        );
    if (context == nullptr)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        KTraceOutOfMemory( 0, status, context.RawPtr(), AllocationTag, 0);
        return(status);
    }

    status = context->Status();
    if (!NT_SUCCESS(status))
    {
        return(status);
    }

    Context = context.RawPtr();
    
    return(STATUS_SUCCESS);     
}

//
// Open Service
//
VOID
OverlayLog::StreamRecoveryCompletion(
    __in_opt KAsyncContextBase* const ParentAsync,
    __in KAsyncContextBase& Async
    )
{
    SharedLCMBInfoAccess::LCEntryEnumAsync* enumAsync;
    NTSTATUS status = Async.Status();

    //
    // This is the completion when the stream recovery succeeds and the stream is not deleted
    //
    enumAsync = (SharedLCMBInfoAccess::LCEntryEnumAsync*)ParentAsync;
    SharedLCMBInfoAccess::SharedLCEntry& entry = enumAsync->GetEntry();
    
    if (NT_SUCCESS(status))
    {
        //
        // Process aliases and create entries for each stream
        //
        if (entry.AliasName[0] != 0)
        {
            KString::SPtr alias = KString::Create(entry.AliasName,
                                                  GetThisAllocator());
            if (alias)
            {
                AddOrUpdateAlias(*alias,
                                 entry.LogStreamId);
            } else {
                status = STATUS_INSUFFICIENT_RESOURCES;
                KTraceOutOfMemory( 0, status, 0, 0, 0);
            }
        }   
    } else {
        NTSTATUS statusDontCare;
        BOOLEAN freed;
        OverlayStreamFreeService::SPtr overlayStreamFS;

        statusDontCare = _StreamsTable.FindOverlayStream(entry.LogStreamId, overlayStreamFS);
        KInvariant(NT_SUCCESS(statusDontCare));

        if (NT_SUCCESS(statusDontCare))
        {
            KDbgPrintf("Failed to open stream Status %x. Possible Data Loss !! LogId %08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x \n", 
                status,
                entry.LogStreamId.Get().Data1, 
                entry.LogStreamId.Get().Data2, 
                entry.LogStreamId.Get().Data3, 
                entry.LogStreamId.Get().Data4[0],
                entry.LogStreamId.Get().Data4[1],
                entry.LogStreamId.Get().Data4[2],
                entry.LogStreamId.Get().Data4[3],
                entry.LogStreamId.Get().Data4[4],
                entry.LogStreamId.Get().Data4[5],
                entry.LogStreamId.Get().Data4[6],
                entry.LogStreamId.Get().Data4[7]);

                freed = _StreamsTable.ReleaseOverlayStream(*overlayStreamFS);
                KInvariant(! freed);
        }

        KTraceFailedAsyncRequest(status, this, entry.LogStreamId.Get().Data1, 0);


       if ((status == STATUS_OBJECT_NAME_NOT_FOUND) ||
            (status == STATUS_OBJECT_PATH_NOT_FOUND) ||
            (status == K_STATUS_LOG_STRUCTURE_FAULT))
        {
        //
            // These specific errors indicate that the dedicated log
            // file or directory containing it have been deleted or
            // have been detected as corrupted. In this unfortunate
            // situation there is already data loss
            // and no way the stream will open successfully. Delete
            // the stream in the shared log so its resources can be
            // freed.
            //
            //
            KDbgCheckpointWDataInformational(KLoggerUtilities::ActivityIdFromStreamId(entry.LogStreamId),
                                "OverlayStreamFS Missing stream file", STATUS_SUCCESS,
                            (ULONGLONG)0,
                            (ULONGLONG)0,
                            (ULONGLONG)0,
                            (ULONGLONG)0);

            OverlayStreamFreeService::OperationDelete::SPtr operationDelete;

            operationDelete = _new(GetThisAllocationTag(), GetThisAllocator())
                                                OverlayStreamFreeService::OperationDelete(*overlayStreamFS,
                                                                                        *_BaseLogManager,
                                                                                        *_BaseLogContainer,
                                                                                        *_LCMBInfo); 

            if (! operationDelete)
            {
                KTraceOutOfMemory( 0, STATUS_INSUFFICIENT_RESOURCES, 0, 0, 0);
                enumAsync->CompleteIt(STATUS_INSUFFICIENT_RESOURCES);
                return;
            }

            status = operationDelete->Status();            
            if (! NT_SUCCESS(status))
            {
                KTraceFailedAsyncRequest(status, this, entry.LogStreamId.Get().Data1, 0);
                enumAsync->CompleteIt(status);
                return;
            }

            ServiceWrapper::GlobalContext::SPtr globalContext;
            status = ServiceWrapper::GlobalContext::Create(entry.LogStreamId.Get(),
                                                           GetThisAllocator(),
                                                           GetThisAllocationTag(),
                                                           globalContext);
            
            if (! NT_SUCCESS(status))
            {
                KTraceFailedAsyncRequest(status, this, entry.LogStreamId.Get().Data1, 0);
                enumAsync->CompleteIt(status);
                return;
            }
            
            operationDelete->SetGlobalContext(up_cast<KAsyncGlobalContext, ServiceWrapper::GlobalContext>(globalContext));
            
            KAsyncContextBase::CompletionCallback failedCompletion(this, &OverlayLog::StreamFailedRecoveryCompletion);
            operationDelete->StartDeleteOperation(ParentAsync,
                                                  failedCompletion);           
            return;
        }        		
		
        //
        // Don't fail container open
        //
        status = STATUS_SUCCESS;
    }

    enumAsync->CompleteIt(status);
}

VOID
OverlayLog::StreamFailedRecoveryCompletion(
    __in_opt KAsyncContextBase* const ParentAsync,
    __in KAsyncContextBase& Async
    )
{
    SharedLCMBInfoAccess::LCEntryEnumAsync* enumAsync;
    NTSTATUS status = Async.Status();

    //
    // This is the completion when the stream recovery fails and the stream is deleted
    //

    enumAsync = (SharedLCMBInfoAccess::LCEntryEnumAsync*)ParentAsync;
    
    if (! NT_SUCCESS(status))
    {
        KTraceFailedAsyncRequest(status, this, 0, 0);
    }

    //
    // Remove usage count on the OverlayStreamFreeService
    //
    BOOLEAN freed;
    OverlayStreamFreeService::SPtr overlayStreamFS;
    SharedLCMBInfoAccess::SharedLCEntry& entry = enumAsync->GetEntry();

    status = _StreamsTable.FindOverlayStream(entry.LogStreamId, overlayStreamFS);
    KInvariant(NT_SUCCESS(status));

    if (NT_SUCCESS(status))
    {
        //
        // Deactivate the queue that is part of the OverlayStreamFreeService
        //
        ServiceWrapper::DeactivateQueueCallback queueDeactivated(this,
                                &OverlayLog::StreamFailedRecoveryQueueDeactivated);           
        overlayStreamFS->DeactivateQueue(queueDeactivated);

        //
        // Release one count for the FindOverlayStream above and then one count for the 
        // CreateOrGet in the LCEntry callback below
        //
        freed = _StreamsTable.ReleaseOverlayStream(*overlayStreamFS);
        KInvariant(! freed);
        freed = _StreamsTable.ReleaseOverlayStream(*overlayStreamFS);
        KInvariant(freed);
    } else {
        status = STATUS_SUCCESS;
    }

    enumAsync->CompleteIt(status);
}

VOID
OverlayLog::StreamFailedRecoveryQueueDeactivated(
    __in ServiceWrapper& Service
    )
{
    UNREFERENCED_PARAMETER(Service);

    // Nothing to do....
}


NTSTATUS
OverlayLog::LCEntryCallback(
    __in SharedLCMBInfoAccess::LCEntryEnumAsync& Async
    )
{
    NTSTATUS status;
    OverlayStreamFreeService::SPtr overlayStreamFS;
    BOOLEAN created;
    SharedLCMBInfoAccess::SharedLCEntry& entry = Async.GetEntry();
    KString::SPtr path;

    // NOTE: Any return other than STATUS_PENDING will autocomplete the LCEntryEnumAsync

    if (entry.PathToDedicatedContainer[0] == 0)
    {
        path = nullptr;
    } else {
        path = KString::Create(&entry.PathToDedicatedContainer[0], GetThisAllocator());
        if (! path)
        {
            KTraceOutOfMemory( 0, STATUS_INSUFFICIENT_RESOURCES, 0, 0, 0);
            return(STATUS_INSUFFICIENT_RESOURCES);
        }
    }
    
    status = _StreamsTable.CreateOrGetOverlayStream(FALSE,    // Read Only
                                                    entry.LogStreamId,
                                                    _DiskId,
                                                    path.RawPtr(),
                                                    entry.MaxRecordSize,
                                                    entry.StreamSize,
                                                    entry.MaxLLMetadataSize,
                                                    overlayStreamFS,
                                                    created);

    if (NT_SUCCESS(status))
    {
        if (! created)
        {
            KDbgCheckpointWDataInformational(KLoggerUtilities::ActivityIdFromStreamId(entry.LogStreamId),
                                "OverlayLog::LCEntryCallback, expect unique StreamId", status,
                                (ULONGLONG)0,
                                (ULONGLONG)0,
                                (ULONGLONG)0,
                                (ULONGLONG)0);
            
            _StreamsTable.ReleaseOverlayStream(*overlayStreamFS);

            return(STATUS_SUCCESS);
        }
        
        overlayStreamFS->SetLCMBEntryIndex(entry.Index);

        //
        // Determine what kind of recovery actions are needed
        //
        if (entry.Flags == SharedLCMBInfoAccess::FlagCreated)
        {
            //
            // This log stream looks just fine, perform the normal
            // recovery
            //
#ifdef VERBOSE
            KDbgCheckpointWData(KLoggerUtilities::ActivityIdFromStreamId(entry.LogStreamId),
                                "OverlayStreamFS Recovery", STATUS_SUCCESS,
                                (ULONGLONG)entry.Index,
                                (ULONGLONG)0,
                                (ULONGLONG)0,
                                (ULONGLONG)0);                                
#endif                        
            OverlayStreamFreeService::OperationOpen::SPtr operationOpen;

            operationOpen = _new(GetThisAllocationTag(), GetThisAllocator())
                                                OverlayStreamFreeService::OperationOpen(*overlayStreamFS,
                                                                                        *this,
                                                                                        *_BaseLogManager,
                                                                                        *_BaseLogContainer,
                                                                                        *_LCMBInfo); 

            if (! operationOpen)
            {
                KTraceOutOfMemory( 0, STATUS_INSUFFICIENT_RESOURCES, 0, 0, 0);
                return(STATUS_INSUFFICIENT_RESOURCES);
            }

            status = operationOpen->Status();            
            if (! NT_SUCCESS(status))
            {
                KTraceFailedAsyncRequest(status, this, entry.LogStreamId.Get().Data1, 0);
                return(status);
            }

            KAsyncContextBase::CompletionCallback completion(this, &OverlayLog::StreamRecoveryCompletion);
            operationOpen->StartRecoverOperation(&Async,
                                                 completion);
            return(STATUS_PENDING);
        } else {
            //
            // This is some other type of recovery; end result is
            // deleting the stream
            //
            KInvariant(entry.Flags != SharedLCMBInfoAccess::FlagDeleted);
            
            KDbgCheckpointWDataInformational(KLoggerUtilities::ActivityIdFromStreamId(entry.LogStreamId),
                                "OverlayStreamFS Delete Stream Recovery", STATUS_SUCCESS,
                            (ULONGLONG)entry.Index,
                            (ULONGLONG)entry.Flags,
                            (ULONGLONG)0,
                            (ULONGLONG)0);

            OverlayStreamFreeService::OperationDelete::SPtr operationDelete;

            operationDelete = _new(GetThisAllocationTag(), GetThisAllocator())
                                                OverlayStreamFreeService::OperationDelete(*overlayStreamFS,
                                                                                        *_BaseLogManager,
                                                                                        *_BaseLogContainer,
                                                                                        *_LCMBInfo); 

            if (! operationDelete)
            {
                KTraceOutOfMemory( 0, STATUS_INSUFFICIENT_RESOURCES, 0, 0, 0);
                return(STATUS_INSUFFICIENT_RESOURCES);
            }

            status = operationDelete->Status();            
            if (! NT_SUCCESS(status))
            {
                KTraceFailedAsyncRequest(status, this, entry.LogStreamId.Get().Data1, 0);
                return(status);
            }

            ServiceWrapper::GlobalContext::SPtr globalContext;
            status = ServiceWrapper::GlobalContext::Create(entry.LogStreamId.Get(),
                                                           GetThisAllocator(),
                                                           GetThisAllocationTag(),
                                                           globalContext);
            
            if (! NT_SUCCESS(status))
            {
                KTraceFailedAsyncRequest(status, this, entry.LogStreamId.Get().Data1, 0);
                return(status);
            }
            
            operationDelete->SetGlobalContext(up_cast<KAsyncGlobalContext, ServiceWrapper::GlobalContext>(globalContext));
            
            KAsyncContextBase::CompletionCallback failedCompletion(this, &OverlayLog::StreamFailedRecoveryCompletion);
            operationDelete->StartDeleteOperation(&Async,
                                                  failedCompletion);           
            return(STATUS_PENDING);
        }        
    }

    return(STATUS_SUCCESS);
}

VOID
OverlayLog::OpenServiceFSM(
    __in NTSTATUS Status
    )
{
    KAsyncContextBase::CompletionCallback completion(this, &OverlayLog::OpenServiceCompletion);

    KDbgCheckpointWDataInformational(KLoggerUtilities::ActivityIdFromStreamId(_ContainerId), "OpenServiceFSM", Status,
                        (ULONGLONG)_State,
                        (ULONGLONG)this,
                        (ULONGLONG)0,
                        (ULONGLONG)0);                                
            
    if (! NT_SUCCESS(Status))
    {
        if ((_State != OpenBaseContainer) || (Status != STATUS_OBJECT_NAME_NOT_FOUND))
        {
            KTraceFailedAsyncRequest(Status, this, _State, 0);          
            KDbgPrintf("Failed to open log LogId %08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x \n", 
                _ContainerId.Get().Data1, 
                _ContainerId.Get().Data2, 
                _ContainerId.Get().Data3, 
                _ContainerId.Get().Data4[0],
                _ContainerId.Get().Data4[1],
                _ContainerId.Get().Data4[2],
                _ContainerId.Get().Data4[3],
                _ContainerId.Get().Data4[4],
                _ContainerId.Get().Data4[5],
                _ContainerId.Get().Data4[6],
                _ContainerId.Get().Data4[7]);
        }

        if (_State == OpenBaseContainer)
        {
            //
            // On failure to open base container just propogate the
            // error back
            //
#if !defined(PLATFORM_UNIX)
            if (_PerfCounterSetInstance)
            {
                _PerfCounterSetInstance->Stop();
            }
            _PerfCounterSetInstance = nullptr;
#endif
            _BaseLogContainer = nullptr;
            CompleteOpen(Status);
            return;
        }
        
        if (_State == OpenLCMBInfo)
        {           
            //
            // On failure we need to cleanup any streams that have been
            // accessed.
            //
            _FinalStatus = Status;
            
            _State = CleanupStreams;
            _LastOverlayStreamFS = nullptr;

            // Fall through
        }
    }

    switch (_State)
    {
        case OpenInitial:
        {                   
            //
            // Flag this service for deferred close behavior to ensure
            // that all operations against the container are drained
            // before close process starts
            //
            _FinalStatus = STATUS_SUCCESS;
            
            KInvariant(_ObjectState == Closed);
            
            SetDeferredCloseBehavior();

            KStringView prefix("SharedLogWrite");
            Status = _LogInstrumentedComponent->SetComponentName(prefix, _ContainerId.Get());
            if (! NT_SUCCESS(Status))
            {
                KTraceFailedAsyncRequest(Status, this, _State, 0);
                CompleteOpen(Status);
                return;
            }

            
#if !defined(PLATFORM_UNIX)
            RtlZeroMemory(&_PerfCounterData, sizeof(_PerfCounterData));
            OverlayManager::SPtr overlayManager = GetOverlayManager();
            if (overlayManager)
            {
                Status = overlayManager->CreateNewLogContainerPerfCounterSet(_ContainerId,
                                                                             &_PerfCounterData,
                                                                             _PerfCounterSetInstance);
                if (! NT_SUCCESS(Status))
                {
                    //
                    // Not having perf counters is not fatal
                    //
                    KTraceFailedAsyncRequest(Status, this, _State, 0);
                }
            }
#endif
            
            //
            // First step is to open the base log container
            //
            _State = OpenBaseContainer;
            if (_Path)
            {
                _OpenLogContext->StartOpenLog(*_Path,
                                              _BaseLogContainer,
                                              this,
                                              completion);
            } else {
                _OpenLogContext->StartOpenLog(_DiskId,
                                              _ContainerId,
                                              _BaseLogContainer,
                                              this,
                                              completion);
            }
            break;
        }

        case OpenBaseContainer:
        {
            //
            // Next step is to open the shared log container
            // metadata
            //
            _State = OpenLCMBInfo;


            //
            // Determine the throttle threshold
            //
            ULONGLONG totalSpace, freeSpace;
            _BaseLogContainer->QuerySpaceInformation(&totalSpace,
                                                     &freeSpace);

            //
            // start throttling at 95% or 4MB
            //
            _ThrottleThreshold = totalSpace / KtlLogContainer::ThrottleFactor;
            if (_ThrottleThreshold == 0)
            {
                _ThrottleThreshold = KtlLogContainer::MinimumThrottleAmountInBytes;
            }
            
            //
            // Open up the shared metadata to process each stream
            //
            Status = SharedLCMBInfoAccess::Create(_LCMBInfo,
                                                  _DiskId,
                                                  _Path.RawPtr(),
                                                  _ContainerId,
                                                  _BaseLogContainer->QueryMaxAllowedStreams(),
                                                  GetThisAllocator(),
                                                  GetThisAllocationTag());
            if (! NT_SUCCESS(Status))
            {
                KTraceFailedAsyncRequest(Status, this, _State, 0);
                _BaseLogContainer = nullptr;
#if !defined(PLATFORM_UNIX)
                if (_PerfCounterSetInstance)
                {
                    _PerfCounterSetInstance->Stop();
                }
                _PerfCounterSetInstance = nullptr;
#endif

                CompleteOpen(Status);
                return;
            }

            SharedLCMBInfoAccess::LCEntryEnumCallback lCEntryCallback(this, &OverlayLog::LCEntryCallback);
            _LCMBInfo->StartOpenMetadataBlock(lCEntryCallback,
                                              this,
                                              completion);
            break;
        }

        case OpenLCMBInfo:
        {           
            _LogInstrumentedComponent->SetReportFrequency(KInstrumentedComponent::DefaultReportFrequency);
            
            _ObjectState = Opened;
            CompleteOpen(STATUS_SUCCESS);
            break;
        }


        //
        // On an open failure, cleanup of any OverlayStreamFreeService
        // data structures need to be performed
        //
        case CleanupStreams:
        {
            //
            // remove an old stream free service
            //
            if (_LastOverlayStreamFS)
            {
                BOOLEAN freed = _StreamsTable.ReleaseOverlayStream(*_LastOverlayStreamFS);
                KInvariant(freed);
                _LastOverlayStreamFS = nullptr;
            }

            //
            // Clean up next entries on the global streams table
            //
            KtlLogStreamId key;
            PVOID index;

            Status = _StreamsTable.GetFirstObject(_LastOverlayStreamFS, key, index);

            if (NT_SUCCESS(Status))
            {
                //
                // Stream should not be opened at this point
                //
                KInvariant(! _LastOverlayStreamFS->GetOverlayStream());
                
                ServiceWrapper::DeactivateQueueCallback queueDeactivated(this,
                                                                    &OverlayLog::StreamQueueDeactivatedOnCleanup);
                _LastOverlayStreamFS->DeactivateQueue(queueDeactivated);
                
                return;
            }

            //
            // All streams are closed and removed. Next remove ref to base log container and close out LCMBInfo
            //
            _State = CloseBaseLog;
#if DBG
            _BaseLogContainerRaw = _BaseLogContainer.RawPtr();
#endif

            //
            // Set shutdown event on the log container. It will fire
            // when the container is completely shutdown in the
            // destructor.
            //
            Status = _BaseLogContainer->SetShutdownEvent(&_BaseLogShutdownEvent);
            KInvariant(NT_SUCCESS(Status));
            
            _BaseLogContainer = nullptr;
            _BaseLogShutdownWait->StartWaitUntilSet(this,
                                                    completion);
            return;
        }

        case CloseBaseLog:
        {
#if !defined(PLATFORM_UNIX)
            if (_PerfCounterSetInstance)
            {
                _PerfCounterSetInstance->Stop();
            }
            _PerfCounterSetInstance = nullptr;
#endif
            CompleteOpen(_FinalStatus);
            return;
        }
        
        default:
        {
            KInvariant(FALSE);
        }
    }
}

VOID
OverlayLog::OpenServiceCompletion(
    __in_opt KAsyncContextBase* const ParentAsync,
    __in KAsyncContextBase& Async
    )
{
    UNREFERENCED_PARAMETER(ParentAsync);
    
    NTSTATUS status = Async.Status();
    
    OpenServiceFSM(status);
}

VOID
OverlayLog::StreamQueueDeactivatedOnCleanup(
    __in ServiceWrapper& Service
    )
{
    UNREFERENCED_PARAMETER(Service);
    OpenServiceFSM(STATUS_SUCCESS); 
}

VOID
OverlayLog::OnServiceOpen(
    )
{
    _State = OpenInitial;
    OpenServiceFSM(STATUS_SUCCESS);
}

VOID
OverlayLog::OnServiceReuse(
    )
{
    KInvariant(FALSE);
    
    _LCMBInfo->Reuse();
    _OpenLogContext->Reuse();
}        

//
// Close Service
//
VOID
OverlayLog::CloseServiceFSM(
    __in NTSTATUS Status
    )
{
    KAsyncContextBase::CompletionCallback completion(this, &OverlayLog::CloseServiceCompletion);

    KDbgCheckpointWData(KLoggerUtilities::ActivityIdFromStreamId(_ContainerId), "CloseServiceFSM", Status,
                        (ULONGLONG)_State,
                        (ULONGLONG)this,
                        (ULONGLONG)0,
                        (ULONGLONG)0);                                
                
    if (! NT_SUCCESS(Status))
    {
        KTraceFailedAsyncRequest(Status, this, _State, 0);
        _CloseFSOperation = nullptr;
        _LastOverlayStreamFS = nullptr;
        CompleteClose(Status);
        return;
    }
    
#pragma warning(disable:4127)   // C4127: conditional expression is constant    
    while (TRUE)
    {
        switch (_State)
        {
            case CloseInitial:
            {
                KInvariant(_ObjectState == Opened);

                _State = CleanupStreams;
                _LastOverlayStreamFS = nullptr;
#if !defined(PLATFORM_UNIX)
                if (_PerfCounterSetInstance)
                {
                    _PerfCounterSetInstance->Stop();
                }
                _PerfCounterSetInstance = nullptr;
#endif

                // fall through
            }

            case CleanupStreams:
            {
                //
                // remove an old stream free service
                //
                if (_LastOverlayStreamFS)
                {
                    BOOLEAN freed = _StreamsTable.ReleaseOverlayStream(*_LastOverlayStreamFS);
                    KInvariant(freed);
                    _LastOverlayStreamFS = nullptr;
                }

                //
                // Clean up next entries on the global streams table
                //
                KtlLogStreamId key;
                PVOID index;
                
                Status = _StreamsTable.GetFirstObject(_LastOverlayStreamFS, key, index);

                if (NT_SUCCESS(Status))
                {
                    //
                    // Force close the stream
                    //
                    OverlayStream::SPtr overlayStream = _LastOverlayStreamFS->GetOverlayStream();
                    if (overlayStream)
                    {
                        _CloseFSOperation = nullptr;
                        Status = ServiceWrapper::Operation::Create(_CloseFSOperation,
                                                                 *(static_cast<ServiceWrapper*>(_LastOverlayStreamFS.RawPtr())),
                                                                 GetThisAllocator(),
                                                                 GetThisAllocationTag());

                        if (! NT_SUCCESS(Status))
                        {
                            _CloseFSOperation = nullptr;
                            _LastOverlayStreamFS = nullptr;
                            KTraceFailedAsyncRequest(Status, this, _State, 0);
                            CompleteClose(Status);
                            return;
                        }

                        ServiceWrapper::GlobalContext::SPtr globalContext;
                        Status = ServiceWrapper::GlobalContext::Create(overlayStream->GetStreamId().Get(),
                                                                       GetThisAllocator(),
                                                                       GetThisAllocationTag(),
                                                                       globalContext);

                        if (! NT_SUCCESS(Status))
                        {
                            _CloseFSOperation = nullptr;
                            _LastOverlayStreamFS = nullptr;
                            KTraceFailedAsyncRequest(Status, this, _State, 0);
                            CompleteClose(Status);
                            return;
                        }

                        _CloseFSOperation->SetGlobalContext(up_cast<KAsyncGlobalContext, ServiceWrapper::GlobalContext>(globalContext));

                        _State = ClosedStream;

                        KDbgCheckpointWData(KLoggerUtilities::ActivityIdFromStreamId(overlayStream->GetStreamId()),
                                            "Container closing stream", Status,
                                            (ULONGLONG)overlayStream.RawPtr(),
                                            (ULONGLONG)this,
                                            (ULONGLONG)0,
                                            (ULONGLONG)0);

                        _CloseFSOperation->StartCloseOperation(overlayStream.RawPtr(),
                                                               this,
                                                               completion);         

                        return;
                    } else {
                        //
                        // Stream already closed, jump ahead to
                        // deactivate the queue
                        //
                        _State = ClosedStream;
                        continue;
                    }
                    return;
                }

                //
                // All streams are closed and removed. Next remove ref to base log container and close out LCMBInfo
                //
                _State = CloseLCMBInfo;
                
                _LCMBInfo->Reuse();
                _LCMBInfo->StartCloseMetadataBlock(this, completion);
                return;
            }
            
            case ClosedStream:
            {
                //
                // Stream was closed, deactivate its queue processing
                //
                _State = CleanupStreams;
                        
                ServiceWrapper::DeactivateQueueCallback queueDeactivated(this,
                                                                    &OverlayLog::StreamQueueDeactivated);           
                _LastOverlayStreamFS->DeactivateQueue(queueDeactivated);
                return;
            }

            case CloseLCMBInfo:
            {
                _State = CloseBaseLog;
#if DBG
                _BaseLogContainerRaw = _BaseLogContainer.RawPtr();
#endif
                //
                // Set shutdown event on the log container. It will fire
                // when the container is completely shutdown in the
                // destructor.
                //
                Status = _BaseLogContainer->SetShutdownEvent(&_BaseLogShutdownEvent);
                KInvariant(NT_SUCCESS(Status));
                
                _BaseLogContainer = nullptr;
                _BaseLogShutdownWait->StartWaitUntilSet(this,
                                                        completion);
                return;
            }

            case CloseBaseLog:
            {
                _CloseFSOperation = nullptr;
                _LastOverlayStreamFS = nullptr;
                CompleteClose(STATUS_SUCCESS);
                return;
            }

            default:
            {
                KInvariant(FALSE);
            }
        }
    }
}

VOID
OverlayLog::CloseServiceCompletion(
    __in_opt KAsyncContextBase* const ParentAsync,
    __in KAsyncContextBase& Async
    )
{
    UNREFERENCED_PARAMETER(ParentAsync);

    NTSTATUS status = Async.Status();
    
    CloseServiceFSM(status);
}

VOID
OverlayLog::StreamQueueDeactivated(
    __in ServiceWrapper& Service
    )
{
    UNREFERENCED_PARAMETER(Service);
    CloseServiceFSM(STATUS_SUCCESS); 
}


VOID
OverlayLog::OnServiceClose(
    )
{
    _State = CloseInitial;
    CloseServiceFSM(STATUS_SUCCESS);
}

NTSTATUS
OverlayLog::TryAcquireRequestRef()
{
    NTSTATUS status;
    BOOLEAN acquired;

    acquired = TryAcquireServiceActivity();
    if (acquired)
    {
        status = STATUS_SUCCESS;
    } else {
        status = STATUS_OBJECT_NO_LONGER_EXISTS;
    }

    return(status);
}

VOID
OverlayLog::ReleaseRequestRef()
{
    ReleaseServiceActivity();
}


NTSTATUS
OverlayLog::StartServiceOpen(
    __in_opt KAsyncContextBase* const ParentAsync, 
    __in_opt OpenCompletionCallback OpenCallbackPtr,
    __in_opt KAsyncGlobalContext* const GlobalContextOverride
)
{
    NTSTATUS status;

    status = StartOpen(ParentAsync, OpenCallbackPtr, GlobalContextOverride);

    return(status);
}

NTSTATUS
OverlayLog::StartServiceClose(
    __in_opt KAsyncContextBase* const ParentAsync, 
    __in_opt CloseCompletionCallback CloseCallbackPtr
)
{
    NTSTATUS status;

    status = StartClose(ParentAsync, CloseCallbackPtr);

    return(status);
}

VOID
OverlayLog::AccountForStreamOpen(
    __in OverlayStream& Os
    )
{
    OverlayStream::SPtr os = &Os;

    _TotalDedicatedSpace += os->GetStreamSize();
    RecomputeStreamQuotas(FALSE, _TotalDedicatedSpace);
    os->SetThrottledAllocatorBytes(0);
}

VOID
OverlayLog::AccountForStreamClosed(
    __in OverlayStream& Os
    )
{
    OverlayStream::SPtr os = &Os;

    _TotalDedicatedSpace -= os->GetStreamSize();
    RecomputeStreamQuotas(FALSE, _TotalDedicatedSpace);
}

BOOLEAN
OverlayLog::IsUnderThrottleLimit(
    __out ULONGLONG& TotalSpace,
    __out ULONGLONG& FreeSpace
    )
{
    _BaseLogContainer->QuerySpaceInformation(&TotalSpace,
                                             &FreeSpace);
    return(FreeSpace >= _ThrottleThreshold);
}

BOOLEAN
OverlayLog::ShouldThrottleSharedLog(
    __in OverlayStream::AsyncDestagingWriteContextOverlay& DestagingWriteContext,
    __out ULONGLONG& TotalSpace,
    __out ULONGLONG& FreeSpace
)
{
    if (! IsUnderThrottleLimit(TotalSpace, FreeSpace))
    {
        K_LOCK_BLOCK(_ThrottleWriteLock)
        {
            _ThrottledWritesList.AppendTail(&DestagingWriteContext);
        }
        
        return(TRUE);
    }

    return(FALSE);
}

VOID
OverlayLog::ShouldUnthrottleSharedLog(
    __in BOOLEAN ReleaseOne
)
{    
    BOOLEAN continueUnthrottling = TRUE;
    OverlayStream::AsyncDestagingWriteContextOverlay* throttledWrite = NULL;
    ULONGLONG freeSpace = 0, totalSpace = 0;

    while (continueUnthrottling)
    {
        throttledWrite = NULL;

        K_LOCK_BLOCK(_ThrottleWriteLock)
        {
            throttledWrite = _ThrottledWritesList.PeekHead();

            if (throttledWrite == NULL)
            {
                //
                // If there is nothing on the list then nothing to
                // unthrottle
                //
                return;
            }

            if (ReleaseOne || (IsUnderThrottleLimit(totalSpace, freeSpace)))
            {
                throttledWrite = _ThrottledWritesList.RemoveHead();
            } else {
                // TODO: Remove when stabilized
                KDbgCheckpointWDataInformational(throttledWrite->GetActivityId(), "Didnt Unthrottled write", STATUS_SUCCESS,
                                    (ULONGLONG)throttledWrite, totalSpace,
                                    throttledWrite->GetRecordAsn().Get(), freeSpace);
                return;
            }
            
            ReleaseOne = FALSE;
        }

        // TODO: Remove when stabilized
        KDbgCheckpointWDataInformational(throttledWrite->GetActivityId(), "Unthrottled write", STATUS_SUCCESS,
                            (ULONGLONG)throttledWrite, totalSpace,
                            throttledWrite->GetRecordAsn().Get(), freeSpace);
        throttledWrite->OnStartAllocateBuffer();
    }
}

VOID
OverlayLog::RecomputeStreamQuotas(
    __in BOOLEAN IncludeClosedStreams,
    __in ULONGLONG TotalDedicatedSpace
    )
{
    ULONGLONG totalSharedSpace;
    ULONGLONG freeSharedSpace;
    float scale;

    //
    // If there are no streams open then nothing to do
    //
    if (TotalDedicatedSpace == 0)
    {
        return;
    }
    
    //
    // First obtain the total shared log space
    //
    _BaseLogContainer->QuerySpaceInformation(&totalSharedSpace,
                                             &freeSharedSpace);

    KInvariant(TotalDedicatedSpace != 0);
    scale = (float)totalSharedSpace / (float)TotalDedicatedSpace;
    
    //
    // Next, compute the total space for all streams using the
    // shared log and then go back and assign quotas for each log
    //
    K_LOCK_BLOCK(_StreamsTable.GetLock())
    {
        NTSTATUS status;
        PVOID index;
        KtlLogStreamId key;
        OverlayStreamFreeService::SPtr osfs;
    
        status = _StreamsTable.GetFirstObjectNoLock(osfs,
                                                    key,
                                                    index);
        while (NT_SUCCESS(status))
        {
            OverlayStream::SPtr os = down_cast<OverlayStream>(osfs->GetUnderlyingService());

            if (IncludeClosedStreams || ((osfs->GetObjectState() == ServiceWrapper::Opened) ||
                                         (osfs->GetObjectState() == ServiceWrapper::Opening)))
            {
                float fquota;
                LONGLONG quota;

                fquota = scale * (float)osfs->GetStreamSize();
                quota = (ULONGLONG)fquota;
                if (quota > osfs->GetStreamSize())
                {
                    quota = osfs->GetStreamSize();
                }


                KDbgCheckpointWDataInformational(KLoggerUtilities::ActivityIdFromStreamId(osfs->GetStreamId()),
                                    "OverlayLog::RecomputeStreamQuotas", status,
                                    (ULONGLONG)quota,
                                    (ULONGLONG)0,
                                    (ULONGLONG)0,
                                    (ULONGLONG)0);
                
                
                osfs->SetSharedQuota(quota);
                
                if (os)
                {                
                    os->SetSharedQuota(quota);
                }
                    
            }

            osfs = nullptr;
            status = _StreamsTable.GetNextObjectNoLock(index,
                                                       osfs);
        }               
    }
}

NTSTATUS
OverlayLog::GetStreamQuota(
    __in KtlLogStreamId StreamId,
    __out ULONGLONG& Quota
    )
{
    NTSTATUS status;
    OverlayStreamFreeService::SPtr osfs;

    status = _StreamsTable.LookupObject(osfs,
                                        StreamId);
    if (NT_SUCCESS(status))
    {
        Quota = osfs->GetSharedQuota();
    }

    return(status);
}


// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include <Python.h> 

#include "Common/Common.h"
#include "ScopedPyObject.h"
#include "ScopedGilState.h"
#include "PyInterpreter.h"
#include "PyModuleCache.h"
#include "PyUtils.h"

#define DEFINE_PY_CALLBACK( Name ) \
    PyObject * Global_##Name(PyObject * self, PyObject * args) \
    { \
        return SingletonImpl::GetInstance().GetImpl()->Name(self, args); \
    } \
    void PyInterpreter::Register_##Name(Name##Callback const & callback) \
    { \
        SingletonImpl::GetInstance().GetImpl()->Register_##Name(callback); \
    } \

#define DEFINE_PY_METHOD_ENTRY( Name, Description ) \
    { #Name, Global_##Name, METH_VARARGS, Description }, \

#define DEFINE_PY_CALLBACK_IMPL( Name ) \
    private: \
        PyInterpreter::Name##Callback Name##Callback_; \
    public: \
        void Register_##Name(PyInterpreter::Name##Callback const & callback) \
        { \
            Name##Callback_ = callback; \
        } \
        PyObject * Name(PyObject * self, PyObject * args) \
        { \
            if (Name##Callback_ == nullptr) \
            { \
                PyErr_SetString(PyExc_RuntimeError, "Callback not registered"); \
                return NULL; \
            } \

using namespace Common;
using namespace PyHost;
using namespace std;

StringLiteral const TraceComponent("PyInterpreter");

const string EmbeddedModuleName = "horizon";

//
// Impl
//

PyObject * InitializeGlobalModule();

class Impl
{
public:
    Impl()
        : mainThreadState_(nullptr)
        , moduleCache_()
    {
        Initialize();
    }

    ~Impl()
    {
        Uninitialize();
    }

private:

    void Initialize()
    {
        Trace.WriteInfo(TraceComponent, "Initializing '{0}' module...", EmbeddedModuleName);

        PyImport_AppendInittab(EmbeddedModuleName.c_str(), &InitializeGlobalModule);

        Py_Initialize();

        // This isn't needed on Windows, but on Linux, modules won't load
        // from the current directory without adding "." explicitly.
        //
        PyRun_SimpleString("import sys\nsys.path.append(\".\")");

        if (!PyEval_ThreadsInitialized())
        {
            PyEval_InitThreads();
        }

        mainThreadState_ = PyEval_SaveThread();
    }

    void Uninitialize()
    {
        moduleCache_.Clear();

        Trace.WriteInfo(TraceComponent, "Uninitializing '{0}' module...", EmbeddedModuleName);

        PyEval_RestoreThread(mainThreadState_);
        
        Py_Finalize();
    }

public:

    // 
    // C++ -> Python support
    //

    ErrorCode Execute(
        wstring const & moduleName,
        wstring const & funcName,
        vector<wstring> const & args)
    {
        return ExecuteWrapper(moduleName, funcName, args, true); // strict
    }

    ErrorCode ExecuteIfFound(
        wstring const & moduleName,
        wstring const & funcName,
        vector<wstring> const & args)
    {
        return ExecuteWrapper(moduleName, funcName, args, false); // strict=false
    }

private:

    ErrorCode ExecuteWrapper(
        wstring const & moduleName,
        wstring const & funcName,
        vector<wstring> const & args,
        bool strict)
    {
        try
        {
            Execute_Throws(moduleName, funcName, args, strict);

            return ErrorCodeValue::Success;
        }
        catch (exception & ex)
        {
            Trace.WriteError(TraceComponent, "Execute failed: {0}", ex.what());
            
            return ErrorCode(ErrorCodeValue::OperationFailed, StringUtility::Utf8ToUtf16(ex.what()));
        }
    }

    void Execute_Throws(
        wstring const & moduleNameW,
        wstring const & funcNameW,
        vector<wstring> const & args,
        bool strict)
    {
        Trace.WriteInfo(TraceComponent, "Execute({0}, {1}, {2})", moduleNameW, funcNameW, args);

        auto moduleName = StringUtility::Utf16ToUtf8(moduleNameW);
        auto funcName = StringUtility::Utf16ToUtf8(funcNameW);

        // TODO: Avoid blocking threads
        //
        ScopedGilState gilState;

        auto pName = ScopedPyObject::CreateFromNew(PyUnicode_DecodeFSDefault(moduleName.c_str()));
        if (pName.Get() == nullptr)
        {
            PyUtils::ThrowOnFailure(moduleName, funcName, "PyUnicode_DecodeFSDefault");
        }

        auto pFunc = moduleCache_.GetOrAddFunctionInModule(moduleName, funcName, strict);
        if (pFunc.Get() == nullptr && !strict)
        {
            // no-op on function not found
            return;
        }
        else if (!PyCallable_Check(pFunc.Get()))
        {
            PyUtils::ThrowOnFailure(moduleName, funcName, "PyCallable_Check");
        }

        auto pArgs = ScopedPyObject::CreateFromNew(PyTuple_New(args.size()));
        if (pArgs.Get() == nullptr)
        {
            PyUtils::ThrowOnFailure(moduleName, funcName, "PyTuple_New");
        }

        for (auto ix=0; ix<args.size(); ++ix)
        {
            auto pValue = ScopedPyObject::CreateFromNew(PyUnicode_FromString(StringUtility::Utf16ToUtf8(args[ix]).c_str()));
            if (pValue.Get() == nullptr)
            {
                PyUtils::ThrowOnFailure(moduleName, funcName, "PyUnicode_FromString");
            }

            PyTuple_SetItem(pArgs.Get(), ix, pValue.Take());
        }

        auto pReturn = ScopedPyObject::CreateFromNew(PyObject_CallObject(pFunc.Get(), pArgs.Get()));

        if (PyErr_Occurred())
        {
            PyUtils::ThrowOnFailure(moduleName, funcName, "PyObject_CallObject");
        }
        else if (pReturn.Get() == nullptr)
        {
            Trace.WriteInfo(TraceComponent, "{0} returned NULL", funcNameW);
        }
        else if (PyObject_IsTrue(pReturn.Get()))
        {
            Trace.WriteInfo(TraceComponent, "{0} returned True", funcNameW);
        }
        else
        {
            Trace.WriteInfo(TraceComponent, "{0} returned False", funcNameW);
        }
    }

public:

    //
    // Python -> C++ support
    //

    DEFINE_PY_CALLBACK_IMPL( SetNodeIdOwnership )
    //{
        wchar_t * moduleName;
        wchar_t * nodeId;
        if (!PyArg_ParseTuple(args, "uu", &moduleName, &nodeId))
        {
            return NULL;
        }

        TRY_PARSE_PY_STRING( moduleName )
        TRY_PARSE_PY_STRING( nodeId )

        Trace.WriteInfo(TraceComponent, "SetNodeIdOwnership({0}, {1})", parsed_moduleName, parsed_nodeId);

        auto error = SetNodeIdOwnershipCallback_(parsed_moduleName, parsed_nodeId);

        if (error.IsSuccess())
        {
            Py_RETURN_TRUE;
        }
        else
        {
            auto msg = wformatString("{0}: {1}", error.ReadValue(), error.Message);
            PyErr_SetString(PyExc_RuntimeError, StringUtility::Utf16ToUtf8(msg).c_str());
            return NULL;
        }
    }

    DEFINE_PY_CALLBACK_IMPL( Broadcast )
    //{
        wchar_t * message;
        if (!PyArg_ParseTuple(args, "u", &message))
        {
            return NULL;
        }

        TRY_PARSE_PY_STRING( message )

        Trace.WriteInfo(TraceComponent, "Broadcast({0})", parsed_message);

        auto error = BroadcastCallback_(parsed_message);

        if (error.IsSuccess())
        {
            Py_RETURN_TRUE;
        }
        else
        {
            auto msg = wformatString("{0}: {1}", error.ReadValue(), error.Message);
            PyErr_SetString(PyExc_RuntimeError, StringUtility::Utf16ToUtf8(msg).c_str());
            return NULL;
        }
    }

private:

    PyThreadState * mainThreadState_;
    PyModuleCache moduleCache_;
};

//
// Embedded Python only supports pure C functions, which
// effectively limits us to a singleton pattern in practice
// for now (i.e. module definitions cannot be entirely
// encapsulated in classes). SingletonImpl and the 
// Global* variables/functions can
// be removed and replaced with the real Impl pattern
// if that ever changes.
//

class SingletonImpl
{
public:
    static SingletonImpl & GetInstance()
    {
        static SingletonImpl instance;
        return instance;
    }

    shared_ptr<Impl> const & GetImpl()
    {
        return impl_;
    }

private:

    SingletonImpl() : impl_(make_shared<Impl>())
    {
    }

    SingletonImpl(SingletonImpl const &) = delete;
    void operator=(SingletonImpl const &) = delete;

private:
    shared_ptr<Impl> impl_;
};

DEFINE_PY_CALLBACK( SetNodeIdOwnership )
DEFINE_PY_CALLBACK( Broadcast )

PyMethodDef GlobalMethods[] =
{
    DEFINE_PY_METHOD_ENTRY( SetNodeIdOwnership, "Set the node ID ownership for this module." )
    DEFINE_PY_METHOD_ENTRY( Broadcast, "Broadcast a message to all modules." )
    { NULL, NULL, 0, NULL }
};

PyModuleDef GlobalModule  = { PyModuleDef_HEAD_INIT, EmbeddedModuleName.c_str(), NULL, -1, GlobalMethods, NULL, NULL, NULL, NULL };

PyObject * InitializeGlobalModule()
{
    return PyModule_Create(&GlobalModule);
}

//
// PyInterpreter
//

ErrorCode PyInterpreter::Execute(
    wstring const & moduleName, 
    wstring const & funcName, 
    vector<wstring> const & args)
{
    return SingletonImpl::GetInstance().GetImpl()->Execute(moduleName, funcName, args);
}

ErrorCode PyInterpreter::ExecuteIfFound(
    wstring const & moduleName, 
    wstring const & funcName, 
    vector<wstring> const & args)
{
    return SingletonImpl::GetInstance().GetImpl()->ExecuteIfFound(moduleName, funcName, args);
}

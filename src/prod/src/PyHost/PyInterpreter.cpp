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
        , setNodeIdOwnershipCallback_()
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

	void Execute(
		string const & moduleName,
		string const & funcName,
		vector<string> const & args)
	{
        try
        {
            InternalExecute(moduleName, funcName, args);
        }
        catch (exception & ex)
        {
            Trace.WriteError(TraceComponent, "Execute failed: {0}", ex.what());
        }
    }

private:

	void InternalExecute(
		string const & moduleNameW,
		string const & funcNameW,
		vector<string> const & args)
	{
        Trace.WriteInfo(TraceComponent, "Execute({0}, {1}, {2})", moduleNameW, funcNameW, args);

        auto moduleName = Utf16ToUtf8NotNeeded(moduleNameW);
        auto funcName = Utf16ToUtf8NotNeeded(funcNameW);

		// TODO: Avoid blocking threads
		//
		ScopedGilState gilState;

		auto pName = ScopedPyObject::CreateFromNew(PyUnicode_DecodeFSDefault(moduleName.c_str()));
		if (pName.Get() == nullptr)
		{
			PyUtils::ThrowOnFailure(moduleName, funcName, "PyUnicode_DecodeFSDefault");
		}

		auto pFunc = moduleCache_.GetOrAddFunctionInModule(moduleName, funcName);
		if (!PyCallable_Check(pFunc.Get()))
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
			auto pValue = ScopedPyObject::CreateFromNew(PyUnicode_FromString(Utf16ToUtf8NotNeeded(args[ix]).c_str()));
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

	void Register_SetNodeIdOwnership(PyInterpreter::SetNodeIdOwnershipCallback const & callback)
	{
		setNodeIdOwnershipCallback_ = callback;
	}

	PyObject * SetNodeIdOwnership(PyObject * self, PyObject * args)
	{
		if (setNodeIdOwnershipCallback_ == nullptr)
		{
			PyErr_SetString(PyExc_RuntimeError, "Callback not registered");
			return NULL;
		}
        
		char * moduleName;
		char * nodeId;
		if (!PyArg_ParseTuple(args, "uu", &moduleName, &nodeId))
		{
			return NULL;
		}

        Trace.WriteInfo(TraceComponent, "Emb_SetNodeIdOwnership({0})", nodeId);

		setNodeIdOwnershipCallback_(moduleName, nodeId);

		Py_RETURN_TRUE;
	}

private:

	PyThreadState * mainThreadState_;
	PyModuleCache moduleCache_;

    PyInterpreter::SetNodeIdOwnershipCallback setNodeIdOwnershipCallback_;
};

//
// Embedded Python only supports pure C functions, which
// effectively limits us to a singleton pattern in practice
// for now. SingletonImpl and the Global* variables can
// be removed and replace with the real Impl pattern
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

PyObject * Global_SetNodeIdOwnership(PyObject * self, PyObject * args)
{
    return SingletonImpl::GetInstance().GetImpl()->SetNodeIdOwnership(self, args);
}

PyMethodDef GlobalMethods[] =
{
    { "SetNodeIdOwnership", Global_SetNodeIdOwnership, METH_VARARGS, "Set the node ID ownership for this module." },
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

void PyInterpreter::Execute(
    string const & moduleName, 
    string const & funcName, 
    vector<string> const & args)
{
	SingletonImpl::GetInstance().GetImpl()->Execute(moduleName, funcName, args);
}

void PyInterpreter::Register_SetNodeIdOwnership(SetNodeIdOwnershipCallback const & callback)
{
	SingletonImpl::GetInstance().GetImpl()->Register_SetNodeIdOwnership(callback);
}

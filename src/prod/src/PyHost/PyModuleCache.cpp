// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include <Python.h>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <string>

#include "ScopedPyObject.h"
#include "PyUtils.h"
#include "PyModuleCache.h"

using namespace PyHost;
using namespace std;

class PyModuleCache::PyModuleEntry
{
public:
    explicit PyModuleEntry(ScopedPyObject const & pModule)
        : module_(pModule)
        , functions_()
        , lock_()
    {
    }

    ScopedPyObject GetOrAddFunction(string const & moduleName, string const & funcName)
    {
        lock_guard<mutex> lock(lock_);

        auto findFunc = functions_.find(funcName);
        if (findFunc == functions_.end())
        {
            auto pFunc = ScopedPyObject::CreateFromNew(PyObject_GetAttrString(module_.Get(), funcName.c_str()));
            if (pFunc.Get() == nullptr)
            {
                PyUtils::ThrowOnFailure(moduleName, funcName, "PyObject_GetAttrString");
            }

            functions_.insert(make_pair(funcName, ScopedPyObject(pFunc)));

            return pFunc;
        }
        else
        {
            return findFunc->second;
        }
    }

private:
    ScopedPyObject module_;
    unordered_map<string, ScopedPyObject> functions_;
    mutex lock_;
};

//
// PyModuleCache
//

ScopedPyObject PyModuleCache::GetOrAddFunctionInModule(string const & moduleName, string const & funcName)
{
    lock_guard<mutex> lock(lock_);

    shared_ptr<PyModuleEntry> moduleEntry;

    auto findModule = modules_.find(moduleName);
    if (findModule == modules_.end())
    {
        auto pName = ScopedPyObject::CreateFromNew(PyUnicode_DecodeFSDefault(moduleName.c_str()));
        if (pName.Get() == nullptr)
        {
            PyUtils::ThrowOnFailure(moduleName, funcName, "PyUnicode_DecodeFSDefault");
        }

        auto pModule = ScopedPyObject::CreateFromNew(PyImport_Import(pName.Get()));
        if (pModule.Get() == nullptr)
        {
            PyUtils::ThrowOnFailure(moduleName, funcName, "PyImport_Import");
        }

        moduleEntry = make_shared<PyModuleEntry>(pModule);

        modules_.insert(make_pair(moduleName, moduleEntry));
    }
    else
    {
        moduleEntry = findModule->second;
    }
    
    return moduleEntry->GetOrAddFunction(moduleName, funcName);
}

void PyModuleCache::Remove(string const & moduleName)
{
    lock_guard<mutex> lock(lock_);

    modules_.erase(moduleName);
}

void PyModuleCache::Clear()
{
    lock_guard<mutex> lock(lock_);

    modules_.clear();
}

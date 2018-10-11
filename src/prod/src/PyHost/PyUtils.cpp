// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include <Python.h>
#include <stdexcept>

#include "Common/Common.h"
#include "ScopedPyObject.h"
#include "PyUtils.h"

using namespace Common;
using namespace PyHost;
using namespace std;

void PyUtils::ThrowOnFailure(
    string const & moduleName, 
    string const & funcName,
    string const & operation)
{
    string debugTag = moduleName;
    debugTag.append(":");
    debugTag.append(funcName);
    debugTag.append(":");
    debugTag.append(operation);

    CheckPyErrAndThrow(move(debugTag));
}

void PyUtils::CheckPyErrAndThrow(string && debugTag)
{
    string message = move(debugTag);

    if (PyErr_Occurred())
    {
        ScopedPyObject pExType, pExValue, pExStack;
        PyErr_Fetch(pExType.Ref(), pExValue.Ref(), pExStack.Ref());
        
        message.append(" failed:");
        message.append("\t\ntype:");
        message.append(StringUtility::Utf16ToUtf8(PyUnicode_AsWideCharString(PyObject_Str(pExType.Get()), nullptr)));
        message.append("\t\nvalue:");
        message.append(StringUtility::Utf16ToUtf8(PyUnicode_AsWideCharString(PyObject_Str(pExValue.Get()), nullptr)));
        message.append("\t\nstack:");
        message.append(StringUtility::Utf16ToUtf8(PyUnicode_AsWideCharString(PyObject_Str(pExStack.Get()), nullptr)));

        PyErr_Restore(pExType.Take(), pExValue.Take(), pExStack.Take());
        PyErr_Print();
    }
    else
    {
        message.append(" failed but no PyErr occurred.");
    }

    throw runtime_error(message);
}


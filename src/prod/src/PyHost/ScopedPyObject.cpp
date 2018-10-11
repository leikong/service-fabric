// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include <Python.h>

#include "ScopedPyObject.h"

using namespace PyHost;

ScopedPyObject::ScopedPyObject() : obj_(nullptr)
{
}

ScopedPyObject::~ScopedPyObject()
{
    Py_XDECREF(obj_);
}

ScopedPyObject::ScopedPyObject(PyObject * obj) : obj_(obj)
{
    // New PyObject has already had its reference count incremented
}

ScopedPyObject::ScopedPyObject(ScopedPyObject const & other) : obj_(other.Get())
{
    Py_XINCREF(obj_);
}

ScopedPyObject::ScopedPyObject(ScopedPyObject && other) : obj_(other.Take())
{
}

ScopedPyObject & ScopedPyObject::operator = (ScopedPyObject const & other)
{
    if (this != &other)
    {
        this->FromBorrowed(other.Get());
    }

    return *this;
}

ScopedPyObject & ScopedPyObject::operator = (ScopedPyObject && other)
{
    if (this != &other)
    {
        this->FromNew(other.Take());
    }

    return *this;
}

ScopedPyObject ScopedPyObject::CreateFromNew(PyObject * obj)
{
    return ScopedPyObject(obj);
}

ScopedPyObject ScopedPyObject::CreateFromBorrowed(PyObject * obj)
{
    Py_XINCREF(obj);

    return ScopedPyObject(obj);
}

void ScopedPyObject::FromNew(PyObject * obj)
{
    ScopedPyObject toRelease(obj_);

    obj_ = obj;
}

void ScopedPyObject::FromBorrowed(PyObject * obj)
{
    ScopedPyObject toRelease(obj_);

    obj_ = obj;

    Py_XINCREF(obj_);
}

PyObject ** ScopedPyObject::Ref()
{
    return &obj_;
}

PyObject * ScopedPyObject::Get() const
{
    return obj_;
}

PyObject * ScopedPyObject::Take()
{
    PyObject * result = obj_;
    obj_ = nullptr;
    return result;
}

// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace PyHost
{
    class ScopedPyObject
    {
    public:
        ScopedPyObject();
        ~ScopedPyObject();

    private:

        explicit ScopedPyObject(PyObject * obj);

    public:

        ScopedPyObject(ScopedPyObject const & other);
        ScopedPyObject(ScopedPyObject && other);

        ScopedPyObject & operator = (ScopedPyObject const & other);
        ScopedPyObject & operator = (ScopedPyObject && other);

        //
        // Do not create an assignment operator overload from PyObject *
        // since we do not know if the incoming PyObject * is a new
        // or borrowed reference
        //

        static ScopedPyObject CreateFromNew(PyObject * obj);
        static ScopedPyObject CreateFromBorrowed(PyObject * obj);

        void FromNew(PyObject * obj);
        void FromBorrowed(PyObject * obj);

        PyObject ** Ref();
        PyObject * Get() const;
        PyObject * Take();

    private:
        PyObject * obj_;
    };
}

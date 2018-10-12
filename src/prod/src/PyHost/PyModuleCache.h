// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace PyHost
{
    class PyModuleCache
    {
    public:

        ScopedPyObject GetOrAddFunctionInModule(std::string const & moduleName, std::string const & funcName, bool strict);
        void Remove(std::string const & moduleName);
        void Clear();

    private:
        class PyModuleEntry;

        std::unordered_map<std::string, std::shared_ptr<PyModuleEntry>> modules_;
        std::mutex lock_;
    };
}

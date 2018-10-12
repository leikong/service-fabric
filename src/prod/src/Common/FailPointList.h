// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class FailPointList
    {
    public:
        FailPointList()
        {
        }

        void Set(FailPointCriteriaSPtr const & criteria, FailPointActionSPtr const & action, int priority=0, std::string name=std::string());

        void Remove(std::string name);

        void Remove(FailPointCriteriaSPtr const & criteria,std::string name=std::string());

        void Remove(const FailPoint& failpoint)
        {
            Remove(failpoint.CriteriaSPtr);
        }

        bool Check(FailPointContext context, bool invokeAction,std::string name=std::string());

        bool Check(FailPointContext context,std::string name=std::string())
        {
            return Check(context,true,name);
        }

        bool CheckAllResults();
        void RemoveAll();
        
    private:
        Common::ExclusiveLock thisLock;
        std::vector<std::unique_ptr<FailPoint>> failpoints_;
    };
};

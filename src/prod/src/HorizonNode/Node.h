// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace HorizonNode
{
    class Node
    {
    public:

        Node(Federation::NodeConfig const &);
    
        // TODO: Synchronous for now since this is only called from Main.cpp
        //       though all new async code should start using coroutines
        //       rather than the begin/end pattern
        //
        Common::ErrorCode Open();

    private:
        class Impl;

        std::shared_ptr<Impl> impl_;
    };
};

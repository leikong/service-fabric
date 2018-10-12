// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include <stdio.h>
#include <string>

#include "Common/Common.h"
#include "Federation/Federation.h"

#include "Host.h"
#include "PyInterpreter.h"
#include "PyHostConfig.h"

using namespace Common;
using namespace Federation;
using namespace std;

using namespace PyHost;

StringLiteral const TraceComponent("Host");

//
// Impl
//

class Host::Impl
{
public:
    Impl() : pyInterpreter_()
    {
    }

    void OnRoutingTokenChanged(shared_ptr<FederationSubsystem> const & fs)
    {
        auto range = fs->Token.getRange();
        string rangeString;
        StringWriter rangeWriter(rangeString);
        range.WriteTo(rangeWriter, null_format);

        printf("OnRoutingTokenChanged: %s \n", Utf16ToUtf8NotNeeded(rangeString).c_str());
    }

    void Initialize()
    {
        auto moduleName = PyHostConfig::GetConfig().ModuleName;

        vector<string> args;
        args.push_back("MyTestArg");
        pyInterpreter_.Execute(moduleName, "OnInitialize", args);
    }

private:

    PyInterpreter pyInterpreter_;
};

//
// Host
//

Host::Host() : impl_(make_shared<Impl>())
{
}

void Host::Initialize()
{
    impl_->Initialize();
}

void Host::OnRoutingTokenChanged(shared_ptr<FederationSubsystem> const & fs)
{
    impl_->OnRoutingTokenChanged(fs);
}

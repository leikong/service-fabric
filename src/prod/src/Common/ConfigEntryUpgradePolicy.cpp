// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace std;
using namespace Common;

void ConfigEntryUpgradePolicy::WriteToTextWriter(TextWriter & w, Enum const & val)
{
    switch (val)
    {
    case Dynamic: 
        w << "Dynamic";
        return;
    case Static: 
        w << "Static";
        return;
    case NotAllowed: 
        w << "NotAllowed";        
        return;
    case SingleChange:
        w << "SingleChange";
        return;
    default: 
        Assert::CodingError("Unknown UpgradePolicy");
    }
}

string ConfigEntryUpgradePolicy::ToString(ConfigEntryUpgradePolicy::Enum const & val)
{
    string upgradePolicy;
    StringWriter(upgradePolicy).Write(val);
    return upgradePolicy;
}

ConfigEntryUpgradePolicy::Enum ConfigEntryUpgradePolicy::FromString(string const & val)
{
    if(StringUtility::AreEqualCaseInsensitive(val, "Dynamic"))
    {
        return ConfigEntryUpgradePolicy::Dynamic;
    }
    else if(StringUtility::AreEqualCaseInsensitive(val, "Static"))
    {
        return ConfigEntryUpgradePolicy::Static;
    }
    else if(StringUtility::AreEqualCaseInsensitive(val, "NotAllowed"))
    {
        return ConfigEntryUpgradePolicy::NotAllowed;
    }
    else if (StringUtility::AreEqualCaseInsensitive(val, "SingleChange"))
    {
        return ConfigEntryUpgradePolicy::SingleChange;
    }
    else
    {
        Assert::CodingError("Unknown UpgradePolicy: {0}", val);
    }    
}

// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace Transport;
using namespace Common;
using namespace std;

ClaimsMessage::ClaimsMessage()
{
}

ClaimsMessage::ClaimsMessage(string const & claims) : claims_(claims)
{
}

void ClaimsMessage::WriteTo(TextWriter & w, FormatOptions const &) const
{ 
    w.Write("{0}", claims_);
}

string const & ClaimsMessage::Claims() const
{
    return claims_;
}

MessageUPtr ClaimsMessage::CreateClaimsMessage(string const& claims)
{
    return make_unique<Message>(ClaimsMessage(claims));
}

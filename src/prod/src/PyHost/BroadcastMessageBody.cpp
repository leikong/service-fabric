// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "Common/Common.h"
#include "Transport/Transport.h"

#include "BroadcastMessageBody.h"

using namespace Common;
using namespace std;
using namespace Transport;
using namespace PyHost;

BroadcastMessageBody::BroadcastMessageBody()
    : contents_()
{
}

BroadcastMessageBody::BroadcastMessageBody(wstring const & contents)
    : contents_(contents)
{
}

MessageUPtr BroadcastMessageBody::CreateMessage(wstring const & contents)
{
    BroadcastMessageBody body(contents);

    auto message = make_unique<Message>(body);
    message->Headers.Add(ActorHeader(Actor::PyHost));
    message->Headers.Add(ActionHeader(L"Broadcast"));

    return message;
}

BroadcastMessageBody BroadcastMessageBody::DeserializeBody(MessageUPtr & msg)
{
    BroadcastMessageBody body;
    msg->GetBody(body);
    return move(body);
}

wstring const & BroadcastMessageBody::GetContents() const
{
    return contents_;
}

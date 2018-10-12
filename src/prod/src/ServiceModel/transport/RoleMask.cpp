// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace Common;
using namespace std;

namespace Transport
{
    namespace RoleMask
    {
        void WriteToTextWriter(Common::TextWriter & w, Enum const & e)
        {
            switch (e)
            {
                case None: w << "None"; return;
                case User: w << "User"; return;
                case Admin: w << "Admin"; return;
                case All: w << "All"; return;
                default:
                    w.Write("{0:x}", (unsigned int)e);
            }
        }

        ErrorCode TryParse(std::string const & stringInput, _Out_ Enum & roleMask)
        {
            roleMask = None;

            if (StringUtility::AreEqualCaseInsensitive(stringInput, "None"))
            {
                return ErrorCode();
            }

            if (StringUtility::AreEqualCaseInsensitive(stringInput, "User"))
            {
                roleMask = User;
                return ErrorCode();
            }

            if (StringUtility::AreEqualCaseInsensitive(stringInput, "Admin"))
            {
                roleMask = Admin;
                return ErrorCode();
            }

            if (StringUtility::AreEqualCaseInsensitive(stringInput, "All"))
            {
                roleMask = All;
                return ErrorCode();
            }

            return ErrorCodeValue::InvalidArgument;
        }

        ErrorCode TryParse(FABRIC_CLIENT_ROLE clientRole, _Out_ Enum & roleMask)
        {
            roleMask = None;

            if (clientRole == FABRIC_CLIENT_ROLE_USER)
            {
                roleMask = User;
                return ErrorCode();
            }

            if (clientRole == FABRIC_CLIENT_ROLE_ADMIN)
            {
                roleMask = Admin;
                return ErrorCode();
            }

            return ErrorCodeValue::InvalidArgument;
        }

        std::string EnumToString(Transport::RoleMask::Enum e)
        {
            switch (e)
            {
            case None:
                return "None";
            case User:
                return "User";
            case Admin:
                return "Admin";
            case All:
                return "All";
            default:
                return formatString.L("{0:x}", (unsigned int)e);
            }
        }
    }
}

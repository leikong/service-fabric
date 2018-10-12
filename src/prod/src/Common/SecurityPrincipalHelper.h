// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class SecurityPrincipalHelper : protected Common::TextTraceComponent<Common::TraceTaskCodes::Hosting>
    {
    public:

        static Common::ErrorCode AddMemberToLocalGroup(std::string const & parentGroupAccountName, std::string const & memberToAddAccountName);
        static Common::ErrorCode RemoveMemberFromLocalGroup(std::string const & parentGroupAccountName, std::string const & memberToAddAccountName);
        static Common::ErrorCode SetLocalGroupMembers(std::string const & parentGroupAccountName, std::vector<PSID> const & membersToSet);

        static Common::ErrorCode CreateUserAccount(std::string const & accountName, std::string const & password, std::string const & comment);
        static Common::ErrorCode CreateGroupAccount(std::string const & groupName, std::string const & comment);

        static Common::ErrorCode DeleteUserProfile(std::string const & accountName, Common::SidSPtr const& sid = Common::SidSPtr());
        static Common::ErrorCode DeleteUserAccount(std::string const & accountName);
        static Common::ErrorCode DeleteUserAccountIgnoreDeleteProfileError(std::string const & accountName, Common::SidSPtr const& sid = Common::SidSPtr());

        static Common::ErrorCode DeleteGroupAccount(std::string const & groupName);

        static Common::ErrorCode DeleteUsersWithCommentPrefix(std::string const & commentPrefix);
        static Common::ErrorCode DeleteGroupWithCommentPrefix(std::string const & commentPrefix);

        static Common::ErrorCode GetMembership(std::string const & accountName, __out std::vector<std::string> & memberOf);
        static Common::ErrorCode GetMembers(std::string const & accountName, __out std::vector<std::string> & members);

        static Common::ErrorCode GetGroupComment(std::string const & accountName, __out std::string & comment);
        static Common::ErrorCode GetUserComment(std::string const & accountName, __out std::string & comment);

        static Common::ErrorCode UpdateGroupComment(std::string const & accountName, std::string const& comment);
        static Common::ErrorCode UpdateUserComment(std::string const & accountName, std::string const& comment);

        static std::string GetMembershipString(std::vector<std::string> const& membership);

        static Common::ErrorCode AcquireMutex(std::string const& mutexName, __out Common::MutexHandleUPtr & mutex);
    };
}

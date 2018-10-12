// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace std;
using namespace Common;

StringLiteral const TraceSecurityPrincipalHelper("SecurityPrincipalHelper");

#if !defined(PLATFORM_UNIX)
ErrorCode SecurityPrincipalHelper::AddMemberToLocalGroup(string const & parentGroup, string const & memberToAdd)
{
    LOCALGROUP_MEMBERS_INFO_3 groupMembership;
    ::ZeroMemory(&groupMembership, sizeof(groupMembership));
    groupMembership.lgrmi3_domainandname = const_cast<LPSTR>(memberToAdd.c_str());

    NET_API_STATUS nStatus = ::NetLocalGroupAddMembers(
        NULL, 
        parentGroup.c_str(), 
        3 /*LOCALGROUP_MEMBERS_INFO level*/, 
        reinterpret_cast<LPBYTE>(&groupMembership), 
        1 /*user count*/);
    if (nStatus != NERR_Success)
    {
        ErrorCode error;
        if (nStatus == NERR_GroupNotFound)
        {
            error = ErrorCode(ErrorCodeValue::NotFound);
        }
        else if (nStatus == ERROR_MEMBER_IN_ALIAS)
        {
            error = ErrorCode(ErrorCodeValue::AlreadyExists);
        }
        else
        {
            error = ErrorCode::FromWin32Error(nStatus);
        }
        WriteTrace(
            error.IsError(ErrorCodeValue::AlreadyExists) ? LogLevel::Noise : LogLevel::Warning,
            TraceSecurityPrincipalHelper,
            "Error adding user account {0} to group {1}: status={2}, error={3}",
            memberToAdd,
            parentGroup,
            nStatus,
            error);

        return error;
    }

    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode SecurityPrincipalHelper::RemoveMemberFromLocalGroup(std::string const & parentGroupAccountName, std::string const & memberToRemoveAccountName)
{
    LOCALGROUP_MEMBERS_INFO_3 groupMembership;
    ::ZeroMemory(&groupMembership, sizeof(groupMembership));
    groupMembership.lgrmi3_domainandname = const_cast<LPSTR>(memberToRemoveAccountName.c_str());

    NET_API_STATUS nStatus = ::NetLocalGroupDelMembers(
        NULL, 
        parentGroupAccountName.c_str(), 
        3 /*LOCALGROUP_MEMBERS_INFO level*/, 
        reinterpret_cast<LPBYTE>(&groupMembership), 
        1 /*user count*/);
    if (nStatus != NERR_Success)
    {
        ErrorCode error;
        if (nStatus == NERR_GroupNotFound)
        {
            error = ErrorCode(ErrorCodeValue::NotFound);
        }
        else
        {
            error = ErrorCode::FromWin32Error(nStatus);
        }

        WriteWarning(
            TraceSecurityPrincipalHelper,
            "Error removing user account {0} from group {1}: status={2}, error={3}",
            memberToRemoveAccountName,
            parentGroupAccountName,
            nStatus,
            error);

        return error;
    }

    return ErrorCode(ErrorCodeValue::Success);
}
#endif

ErrorCode SecurityPrincipalHelper::SetLocalGroupMembers(string const & groupName, vector<PSID> const & membersToSet)
{
    vector<LOCALGROUP_MEMBERS_INFO_0> members;
    DWORD totalEntries = 0;
    for (auto iter = membersToSet.begin(); iter != membersToSet.end(); iter++)
    {
        LOCALGROUP_MEMBERS_INFO_0 memberInfo;
        ::ZeroMemory(&memberInfo, sizeof(memberInfo));
        memberInfo.lgrmi0_sid = *iter;
        members.push_back(memberInfo);
        totalEntries++;
    }

    DWORD win32Error = ::NetLocalGroupSetMembers(
        NULL,
        groupName.c_str(),
        0,
        reinterpret_cast<LPBYTE>(members.data()),
        totalEntries);

    return ErrorCode::TraceReturn(
        ErrorCode::FromWin32Error(win32Error),
        TraceTaskCodes::Common,
        TraceSecurityPrincipalHelper,
        "SetLocalGroupMembers");

}

#if !defined(PLATFORM_UNIX)
ErrorCode SecurityPrincipalHelper::CreateUserAccount(string const & accountName, string const & password, string const & comment)
{
    USER_INFO_4 userInfo;
    ::ZeroMemory(&userInfo, sizeof(userInfo));
    userInfo.usri4_name = const_cast<LPSTR>(accountName.c_str());
    userInfo.usri4_password = const_cast<LPSTR>(password.c_str());
    userInfo.usri4_comment = const_cast<LPSTR>(comment.c_str());
    userInfo.usri4_flags = 
        UF_SCRIPT | UF_PASSWD_CANT_CHANGE | UF_DONT_EXPIRE_PASSWD | UF_NOT_DELEGATED | UF_NORMAL_ACCOUNT;
    userInfo.usri4_acct_expires = TIMEQ_FOREVER;
    userInfo.usri4_primary_group_id = DOMAIN_GROUP_RID_USERS;
    ErrorCode error(ErrorCodeValue::Success);
    NET_API_STATUS nStatus = ::NetUserAdd(NULL, 4 /* USER_INFO level*/, reinterpret_cast<LPBYTE>(&userInfo), NULL);
    if (nStatus != NERR_Success)
    {
        if(nStatus == NERR_UserExists || nStatus == NERR_GroupExists || nStatus == ERROR_ALIAS_EXISTS)
        {
            error = ErrorCode(ErrorCodeValue::AlreadyExists);
        }
        else
        {
            error = ErrorCode::FromWin32Error(nStatus);
        }

        WriteWarning(
            TraceSecurityPrincipalHelper,
            "Error creating user account {0}: status={1}, error={2}",
            accountName,
            nStatus,
            error);
        return error;
    }

    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode SecurityPrincipalHelper::DeleteUserProfile(string const & accountName, SidSPtr const& sid)
{
    string sidString;
    ErrorCode error;
    if(!sid)
    {
        SidSPtr tempSid;
        error = BufferedSid::CreateSPtr(accountName, tempSid);
        if (!error.IsSuccess())
        {
            WriteWarning(
                TraceSecurityPrincipalHelper,
                "GetSid for account {0} failed with {1}",
                accountName,
                error);
            return error;
        }

        error = tempSid->ToString(sidString);
    }
    else
    {
        error = sid->ToString(sidString);
    }

    if(!error.IsSuccess())
    {
        WriteWarning(
            TraceSecurityPrincipalHelper,
            "sidUPtr->ToString for account {0} failed with {1}",
            accountName,
            error);

        return error;
    }

    if (!::DeleteProfile(sidString.c_str(), NULL, NULL))
    {
        DWORD nStatus = ::GetLastError();
        if (nStatus == ERROR_FILE_NOT_FOUND)
        {
            // If the profile doesn't exist, consider DeleteProfile successful
            WriteNoise(
                TraceSecurityPrincipalHelper,
                "User profile doesn't exist: Account={0}, sid={1}",
                accountName,
                sidString);
            error = ErrorCode::Success();
        }
        else
        {
            error = ErrorCode::FromWin32Error(nStatus);
            // ERROR_INVALID_PARAMETER happens if the profile is still used by other handles
            // (eg. host processes not terminated)
            WriteWarning(
                TraceSecurityPrincipalHelper,
                "Error deleting user profile: Account={0}, sid={1}, status={2}, error={3}",
                accountName,
                sidString,
                nStatus,
                error);
            return error;
        }
    }

    return error;
}

ErrorCode SecurityPrincipalHelper::DeleteUserAccount(string const & accountName)
{
    NET_API_STATUS nStatus = ::NetUserDel(".", accountName.c_str());
    if (nStatus != NERR_Success)
    {
        if(nStatus == NERR_UserNotFound)
        {
            WriteInfo(
                TraceSecurityPrincipalHelper,
                "Deleting user account {0} failed with NERR_UserNotFound",
                accountName);
            return ErrorCode(ErrorCodeValue::NotFound);
        }
        else
        {
            auto error = ErrorCode::FromWin32Error(nStatus);
            WriteWarning(
                TraceSecurityPrincipalHelper,
                "Error deleting user account {0}: status={1}, error={2}",
                accountName,
                nStatus,
                error);
            return error;
        }
    }

    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode SecurityPrincipalHelper::DeleteUserAccountIgnoreDeleteProfileError(string const & accountName, Common::SidSPtr const& sid)
{
    ErrorCode error = DeleteUserProfile(accountName, sid);
    error.ReadValue();// Ignore delete profile error.

    NET_API_STATUS nStatus = ::NetUserDel(".", accountName.c_str());
    if (nStatus != NERR_Success)
    {
        if(nStatus == NERR_UserNotFound)
        {
            WriteInfo(
                TraceSecurityPrincipalHelper,
                "Deleting user account {0} failed with NERR_UserNotFound",
                accountName);
            return ErrorCode(ErrorCodeValue::NotFound);
        }
        else
        {
            error = ErrorCode::FromWin32Error(nStatus);
            WriteWarning(
                TraceSecurityPrincipalHelper,
                "Error deleting user account {0}: status={1}, error={2}",
                accountName,
                nStatus,
                error);
            return error;
        }
    }

    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode SecurityPrincipalHelper::CreateGroupAccount(string const & groupName, string const & comment)
{
    LOCALGROUP_INFO_1 groupInfo;
    groupInfo.lgrpi1_name = const_cast<LPSTR>(groupName.c_str());
    groupInfo.lgrpi1_comment = const_cast<LPSTR>(comment.c_str());

    NET_API_STATUS nStatus = ::NetLocalGroupAdd(
        NULL,
        1 /* Information level */, 
        reinterpret_cast<LPBYTE>(&groupInfo),
        NULL);
    if (nStatus != NERR_Success)
    {
        if (nStatus == ERROR_ALIAS_EXISTS || nStatus == NERR_GroupExists || nStatus == NERR_UserExists)
        {
            return ErrorCode(ErrorCodeValue::AlreadyExists);
        }

        auto error = ErrorCode::FromWin32Error(nStatus);
        WriteWarning(
            TraceSecurityPrincipalHelper,
            "Error creating group account {0}: status={1}, error={2}",
            groupName,
            nStatus,
            error);

        return error;
    }

    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode SecurityPrincipalHelper::GetMembers(string const & accountName, __out vector<string> & members)
{
    PLOCALGROUP_MEMBERS_INFO_1 localGroupMembersInfo = NULL;
    DWORD entryReadCount = 0;
    DWORD totalEntryCount = 0;
    NET_API_STATUS nStatus = NetLocalGroupGetMembers(
        NULL /*serverName*/,
        accountName.c_str() /*accountName*/,
        1 /*level*/,
        (LPBYTE *) &localGroupMembersInfo,
        MAX_PREFERRED_LENGTH,
        &entryReadCount,
        &totalEntryCount,
        NULL);
    
    if (nStatus != NERR_Success)
    {
        if (nStatus == ERROR_NO_SUCH_ALIAS || nStatus == NERR_GroupNotFound)
        {
            return ErrorCode(ErrorCodeValue::NotFound);
        }

        auto error = ErrorCode::FromWin32Error(nStatus);
        WriteWarning(
            TraceSecurityPrincipalHelper,
            "Error getting group members for account {0}: status={1}, error={2}",
            accountName,
            nStatus,
            error);
        return error;
    }
    
    if (localGroupMembersInfo != NULL)
    {
        PLOCALGROUP_MEMBERS_INFO_1 currentMembersInfo = localGroupMembersInfo;
        for (DWORD i = 0; i < entryReadCount; ++i)
        {
            ASSERT_IF(currentMembersInfo == NULL, "Buffer shouldn't be null");
            members.push_back(string(currentMembersInfo->lgrmi1_name));
            ++currentMembersInfo;
        }

        NetApiBufferFree(localGroupMembersInfo);
    }

    return ErrorCode(ErrorCodeValue::Success);
}


ErrorCode SecurityPrincipalHelper::GetMembership(string const & accountName, __out vector<string> & memberOf)
{
    LPLOCALGROUP_USERS_INFO_0 localGroupInfo = NULL;
    DWORD entryReadCount = 0;
    DWORD totalEntryCount = 0;
    NET_API_STATUS nStatus = NetUserGetLocalGroups(
        NULL /*serverName*/,
        accountName.c_str() /*userName*/,
        0 /*LOCAL_GROUP_USERS_INFO level*/,
        LG_INCLUDE_INDIRECT,
        (LPBYTE *) &localGroupInfo,
        MAX_PREFERRED_LENGTH,
        &entryReadCount,
        &totalEntryCount);
    if (nStatus != NERR_Success)
    {
        auto error = ErrorCode::FromWin32Error(nStatus);
        WriteWarning(
            TraceSecurityPrincipalHelper,
            "Error getting user account information for {0}: status={1}, error={2}",
            accountName,
            nStatus,
            error);

        return error;
    }
    
    if (localGroupInfo != NULL)
    {
        LPLOCALGROUP_USERS_INFO_0 currentGroupInfo = localGroupInfo;
        for (DWORD i = 0; i < entryReadCount; ++i)
        {
            ASSERT_IF(currentGroupInfo == NULL, "Buffer shouldn't be null");
            memberOf.push_back(string(currentGroupInfo->lgrui0_name));
            ++currentGroupInfo;
        }

        NetApiBufferFree(localGroupInfo);
    }

    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode SecurityPrincipalHelper::DeleteGroupAccount(string const & groupName)
{
    NET_API_STATUS nStatus = ::NetLocalGroupDel(".", groupName.c_str());
    if (nStatus != NERR_Success)
    {
        if(nStatus == NERR_GroupNotFound)
        {
            WriteInfo(
                TraceSecurityPrincipalHelper,
                "Deleting group account {0} failed with NERR_GroupNotFound",
                groupName);
            return ErrorCode(ErrorCodeValue::NotFound);
        }
        else
        {
            auto error = ErrorCode::FromWin32Error(nStatus); 
            WriteWarning(
                TraceSecurityPrincipalHelper,
                "Error deleting group account {0}: status={1}, error={2}",
                groupName,
                nStatus,
                error);
            return error;
        }
    }

    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode SecurityPrincipalHelper::GetGroupComment(std::string const & accountName, __out string & comment)
{
    // Get group info
    PLOCALGROUP_INFO_1 groupInfo = NULL;
    NET_API_STATUS nStatus = ::NetLocalGroupGetInfo(
        NULL /*serverName*/, 
        accountName.c_str(), 
        1 /*LOCALGROUP_INFO level*/, 
        (LPBYTE*) &groupInfo); 

    if (nStatus != NERR_Success)
    {
        if (nStatus == ERROR_NO_SUCH_ALIAS || nStatus == NERR_GroupNotFound)
        {
            return ErrorCode(ErrorCodeValue::NotFound);
        }

        auto error = ErrorCode::FromWin32Error(nStatus);
        WriteWarning(
            TraceSecurityPrincipalHelper,
            "Error getting group information for account {0}: status={1}, error={2}",
            accountName,
            nStatus,
            error);
        
        return error;
    }

    ASSERT_IF(groupInfo == NULL, "groupInfo should not be null");
    comment = string(groupInfo->lgrpi1_comment);
    WriteNoise(
        TraceSecurityPrincipalHelper,
        "Group {0}: retrieved comment \"{1}\"",
        accountName,
        comment);

    NetApiBufferFree(groupInfo);
    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode SecurityPrincipalHelper::GetUserComment(std::string const & accountName, __out string & comment)
{
    // Get group info
    LPUSER_INFO_1 userInfo = NULL;
    NET_API_STATUS nStatus = ::NetUserGetInfo(
        NULL /*serverName*/, 
        accountName.c_str(), 
        1 /*LOCALUSER_INFO level*/, 
        (LPBYTE*) &userInfo); 

    if (nStatus != NERR_Success)
    {
        if (nStatus == ERROR_NO_SUCH_ALIAS || nStatus == NERR_UserNotFound)
        {
            return ErrorCode(ErrorCodeValue::NotFound);
        }

        auto error = ErrorCode::FromWin32Error(nStatus);
        WriteWarning(
            TraceSecurityPrincipalHelper,
            "Error getting user information for account {0}: status={1}, error={2}",
            accountName,
            nStatus,
            error);
        
        return error;
    }

    ASSERT_IF(userInfo == NULL, "user info should not be null");
    comment = string(userInfo->usri1_comment);
    WriteNoise(
        TraceSecurityPrincipalHelper,
        "User {0}: retrieved comment \"{1}\"",
        accountName,
        comment);

    NetApiBufferFree(userInfo);
    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode SecurityPrincipalHelper::UpdateUserComment(string const & accountName, string const & comment)
{
    DWORD errorIndex;
    USER_INFO_1007  commentUserInfo;
    ::ZeroMemory(&commentUserInfo, sizeof(commentUserInfo));
    commentUserInfo.usri1007_comment = const_cast<LPSTR>(comment.c_str());
    NET_API_STATUS nStatus = NetUserSetInfo(
        NULL /*serverName*/,
        accountName.c_str(),
        1007 /*LOCAL_GROUP_INFO level*/,
        reinterpret_cast<LPBYTE>(&commentUserInfo),
        &errorIndex);

    if (nStatus != NERR_Success)
    {
        if (nStatus == ERROR_NO_SUCH_ALIAS || nStatus == NERR_UserNotFound)
        {
            return ErrorCode(ErrorCodeValue::NotFound);
        }

        auto error = ErrorCode::FromWin32Error(nStatus);
        WriteWarning(
            TraceSecurityPrincipalHelper,
            "Group {0}: Error updating user comment: newComment={1}, status={2}, error={3}",
            accountName,
            comment,
            nStatus,
            error);
        return error;
    }
    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode SecurityPrincipalHelper::UpdateGroupComment(string const & accountName, string const & comment)
{
    DWORD errorIndex;
    LOCALGROUP_INFO_1002 commentGroupInfo;
    ::ZeroMemory(&commentGroupInfo, sizeof(commentGroupInfo));
    commentGroupInfo.lgrpi1002_comment = const_cast<LPSTR>(comment.c_str());
    NET_API_STATUS nStatus = NetLocalGroupSetInfo(
        NULL /*serverName*/,
        accountName.c_str(),
        1002 /*LOCAL_GROUP_INFO level*/,
        reinterpret_cast<LPBYTE>(&commentGroupInfo),
        &errorIndex);

    if (nStatus != NERR_Success)
    {
        if (nStatus == ERROR_NO_SUCH_ALIAS || nStatus == NERR_GroupNotFound)
        {
            return ErrorCode(ErrorCodeValue::NotFound);
        }

        auto error = ErrorCode::FromWin32Error(nStatus);
        WriteWarning(
            TraceSecurityPrincipalHelper,
            "Group {0}: Error updating group comment: newComment={1}, status={2}, error={3}",
            accountName,
            comment,
            nStatus,
            error);
        return error;
    }
    return ErrorCode(ErrorCodeValue::Success);
}

ErrorCode SecurityPrincipalHelper::AcquireMutex(string const& mutexName, __out MutexHandleUPtr & mutex)
{
    mutex = MutexHandle::CreateUPtr(mutexName);
    WriteNoise(TraceSecurityPrincipalHelper, "{0}: Wait for mutex", mutexName);
    auto error = mutex->WaitOne();
    if (!error.IsSuccess())
    {
        WriteTrace(
            error.ToLogLevel(),
            TraceSecurityPrincipalHelper,
            "{0}: Error waiting for mutex: error={1}",
            mutexName,
            error);        
    }

    return error;
}

string SecurityPrincipalHelper::GetMembershipString(vector<string> const& membership)
{
    string membershipString("Membership = ");
    StringWriter writer(membershipString);
    for (auto it = membership.begin(); it != membership.end(); ++it)
    {
        if(it == membership.begin())
        {
            writer.Write("{0}", *it);
        }
        else
        {
            writer.Write(",{0}", *it);
        }
    }

    return membershipString;
}

ErrorCode SecurityPrincipalHelper::DeleteUsersWithCommentPrefix(string const & prefix)
{
    ErrorCode lastError(ErrorCodeValue::Success);
    LPUSER_INFO_1 pBuff = NULL;
    DWORD entriesRead = 0;
    DWORD totalEntries = 0;
    LPDWORD resumeHandle = 0;
    NET_API_STATUS status = ::NetUserEnum(
        NULL /*local server*/,
        1 /*LOCALGROUP_INFO level*/,
        FILTER_NORMAL_ACCOUNT,
        (LPBYTE *)&pBuff,
        MAX_PREFERRED_LENGTH,
        &entriesRead,
        &totalEntries,
        resumeHandle /*resume handle*/);

    lastError = ErrorCode::FromWin32Error(status);

    if (status == NERR_Success)
    {
        LPUSER_INFO_1 p;
        if ((p = pBuff) != NULL)
        {
            for (DWORD i = 0; i < entriesRead; i++)
            {
                string comment(p->usri1_comment);
                string name(p->usri1_name);
                if (comment.substr(0, prefix.size()).compare(prefix) == 0)
                {
                    NET_API_STATUS err = ::NetUserDel(".", name.c_str());
                    if (err != NERR_Success)
                    {
                        lastError = ErrorCode::FromWin32Error(err);
                        WriteWarning(
                            TraceSecurityPrincipalHelper,
                            "Failed to remove user {0} with comment prefix {1}: error={2}",
                            name,
                            prefix,
                            lastError);
                    }
                }
                ++p;
            }
        }
    }
    if (pBuff != NULL)
    {
        NetApiBufferFree(pBuff);
    }
    return lastError;
}

ErrorCode SecurityPrincipalHelper::DeleteGroupWithCommentPrefix(string const & prefix)
{
    ErrorCode lastError(ErrorCodeValue::Success);
    LPLOCALGROUP_INFO_1 pBuff = NULL;
    DWORD entriesRead = 0;
    DWORD totalEntries = 0;
    DWORD_PTR resumeHandle = 0;
    NET_API_STATUS status = ::NetLocalGroupEnum(
        NULL /*local server*/,
        1 /*LOCALGROUP_INFO level*/,
        (LPBYTE *)&pBuff,
        MAX_PREFERRED_LENGTH,
        &entriesRead,
        &totalEntries,
        &resumeHandle /*resume handle*/);

    lastError = ErrorCode::FromWin32Error(status);

    if (status == NERR_Success)
    {
        LPLOCALGROUP_INFO_1 p;
        if ((p = pBuff) != NULL)
        {
            for (DWORD i = 0; i < entriesRead; i++)
            {
                string comment(p->lgrpi1_comment);
                string name(p->lgrpi1_name);
                if (comment.substr(0, prefix.size()).compare(prefix) == 0)
                {
                    NET_API_STATUS err = ::NetLocalGroupDel(".", name.c_str());
                    if (err != NERR_Success)
                    {
                        lastError = ErrorCode::FromWin32Error(err);
                        WriteWarning(
                             TraceSecurityPrincipalHelper,
                             "Failed to remove group {0} with comment prefix {1}: error={2}",
                            name,
                            prefix,
                            lastError);
                    }
                }
                ++p;
            }
        }
    }
    if (pBuff != NULL)
    {
        NetApiBufferFree(pBuff);
    }
    return lastError;
}
#endif

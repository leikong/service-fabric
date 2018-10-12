// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class RegistryKey
    {
    public:
        explicit RegistryKey(std::string const & name, bool const readOnly = true, bool const openExisting = false);

        RegistryKey(std::string const & name, LPCSTR machineName, bool const readOnly = true, bool const openExisting = false);

        bool GetValue(std::string const & name, DWORD & value);

        bool GetValue(std::string const & name, std::string & value);

        bool GetValue(std::string const & name, std::string & value, bool expandEnvironmentStrings);

        bool GetValue(std::string const & name, std::vector<std::string> & value);

        bool SetValue(std::string const & name, DWORD value);

        bool SetValue(std::string const & name, std::string const & value, bool typeRegSZ = false);

        bool SetValue(std::string const & name, std::vector<std::string> const & value);

        bool SetValue(std::string const & name, const BYTE * value, ULONG valueLen);

        bool DeleteValue(std::string const & name);

        bool DeleteKey();

        ~RegistryKey();

        __declspec( property(get=GetExisted)) bool Existed;
        bool GetExisted() { return existed_; }

        __declspec( property(get=GetIsValid)) bool IsValid;
        bool GetIsValid() { return error_ == ERROR_SUCCESS; }

        __declspec( property(get=GetError)) DWORD Error;
        DWORD GetError() { return error_; }

    private:
        DENY_COPY(RegistryKey);
        
        std::string keyName_;
        HKEY key_;
        bool existed_;
        bool initialized_;
        DWORD error_;
    };
}

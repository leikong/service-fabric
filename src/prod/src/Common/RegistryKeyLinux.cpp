// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"
#include "windows.h"
#include <string.h>

#define MAX_VALUE_NAME 16383

using namespace std;
using namespace Common;

/*  Helper functions  */

ErrorCode SetEtcConfigValue(const string & name, const string & value)
{
	string pathToConfigSetting = Path::Combine(FabricConstants::FabricRegistryKeyPath, name);
	try
	{
		FileWriter fileWriter;
		auto error = fileWriter.TryOpen(pathToConfigSetting);
		if (!error.IsSuccess())
		{
			return ErrorCodeValue::OperationFailed;
		}

		fileWriter << value;
	}
	catch (std::exception const& e)
	{
		return ErrorCodeValue::OperationFailed;
	}

	return ErrorCodeValue::Success;
}

ErrorCode GetEtcConfigValue(const string & name, __out string & value)
{
	string pathToConfigSetting = Path::Combine(FabricConstants::FabricRegistryKeyPath, name);
	if (!File::Exists(pathToConfigSetting))
	{
		return ErrorCodeValue::FileNotFound;
	}

	try
	{
		File fileReader;
		auto error = fileReader.TryOpen(pathToConfigSetting, Common::FileMode::Open, Common::FileAccess::Read, Common::FileShare::Read);
		if (!error.IsSuccess())
		{
			return ErrorCodeValue::OperationFailed;
		}

		int64 fileSize = fileReader.size();
		size_t size = static_cast<size_t>(fileSize);

		string text;
		text.resize(size);
		fileReader.Read(&text[0], static_cast<int>(size));
		fileReader.Close();

		StringWriter(value).Write(text);
	}
	catch (std::exception const& e)
	{
		return ErrorCodeValue::OperationFailed;
	}

	return Environment::Expand(value, value);
}

ErrorCode DeleteEtcConfigValue(const string & name)
{
	string pathToConfigSetting = Path::Combine(FabricConstants::FabricRegistryKeyPath, name);
	if (!File::Exists(pathToConfigSetting))
	{
		return ErrorCodeValue::FileNotFound;
	}
	try
	{
		FileWriter fileWriter;
		auto error = fileWriter.TryOpen(pathToConfigSetting);
		if (!error.IsSuccess())
		{
			return ErrorCodeValue::OperationFailed;
		}

		std::string result = NULL; //replace with null
		fileWriter << result;
	}
	catch (std::exception const& e)
	{
		return ErrorCodeValue::OperationFailed;
	}
	return ErrorCodeValue::Success;
}

/*     Public functions exposed via RegistryKey.h		*/


RegistryKey::RegistryKey(string const & name, bool const readOnly, bool const openExisting)
{
	ErrorCode err = ErrorCodeValue::Success;
	ASSERT_IF(name.compare(FabricConstants::FabricRegistryKeyPath) != 0, "Registry path name did not match expected path.");

	error_ = ERROR_SUCCESS;
	initialized_ = error_ == ERROR_SUCCESS;
    existed_ = openExisting == true;
}

RegistryKey::RegistryKey(string const & name, LPCSTR machineName, bool const readOnly, bool const openExisting)
{
	Assert::CodingError("Getting registry configuration from remote not supported in linux.");
}

bool RegistryKey::SetValue(string const & name, DWORD value)
{
	string valuestr = std::to_string(static_cast<LONGLONG>(value));
	ErrorCode err = SetEtcConfigValue(name, valuestr);
	error_ = static_cast<DWORD>(err.ReadValue());
	return IsValid;
}

bool RegistryKey::SetValue(string const & name, string const & value, bool typeRegSZ)
{
  // TODO Option for REG_EXPAND_SZ is to be added. Bug 9012047
  UNREFERENCED_PARAMETER(typeRegSZ);
  
	ErrorCode err = SetEtcConfigValue(name, value);
	error_ = static_cast<DWORD>(err.ReadValue());
    return  IsValid;
}

bool RegistryKey::SetValue(string const & name, vector<string> const & value)
{
    size_t size = 0;
    for (auto const & item : value)
    {
        size += item.size() + 1;
    }
    size++;

    vector<char> registryValue(size);
    size_t index = 0;
    for (auto const & item : value)
    {
        KMemCpySafe(&registryValue[index], (registryValue.size() - index) * sizeof(char), item.c_str(), item.size() * sizeof(char));
        index += item.size();
        registryValue[index++] = '\0';
    }

    registryValue[index++] = '\0';
	string valuestr(registryValue.begin(), registryValue.end());
	ErrorCode err = SetEtcConfigValue(name, valuestr);
	error_ = static_cast<DWORD>(err.ReadValue());
    return IsValid;
}

bool RegistryKey::GetValue(string const & name, DWORD & value)
{
    DWORD size = sizeof(DWORD);
	string valuestr;
	ErrorCode err = GetEtcConfigValue(name, valuestr);
	error_ = static_cast<DWORD>(err.ReadValue());
	char *end;
	value = strtol(valuestr.c_str(), &end, 10);
    return  IsValid;
}

bool RegistryKey::GetValue(string const & name, string & value)
{
	ErrorCode err = GetEtcConfigValue(name,value);
	error_ = static_cast<DWORD>(err.ReadValue());
    return IsValid;
}

bool RegistryKey::GetValue(string const & name, string & value, bool expandEnvironmentStrings)
{
	DWORD size = MAX_VALUE_NAME;
	string val;
	val.resize(static_cast<size_t>(size) / sizeof(char)+1);

	ErrorCode err = GetEtcConfigValue(name, val);
	error_ = static_cast<DWORD>(err.ReadValue());

	val.resize(size / sizeof(char)-1);

	if (expandEnvironmentStrings)
	{
		string expandedVal;
		if (Environment::ExpandEnvironmentStrings(val, expandedVal))
		{
			val = move(expandedVal);
		}
		else
		{
			return false;
		}
	}
	value = val;
	return IsValid;
}

bool RegistryKey::GetValue(string const & name, vector<string> & value)
{
	string valuestr;
	ErrorCode err = GetEtcConfigValue(name, valuestr);
	error_ = static_cast<DWORD>(err.ReadValue());

    vector<string> tempValue;
    size_t index = 0;
	string item = string(&valuestr[index]);
    while (item.size() > 0)
    {
        tempValue.push_back(item);
        index += item.size() + 1;
		item = string(&valuestr[index]);
    }

    value = move(tempValue);
    return true;
}

bool RegistryKey::DeleteValue(string const & name)
{
	ErrorCode err = DeleteEtcConfigValue(name);
	error_ = static_cast<DWORD>(err.ReadValue());
    return IsValid || error_ == ERROR_FILE_NOT_FOUND;
}

bool RegistryKey::DeleteKey()
{
    // nothing to do here
    return IsValid;
}

RegistryKey::~RegistryKey()
{
	// nothing to do here
}

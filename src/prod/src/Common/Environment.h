// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once // intellisense workaround

namespace Common
{
    typedef std::map<std::string, std::string, Common::IsLessCaseInsensitiveComparer<std::string>> EnvironmentMap;

    class Environment :
        Common::TextTraceComponent<Common::TraceTaskCodes::Common>
    {
    public:

        static std::string const NewLine;

        //
        // Get the environment variable
        //
        static void GetEnvironmentVariable( std::string const & name,
                                            std::string& outValue,
                                            NOCLEAR );
        static bool GetEnvironmentVariable( std::string const & name,
                                            std::string& outValue,
                                            NOTHROW );
        static void GetEnvironmentVariable( std::string const & name,
                                            std::string& outValue );

        static void GetEnvironmentVariable( std::string const & name,
                                            std::string& outValue,
                                            std::string const & defaultValue );

        //
        // A GetEnvironmentVariable that conforms to the newer cxl way of doing things
        //
        static std::string GetEnvironmentVariable ( std::string const & name );


        static bool SetEnvironmentVariable(std::string const & name, std::string const& value);

        static std::string GetMachineName() { std::string result; GetMachineName(result); return result; }
        static void GetMachineName( std::string& buffer);
        static void GetMachineFQDN( std::string& buffer);

        static void GetUserName(std::string & buffer);
        static void GetExecutablePath(std::string & buffer);
        static void GetExecutableFileName(std::string & buffer);
        static std::string GetExecutableFileName();
        static std::string GetExecutablePath();
        static void GetCurrentModuleFileName(std::string & buffer);
        static std::string GetCurrentModuleFileName() {std::string result; GetCurrentModuleFileName(result); return result;}
        static void GetModuleFileName(HMODULE module, std::string & buffer);
        static DWORD GetNumberOfProcessors();
        static Common::ErrorCode Environment::GetAvailableMemoryInBytes(__out DWORDLONG& memoryInMB);

        static Common::ErrorCode GetCurrentModuleFileName2(__out std::string & moduleFileName);
        static Common::ErrorCode GetModuleFileName2(HMODULE module, __out std::string & moduleFileName);
        static Common::DateTime GetLastRebootTime();

        static bool ExpandEnvironmentStrings(std::string const & str, __out std::string & expandedStr);
        static Common::ErrorCode Expand(std::string const & str, __out std::string & expandedStr);
        static std::string Expand(std::string const & str);
        static bool GetEnvironmentMap(__out EnvironmentMap & envMap);

#if !defined(PLATFORM_UNIX)
        static DWORD_PTR GetActiveProcessorsMask();
        static std::string GetCurrentModuleFileVersion() { std::string result; GetCurrentModuleFileVersion2(result); return result; }
        static Common::ErrorCode GetFileVersion2(__in std::string const & fileName, __out std::string & fileVersion);
        static Common::ErrorCode GetCurrentModuleFileVersion2(__out std::string & fileVersion);
        static Common::ErrorCode GetEnvironmentMap(HANDLE userTokenHandle, bool inherit, __out EnvironmentMap & envMap);
#endif
        static void ToEnvironmentBlock(EnvironmentMap const & envMap, __out std::vector<char> & envBlock);
        static void FromEnvironmentBlock(LPVOID envBlock, __out EnvironmentMap & envMap);
        static std::string ToString(EnvironmentMap const & envMap);

        static std::string GetHomeFolder();
        static std::string GetHomeFolderW();

        static std::string GetObjectFolder();
    };

    class EnvironmentVariable
    {
    public:
        EnvironmentVariable(
            const std::string &   name,
            const std::string &   defaultValue = "" )
        {
            ASSERT_IF( name.empty(), "environment variable empty");
            
            this->name = name;
            
            Environment::GetEnvironmentVariable( name, currentValue, NOTHROW() );

            if ( currentValue.empty() && !defaultValue.empty() )
            {
                *this = defaultValue;
            }
        } // ctor
        
        std::string const& ToString() const
        {
            return currentValue;
        }

        bool Equals( const std::string & rhs ) const
        {
            return rhs == currentValue;
        }

        EnvironmentVariable & operator = (const std::string & newValue);

        __declspec( property( get=size )) int Count;
        int size()    { return (int)currentValue.size(); }

        __declspec( property( get=empty )) bool IsEmpty;
        bool empty() { return currentValue.empty(); }

    protected:
        std::string           name;
        std::string           currentValue;
    }; // EnvironmentVariable

};

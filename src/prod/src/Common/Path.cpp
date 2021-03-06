// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

#include "Common/Path.h"
#include "Common/Throw.h"

#define ROOT_SEPARATOR_CHAR  ':'
#define ROOT_ESCAPE_PATH     '\"'

#if !defined(PLATFORM_UNIX)

#define PATH_SEPARATOR_CHAR  '\\'
#define PATH_SEPARATOR_WSTR  "\\"


#define PATH_GLOBAL_NAMESPACE_WSTR "\\??\\"

#else

#define PATH_SEPARATOR_CHAR  '/'
#define PATH_SEPARATOR_WSTR  "/"

#define PATH_SEPARATOR_WINCHAR  '\\'
#define PATH_SEPARATOR_WINWSTR  "\\"

#define PATH_GLOBAL_NAMESPACE_WSTR ""

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

DWORD GetTempPathW(DWORD nBufferLength, LPSTR lpBuffer)
{
    if (!lpBuffer)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    char buf[nBufferLength > 0 ? nBufferLength : 1];
    char *tmpdir = getenv("TMPDIR");
    if (!tmpdir)
    {
        tmpdir = "/tmp/";
    }
    string tmpdirW(tmpdir);
    if (tmpdirW.back() != '/')
    {
        tmpdirW.push_back('/');
    }
    if (tmpdirW.length() >= nBufferLength)
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return tmpdirW.length() + 1;
    }
    else
    {
        memcpy(lpBuffer, tmpdirW.c_str(), (tmpdirW.length() + 1) * sizeof(char));
        return tmpdirW.length();
    }
}

#endif

using namespace std;

namespace
{
    void CheckInvalidPathChars(std::string const & path)
    {
        size_t index = path.find_first_of("\"<>|");
        ASSERT_IF(index != std::string::npos, "Path contains invalid characters");
    }

    template <typename StringType>
    struct PathT
    {
        // Changes the extension of a file path. The path parameter
        // specifies a file path, and the extension parameter
        // specifies a file extension (with a leading period, such as
        // ".exe" or ".cs").
        //
        // The function returns a file path with the same root, directory, and base
        // name parts as path, but with the file extension changed to
        // the specified extension. If path is null, the function
        // returns null. If path does not contain a file extension,
        // the new file extension is appended to the path. If extension
        // is null, any exsiting extension is removed from path.
        static void ChangeExtension(StringType & path, StringType const & extension)
        {
            CheckInvalidPathChars(path);

            size_t index = path.rfind('.');
            if (index != StringType::npos)
            {
                path.resize(index);
            }

            if (!extension.empty() && extension[0] != '.')
            {
                path.push_back('.');
            }

            path.append(extension);
        }

        static void EscapePath(StringType & path)
        {
            if (!path.empty())
            {
                path.insert(0, 1, (typename StringType::value_type)ROOT_ESCAPE_PATH);
                path.push_back((typename StringType::value_type)ROOT_ESCAPE_PATH);
            }
        }

        static void CombineInPlace(StringType & path1, StringType const & path2, bool escapePath)
        {
            if (!path2.empty() && !path1.empty())
            {
                if (path1.back() != (typename StringType::value_type)PATH_SEPARATOR_CHAR)
                {
                    path1.push_back((typename StringType::value_type)PATH_SEPARATOR_CHAR);
                }
            }

            path1.append(path2);

            if (escapePath)
            {
                EscapePath(path1);
            }
        }

        static StringType Combine(StringType const & path1, StringType const & path2, bool escapePath)
        {
            StringType result (path1);
            CombineInPlace(result, path2, escapePath);
            return result;
        }

        // Returns the directory path of a file path. This method effectively
        // removes the last element of the given file path, i.e. it returns a
        // string consisting of all characters up to but not including the last
        // backslash ("\") in the file path. The returned value is null if the file
        // path is null or if the file path denotes a root (such as "\", "C:", or
        // "\\server\share").
        static StringType GetDirectoryName(StringType const & path)
        {
            CheckInvalidPathChars(path);
            size_t index = path.rfind((typename StringType::value_type)PATH_SEPARATOR_CHAR);

#if defined(PLATFORM_UNIX)
            size_t index2 = path.rfind((typename StringType::value_type)PATH_SEPARATOR_WINCHAR);
            if(index!=StringType::npos && index2 !=StringType::npos)
                index = (index > index2 ? index: index2);
            else
                index = (index == StringType::npos ? index2 : index);
#endif
            if (index == StringType::npos)
                return StringType();

            if (index == 0)
                return StringType();

            if (index < GetRootLength(path))
            {
                return StringType();
            }

            return path.substr(0, index);
        }

        static size_t GetRootLength(StringType const & path)
        {
            if (path.empty())
                return 0;

            if (path.size() == 1)
                return (path[0] == (typename StringType::value_type)PATH_SEPARATOR_CHAR) ? 1 : 0;

            // path.size >= 2 at this point
            
            if (path[1] == ':')
                return 2;

            if (path[0] != (typename StringType::value_type)PATH_SEPARATOR_CHAR)
            {
                return 0;
            }

            // now path[0] == '\\'

            if (path[1] != (typename StringType::value_type)PATH_SEPARATOR_CHAR)
                return 1;

            // now path.StartsWith("\\\\")

            if (path.size() == 2)
                return 2;

            // get the \\server\share part
            size_t index = path.find((typename StringType::value_type)PATH_SEPARATOR_CHAR, 2);
            if (index == StringType::npos)
            {
                return path.size();
            }

            return index;
        }

        static StringType GetFileNameWithoutExtension(StringType const & path)
        {
            StringType result = GetFileName(path);
            size_t index = result.rfind('.');

            if (index == StringType::npos)
                return result;

            result.resize(index);
            return result;
        }

        static StringType GetFileName(StringType const & path)
        {
            StringType separators({ (typename StringType::value_type)PATH_SEPARATOR_CHAR,(typename StringType::value_type)ROOT_SEPARATOR_CHAR });
            size_t index = path.find_last_of(separators);
#if defined(PLATFORM_UNIX)
			StringType separators2({ (typename StringType::value_type)PATH_SEPARATOR_WINCHAR, (typename StringType::value_type)ROOT_SEPARATOR_CHAR });
            size_t index2 = path.find_last_of(separators2);
			if (index != StringType::npos && index2 != StringType::npos)
			{
				index = (index > index2 ? index : index2);
			}
			else
			{
				index = (index != StringType::npos ? index : index2);
			}
#endif
			if (index == StringType::npos)
			{
				return path;
			}

            return path.substr(index + 1);
        }
    };
}

namespace Common
{
    Common::GlobalString const Path::SfpkgExtension = make_global<std::string>(".sfpkg");

    bool IsValidDriveChar(const char value)
    {
        return ((value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z'));
    }

    int PathStartSkip(std::string const & path)
    {
        int startIndex = 0;
        while (startIndex < path.length() && path[startIndex] == ' ')
        {
            startIndex++;
        }

        if (startIndex > 0 && (startIndex < path.length() && path[startIndex] == PATH_SEPARATOR_CHAR)
            || (startIndex + 1 < path.length() && path[startIndex + 1] == ':' && IsValidDriveChar(path[startIndex])))
        {
            // Go ahead and skip spaces as we're either " C:" or " \"
            return startIndex;
        }

        return 0;
    }

    std::string NormalizeDirectorySeparators(std::string const & path)
    {
        if (path.empty())
        {
            return path;
        }

        char current;
        int startIndex = PathStartSkip(path);

        if (startIndex == 0)
        {
            // Make a pass to see if we need to normalize so we can potentially skip allocating
            bool normalized = true;

            for (int i = 0; i < path.length(); i++)
            {
                current = path[i];
                if (current == PATH_SEPARATOR_CHAR
                    // Check for sequential separators past the first position (we need to keep initial two for UNC/extended)
                    // Or separator is at the end position
                    && (i == path.length() - 1 || (i > 0 && i + 1 < path.length() && path[i + 1] == PATH_SEPARATOR_CHAR)))
                {
                    normalized = false;
                    break;
                }
            }

            if (normalized)
            {
                return path;
            }
        }

        std::stringstream builder;
        if (path[startIndex] == PATH_SEPARATOR_CHAR)
        {
            startIndex++;
            builder << PATH_SEPARATOR_CHAR;
        }

        for (int i = startIndex; i < path.length(); i++)
        {
            current = path[i];

            // If we have a separator
            if (current == PATH_SEPARATOR_CHAR)
            {
                //If a separator is at the rear of the path, skip adding this
                if (i == path.length() - 1)
                {
                    break;
                }

                // If the next is a separator, skip adding this
                if (i + 1 < path.length() && path[i + 1] == PATH_SEPARATOR_CHAR)
                {
                    continue;
                }
            }

            builder << current;
        }

        return builder.str();
    }

    void Path::ChangeExtension(std::string & path, std::string const & extension)
    {
        PathT<string>::ChangeExtension(path, extension);
    }

    std::string Path::GetDirectoryName(std::string const & path)
    {
        return PathT<string>::GetDirectoryName(path);
    }

    // Returns the parent directory path n levels deep, based on "level". 
    // Wraps GetDirectoryName.

    std::string Path::GetParentDirectory(std::string const & directory, int level)
    {
        string parentDirectory = directory;
        for (int i = 0; i < level; ++i)
        {
            parentDirectory = Path::GetDirectoryName(parentDirectory);
        }

        return parentDirectory;
    }

    // Returns the extension of the given path. The returned value includes the
    // period (".") character of the extension 
    std::string Path::GetExtension(std::string const & path)
    {
        CheckInvalidPathChars(path);

        size_t index = path.rfind('.');
        if (index == std::string::npos)
            return "";

        return path.substr(index);
    }

    // Expands the given path to a fully qualified path. The resulting string
    // consists of a drive letter, a colon, and a root relative path. This
    // function does not verify that the resulting path is valid or that it
    // refers to an existing file or DirectoryInfo on the associated volume.

    // Returns the name and extension parts of the given path. The resulting
    // string contains the characters of path that follow the last
    // backslash ("\"), slash ("/"), or colon (":") character in 
    // path. The resulting string is the entire path if path 
    // contains no backslash after removing trailing slashes, slash, or colon characters. The resulting 
    // string is null if path is null.

    std::string Path::GetFileName(std::string const & path)
    {
       return PathT<string>::GetFileName(path);
    }

    std::string Path::GetFileNameWithoutExtension(std::string const & path)
    {
        return PathT<string>::GetFileNameWithoutExtension(path);
    }

    bool Path::IsSfpkg(std::string const & path)
    {
        string extension = Path::GetExtension(path);
        return StringUtility::AreEqualCaseInsensitive(extension, *SfpkgExtension);
    }

    void Path::AddSfpkgExtension(__inout std::string & path)
    {
        Path::ChangeExtension(path, *SfpkgExtension);
    }

    // Returns the root portion of the given path. The resulting string
    // consists of those rightmost characters of the path that constitute the
    // root of the path. Possible patterns for the resulting string are: An
    // empty string (a relative path on the current drive), "\" (an absolute
    // path on the current drive), "X:" (a relative path on a given drive,
    // where X is the drive letter), "X:\" (an absolute path on a given drive),
    // and "\\server\share" (a UNC path for a given server and share name).
    // The resulting string is null if path is null.
    std::string Path::GetPathRoot(std::string const & path)
    {
        CheckInvalidPathChars(path);

        std::string normalizedPath = NormalizeDirectorySeparators(path);

        size_t pathRoot = PathT<string>::GetRootLength(normalizedPath);

        return pathRoot == 0 ? "" : normalizedPath.substr(0, pathRoot);
    }

    std::string Path::NormalizePathSeparators(std::string const & path)
    {
        return NormalizeDirectorySeparators(path);
    }

    /*
     For Windows the absoulte path starts from a drive letter. for ex: C:\test
     For Linux, there is no drive letter. Linux have root defined by /.
     for ex: /home/foo/test. Absoulte paths for linux start with root.
    */
    std::string Path::GetFullPath(std::string const & path)
    {
        auto normalizedPath = NormalizeDirectorySeparators(path);

        auto root = GetPathRoot(normalizedPath);

        if (root.empty())
        {
            normalizedPath = Path::Combine(Directory::GetCurrentDirectory(), normalizedPath);
        }
        else if (root.size() == 1)
        {
            // Path starts with '\', the root should be the drive letter and ':'
            //
            root = Path::GetPathRoot(Directory::GetCurrentDirectory());
            normalizedPath = Path::Combine(root, normalizedPath.substr(1, normalizedPath.size() - 1));
        }

        if (!StringUtility::Contains<string>(normalizedPath, "."))
        {
            return normalizedPath;
        }

        auto pathWithoutRoot = normalizedPath.substr(root.size(), normalizedPath.size() - root.size());

        vector<string> segments;
        StringUtility::Split<string>(pathWithoutRoot, segments, PATH_SEPARATOR_WSTR);

        vector<string> resultSegments;
        for (auto const & segment : segments)
        {
            if (segment == ".")
            {
                continue;
            }
            else if (segment == ".." && !resultSegments.empty())
            {
                resultSegments.pop_back();
            }
            else
            {
                resultSegments.push_back(segment);
            }
        } 

        auto result = root;
        for (auto const & segment : resultSegments)
        {
#if defined(PLATFORM_UNIX)
            result.append(PATH_SEPARATOR_WSTR);
#else
            if (!result.empty())
            {
                result.append(PATH_SEPARATOR_WSTR);
            }
#endif
            result.append(segment);
        }

        return result;
    }

    //std::string Path::GetTempFileName();
    //bool Path::HasExtension(std::string const & path);

    ErrorCode Path::GetTempPath(std::string & tempPath)
    {
        DWORD maxSize = MAX_PATH + 1;
        std::vector<char> tempPathBuffer(maxSize);
        DWORD size = ::GetTempPathW(maxSize, tempPathBuffer.data());

        if(size == 0 || size > maxSize)
        {
            auto error = ErrorCode::FromWin32Error();
            Trace.WriteWarning("Path", "GetTempPath failed with {0}", error);
            return error;
        }

        tempPath = std::string(tempPathBuffer.data());

        return ErrorCodeValue::Success;
    }

    void Path::CombineInPlace(std::string& path1, std::string const & path2, bool escapePath)
    {
        PathT<string>::CombineInPlace(path1, path2, escapePath);
    }

    std::string Path::Combine(std::string const & path1, std::string const & path2, bool escapePath)
    {
        return PathT<string>::Combine(path1, path2, escapePath);
    }

    bool Path::IsPathRooted(std::string const & path)
    {
        return PathT<string>::GetRootLength(path) > 0;
    }

    std::string Path::ConvertToNtPath(std::string const & filePath)
    {
        std::string ntFilePath(filePath);

        // UNC is Windows unique - Do passthru for Linux. 
#if !defined(PLATFORM_UNIX)
        size_t rootLength = PathT<string>::GetRootLength(filePath);
        // Prepend '\\?\' to the path if the path
        // 1) starts with "<Drive>:" or "\\"
        // 2) does not contain \..\ for parent directory
        // 3) is not already a UNC path
        // Safe for Linux since rootLength will be always <= 1
        if (rootLength >= 2 
            && std::string::npos == ntFilePath.find( "\\..\\")
            && std::string::npos == ntFilePath.find( "\\\\?\\" )) 
        {
            if(Path::IsRemotePath(ntFilePath))
            {
                ntFilePath.insert( 0, "\\\\?\\UNC" );
            }
            else
            {
                ntFilePath.insert( 0, "\\\\?\\" );
            }

            // Convert '\\' and '/' to '\' if the path is a UNC path since File IO APIs do not modify UNC paths (
            // http://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx#maxpath
            StringUtility::Replace<std::string>(ntFilePath, "/", "\\");
            StringUtility::Replace<std::string>(ntFilePath, "\\\\", "\\", 2);
        }
#else
        std::replace(ntFilePath.begin(), ntFilePath.end(), '\\', '/');
#endif
        return ntFilePath;
    }

    std::string Path::GetModuleLocation(HMODULE hModule)
    {
        std::string buffer;
        size_t length = MAX_PATH;

        for(; ;)
        {
            buffer.resize(length);

            DWORD dwReturnValue = ::GetModuleFileName(hModule, &buffer[0], static_cast<DWORD>(buffer.size()));
            if (dwReturnValue == 0)
            {
                THROW(WinError(GetLastError()), "GetModuleFileName failed");
            }
            else if (dwReturnValue == static_cast<DWORD>(buffer.size()) && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                // insufficient buffer case -> try with bigger size
                length = length * 2;
                continue;
            }
            else
            {
                // return value is length of string in characters not including terminating null character
                buffer.resize(dwReturnValue);
                break;
            }
        }

        return buffer;
    }

    std::string Path::GetFilePathInModuleLocation(HMODULE hModule, std::string const& filename)
    {
        std::string moduleFilePath = Path::GetModuleLocation(hModule);
        std::string containingFolderPath = Path::GetDirectoryName(moduleFilePath);
        return Path::Combine(containingFolderPath, filename);
    }

    void Path::MakeAbsolute(std::string & file)
    {
        if (Path::IsPathRooted(file)) { return; }

        std::string path(Directory::GetCurrentDirectory());
        CombineInPlace(path, file);
        if (File::Exists(path))
        {
            file = path;
            return;
        }

        path.resize(0);
        Environment::GetExecutablePath(path);
        CombineInPlace(path, file);
        if(File::Exists(path))
        {
            file = path;
            return;
        }
    }

#ifdef PLATFORM_UNIX

    bool Path::IsRegularFile(string const & path)
    {
        struct stat buf = {};
        auto retval = stat(path.c_str(), &buf);
        if (retval < 0)
        {
            auto errNo = errno;
            Trace.WriteWarning("Path", "stat('{0}') failed: {1}", path, errNo); 
            return false;
        }

        return S_ISREG(buf.st_mode);
    }

#else

    ErrorCode Path::QualifyPath(std::string const & path, std::string & qualifiedPath)
    {
        std::vector<char> qualifiedPathVector;
        qualifiedPathVector.resize(MAX_PATH + 1);

        if(!PathSearchAndQualify(path.c_str(), qualifiedPathVector.data(), MAX_PATH + 1))
        {
            return ErrorCode::FromWin32Error();
        }

        qualifiedPath = std::string(qualifiedPathVector.data());

        return ErrorCodeValue::Success;
    }

#endif

    bool Path::IsRemotePath(std::string const & path)
    {
        if(path.empty() || path.size() < 2)
        {
            return false;
        }

        return path[0] == PATH_SEPARATOR_CHAR && path[1] == PATH_SEPARATOR_CHAR;
    }

    char Path::GetPathSeparatorChar()
    {
        return PATH_SEPARATOR_CHAR;
    }

    std::string Path::GetPathSeparatorWstr()
    {
        return PATH_SEPARATOR_WSTR;
    }

    std::string Path::GetPathGlobalNamespaceWstr()
    {
        return PATH_GLOBAL_NAMESPACE_WSTR;
    }

#if !defined(PLATFORM_UNIX) 
    const char Path::GetDriveLetter(std::string const & path)
    {
        string root = GetPathRoot(path);
        if (root.length() == 0)
        {
            return (char)0;
        }

        // Search from rear for valid drive char
        for (int i = (int)(root.length()) - 1; i >= 0; i--)
        {
            if (IsValidDriveChar(root[i]))
            {
                return root[i];
            }
        }

        return (char)0;
    }
#endif

} // end namespace Common} // end namespace Common

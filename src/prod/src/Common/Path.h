// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class Path
    { 
    public:
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
        // is null, any existing extension is removed from path.

        static void ChangeExtension(std::string & path, std::string const & extension);

        // Returns the directory path of a file path. This method effectively
        // removes the last element of the given file path, i.e. it returns a
        // string consisting of all characters up to but not including the last
        // backslash ("\") in the file path. The returned value is null if the file
        // path is null or if the file path denotes a root (such as "\", "C:", or
        // "\\server\share").

        static std::string GetDirectoryName(std::string const & path);

        // Returns the parent directory path n levels deep, based on "level". 
        // Wraps GetDirectoryName.

        static std::string GetParentDirectory(std::string const & directory, int level);

        // Returns the extension of the given path. The returned value includes the
        // period (".") character of the extension except when you have a terminal period when you get std::string.Empty, such as ".exe" or
        // ".cpp". The returned value is null if the given path is
        // null or if the given path does not include an extension.

        static std::string GetExtension(std::string const & path);

        // Expands the given path to a fully qualified path. The resulting string
        // consists of a drive letter, a colon, and a root relative path. This
        // function does not verify that the resulting path is valid or that it
        // refers to an existing file or DirectoryInfo on the associated volume.

        static std::string GetFullPath(std::string const & path);

        // Returns the name and extension parts of the given path. The resulting
        // string contains the characters of path that follow the last
        // backslash ("\"), slash ("/"), or colon (":") character in 
        // path. The resulting string is the entire path if path 
        // contains no backslash after removing trailing slashes, slash, or colon characters. The resulting 
        // string is null if path is null.

        static std::string GetFileName(std::string const & path);

        static std::string GetFileNameWithoutExtension(std::string const & path);

        // Returns the root portion of the given path. The resulting string
        // consists of those rightmost characters of the path that constitute the
        // root of the path. Possible patterns for the resulting string are: An
        // empty string (a relative path on the current drive), "\" (an absolute
        // path on the current drive), "X:" (a relative path on a given drive,
        // where X is the drive letter), "X:\" (an absolute path on a given drive),
        // and "\\server\share" (a UNC path for a given server and share name).
        // The resulting string is null if path is null.
        //
        static std::string GetPathRoot(std::string const & path);
        static Common::ErrorCode GetTempPath(std::string &);
        static std::string GetTempFileName();
        static bool HasExtension(std::string const & path);
        static bool IsPathRooted(std::string const & path);
        static std::string NormalizePathSeparators(std::string const & path);
        static void CombineInPlace(std::string& path1, std::string const & path2, bool escapePath = false);
        static std::string Combine(std::string const & path1, std::string const & path2, bool escapePath = false);

        // Checks whether the path ends with sfpkg extension. Does not validates that the file exists.
        static bool IsSfpkg(std::string const & path);
        static void AddSfpkgExtension(__inout std::string & path);

#ifdef PLATFORM_UNIX
        static bool IsRegularFile(std::string const & path); 
#else
        static ErrorCode QualifyPath(std::string const & path, std::string & qualifiedPath);
#endif

        //
        // Convert the given File path into an NT file path (i.e. \\?\{Device0}...).
        //
        static std::string ConvertToNtPath(std::string const & filePath);

        // Returns the path to the module HMODULE
        // This is a wrapper around GetModuleFileName
        static std::string GetModuleLocation(HMODULE hModule);

        // Returns the path to a file in the folder of the module
        // Example: if the exe is located in C:\foo\hello.exe and the filename is test.config
        // the return value will be C:\foo\test.config
        static std::string GetFilePathInModuleLocation(HMODULE hModule, std::string const& filename);

        // Converts the argument to a valid absolute path. 
        // (1) Checks if the path is rooted.
        // (2) If not rooted, prepends the Current Directory to the path and probes.
        // (3) if probe failed, gets the executable path, probes and returns.
        static void MakeAbsolute(std::string & file);

        static bool IsRemotePath(std::string const & path);

        static char GetPathSeparatorChar();

        static std::string GetPathSeparatorWstr();

        static std::string GetPathGlobalNamespaceWstr();

        // Gets the path root drive letter
        static const char GetDriveLetter(std::string const & path);

        static Common::GlobalString const SfpkgExtension;
    };

} // end namespace Common

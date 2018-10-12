// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "palrt.h"
#include <string>

using namespace std;

static bool isslash(char c)
{
    return c == '/' || c == '\\';
}

STDAPI_(BOOL) PathAppendW(LPSTR pszPath, LPCSTR pszMore)
{
    int lenPath = strlen(pszPath);
    int lenMore = strlen(pszMore);
    if (lenPath > 0 && pszPath[lenPath - 1] == '/' &&
        lenMore > 0 && pszMore[0] == '/')
    {
        pszPath[lenPath - 1] = 0;
    }
    else if (lenPath > 0 && pszPath[lenPath - 1] != '/' &&
             lenMore > 0 && pszMore[0] != '/')
    {
        pszPath[lenPath] = '/';
        pszPath[lenPath + 1] = 0;
    }
    wcsncat(pszPath, pszMore, lenMore);
    return TRUE;
}

STDAPI_(BOOL) PathRemoveFileSpecW(LPSTR pFile)
{
    if (pFile)
    {
        LPSTR slow, fast = pFile;

        for (slow = fast; *fast; fast++)
        {
            if (isslash(*fast))
            {
                slow = fast;
            }
        }

        if (*slow == 0)
        {
            return FALSE;
        }
        else if ((slow == pFile) && isslash(*slow))
        {
            if (*(slow + 1) != 0)
            {
                *(slow + 1) = 0;
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            *slow = 0;
            return TRUE;
        }
    }
    return  FALSE;
}

STDAPI_(LPSTR) PathCombineW(LPSTR lpszDest, LPCSTR lpszDir, LPCSTR lpszFile)
{
    if (lpszDest)
    {
        string dir, file;
        if (lpszDir)
        {
            dir = lpszDir;
            if (!isslash(dir.back()))
            {
                dir.push_back('/');
            }
        }
        if (lpszFile)
        {
            file = lpszFile;
            if (!file.empty() && isslash(file.front()))
            {
                dir = "";
            }
        }
        string path  = dir + file;
        memcpy(lpszDest, path.c_str(), (path.length() + 1) * sizeof(char));
    }
    return lpszDest;
}

#pragma once

#include "ktl.h"

// 
// Pull in a few headers that users of CommandLineParer will need.
// 

#include "string.h"
extern "C" _Check_return_ _CRTIMP int __cdecl _wtoi(_In_z_ const char *_Str);


#undef iswspace
#undef iswalpha
#undef iswalnum
#define iswspace(c)  (c == ' ' || c == '\t' || c == '\r' || c == '\n')
#define iswalpha(c)  ((c >= 'a'  && c <= 'z') || (c >= 'A'  && c <= 'Z') || c == '_')
#define iswalnum(c)  (iswalpha(c) || (c >= '0' && c <= '9'))


static const ULONG MaxParameterNameLength  = 32;
static const ULONG MaxParameters           = 32;
static const ULONG MaxValueLength          = 256;
static const ULONG MaxValuesPerParam       = 32;
static const ULONG MaxTokenBuffer          = 512;

class Parameter
{
    K_DENY_COPY(Parameter);

public:
    char  _Name[MaxParameterNameLength+1];
    ULONG    _ValueCount;
    LPCSTR  _Values[MaxValuesPerParam];

    Parameter(KAllocator& Allocator) 
        :   _Allocator(Allocator)
    {
        _Name[0] = 0;
        _ValueCount = 0;
        for (ULONG i = 0; i < MaxValuesPerParam; i++)          
        {
            _Values[i] = 0;
        }
    }

    ~Parameter()
    {
        for (ULONG i = 0; i < MaxValuesPerParam; i++)
        {
            _delete((PVOID)_Values[i]);
        }
    }

    void InsertValue(LPCSTR Token);

private:
    Parameter();

private:
    KAllocator&     _Allocator;
};

class CmdLineParser
{
    K_DENY_COPY(CmdLineParser);

public:
    CmdLineParser(KAllocator& Allocator);
    ~CmdLineParser();
    BOOLEAN Parse(int Argc, __in_ecount(Argc ) char* Argv[]);
    BOOLEAN Parse(__in_opt LPSTR RawString);
    void Reset();
    BOOLEAN GetErrorInfo(ULONG *CharPos, __out LPSTR* Message);
    ULONG ParameterCount();
    Parameter* GetParam(ULONG Index);

private:
    CmdLineParser();

private:
    static const char* _Msg_InvalidSwitchIdent;
    static const char* _Msg_ExpectingSwitch;
    static const char* _Msg_MissingSwitchIdent;
    static const char* _Msg_UnterminatedString;
    static const char* _Msg_NullString;
    static const char* _Msg_TooManyParameters;
    static const char* _Msg_TooManyValues;
    static const char* _Msg_ParameterNameTooLong;
    static const char* _Msg_ValueTooLong;
    
    KAllocator& _Allocator;
    LPSTR    _Src;
    LPSTR    _ErrorMsg;
    ULONG     _CharPos;
    Parameter* _Params[MaxParameters];
    ULONG      _ParamCount;
    char   _TokenBuffer[MaxTokenBuffer];
    ULONG     _TokenBufferPos;

    char   PeekChar();
    BOOLEAN   EndOfInput();
    char*  CurrentLoc();
    BOOLEAN EmitParam();
    void Cleanup();
    void Zero();
    void AppendValue(char c);
    BOOLEAN EmitValue();
    char NextChar();
    void ResetValueAccumulator();
};

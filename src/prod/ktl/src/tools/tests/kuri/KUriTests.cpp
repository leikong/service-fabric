/*++

Copyright (c) Microsoft Corporation

Module Name:

    KUriTests.cpp

Abstract:

    This file contains test case implementations.

    To add a new unit test case:
    1. Declare your test case routine in KUriTests.h.
    2. Add an entry to the gs_KuTestCases table in KUriTestCases.cpp.
    3. Implement your test case routine. The implementation can be in
    this file or any other file.

--*/
#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(PLATFORM_UNIX)
#include <vector>
#endif
#include <ktl.h>
#include <ktrace.h>
#include "KUriTests.h"
#include <CommandLineParser.h>


#if KTL_USER_MODE
#include <ktlevents.um.h>
#else
#include <ktlevents.km.h>
#endif

#if KTL_USER_MODE
 extern volatile LONGLONG gs_AllocsRemaining;
#endif

KAllocator* g_Allocator = nullptr;

#if KTL_USER_MODE

extern volatile LONGLONG gs_AllocsRemaining;

#endif

LPCSTR LegalUris[] =
{
    "foo#",

    // Mainline simple cases

    "foo-2://",
    "foo:",
    "foo+1:",
    "foo.a.b://",

    "foo://b",
    "foo://b/c"
    "foo://bb/cc"

    // Schemeless

    "foo",
    "foo/a",
    "foo#",
    "foo?#",
    "foo?a#b",
    "124abc/foo"

    "//foo",
    "//foo/a",

    // Complex authority

    "foo://b.a",
    "foo://b.a:80",
    "foo://[fe80::202:b3ff:fe1e:8329]",      // ipv6 inset
    "foo://[fe80::202:b3ff:fe1e:8329]:80",
    "foo://121.32.355.12:80",

    // Fragment weirdness

    "foo:#",
    "foo://#",
    "foo://b#",
    "foo://b/c#",
    "foo://bb/cc#"

    "foo:#f",
    "foo://#f",
    "foo://b#f",
    "foo://b/c#f",
    "foo://bb/cc#f",

    // Query string

    "foo://b/cc?",
    "foo://b/cc?a",
    "foo://b/cc?ab",
    "foo://b/cc?abc=def",

    // Delimiters

    "foo://b/cc?abc=def&xyz=123",
    "foo://b/cc?abc=def|xyz=123",
    "foo://b/cc?abc=def,xyz=123",
    "foo://b/cc?abc=def;xyz=123",
    "foo://b/cc?abc=def+xyz=123",

    // Authorityless

    "foo:b",
    "foo:b/c",
    "foo:/bb/cc",
    "foo:./bb/cc",
    "foo:../bb/cc",

    // Weird
    "foo://",
    "http://fred/"
    "//fred/"

};

LPCSTR IllegalUris[] =
{
    "foo~x://a",
    "1foo://a",
    "foo://[abc]",
    "foo:/[abc]"
};


VOID PrintStr(KStringView& v)
{
    for (ULONG i = 0; i < v.Length(); i++)
    {
        KTestPrintf("%C", v[i]);
    }
}

VOID DumpUri(
    __in KDynUri& Uri
    )
{
    KTestPrintf("\n---Uri Parse Result:---\n");
    KTestPrintf("IsValid    = %S\n", Uri.IsValid() ? "TRUE" : "FALSE");
    if (!Uri.IsValid())
    {
        KTestPrintf("!!! Error at position %d\n", Uri.GetErrorPosition());
    }
    KTestPrintf("StdQueryStr= %S\n", Uri.HasStandardQueryString() ? "TRUE" : "FALSE");

    KTestPrintf("Raw         = ");
    PrintStr(Uri.Get(KUriView::eRaw));
    KTestPrintf("\n");

    KTestPrintf("Scheme      = ");
    PrintStr(Uri.Get(KUriView::eScheme));
    KTestPrintf("\n");

    KTestPrintf("Path        = ");
    PrintStr(Uri.Get(KUriView::ePath));
    KTestPrintf("\n");

    if (Uri.Has(KUri::ePath))
    {
        for (ULONG i = 0; i < Uri.GetSegmentCount(); i++)
        {
            KStringView s;
            Uri.GetSegment(i, s);
            KTestPrintf("    Segment [%d] = ", i);
            PrintStr(s);
            KTestPrintf("\n");
        }
    }

    KTestPrintf("Authority   = ");
    PrintStr(Uri.Get(KUriView::eAuthority));
    KTestPrintf("\n");

    KTestPrintf("Host        = ");
    PrintStr(Uri.Get(KUriView::eHost));
    KTestPrintf("\n");

    KTestPrintf("Port        = %u", Uri.GetPort());
    KTestPrintf("\n");

    KTestPrintf("QueryString = ");
    PrintStr(Uri.Get(KUriView::eQueryString));
    KTestPrintf("\n");

    KTestPrintf("Fragment    = ");
    PrintStr(Uri.Get(KUriView::eFragment));
    KTestPrintf("\n");

    if (Uri.Has(KUriView::eQueryString) && Uri.HasStandardQueryString())
    {
        for (ULONG i = 0; i < Uri.GetQueryStringParamCount(); i++)
        {
            KStringView p, v;
            Uri.GetQueryStringParam(i, p, v);

            KTestPrintf("Query string [%d]  : ", i);
            PrintStr(p);
            KTestPrintf(" = ");
            PrintStr(v);
            KTestPrintf("\n");
        }
    }


    KTestPrintf("----------------------------------\n");
}


NTSTATUS ValidationSuite()
{
    const ULONG TestCount = sizeof(LegalUris)/sizeof(char*);

    for (ULONG i = 0; i < TestCount; i++)
    {
        KStringView x(LegalUris[i]);
        KUriView vv(x);
        if (!vv.IsValid())
        {
            return STATUS_UNSUCCESSFUL;
        }
    }

    return STATUS_SUCCESS;
}


NTSTATUS BasicKUriViewTest()
{
    KUriView v1("http://foo:82/a/b?x=y#ff");
    if (v1.IsEmpty() || !v1.IsValid())
    {
        return STATUS_UNSUCCESSFUL;
    }

    KStringView sc = v1.Get(KUriView::eScheme);
    if (sc.Compare(KStringView("http")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    KStringView host = v1.Get(KUriView::eHost);
    if (host.Compare(KStringView("foo")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    ULONG Port = v1.GetPort();
    if (Port != 82)
    {
        return STATUS_UNSUCCESSFUL;
    }

    KStringView auth = v1.Get(KUriView::eAuthority);
    if (auth.Compare(KStringView("foo:82")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    KStringView path = v1.Get(KUriView::ePath);
    if (path.Compare(KStringView("a/b")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    KStringView q = v1.Get(KUriView::eQueryString);
    if (q.Compare(KStringView("x=y")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    KStringView fr = v1.Get(KUriView::eFragment);
    if (fr.Compare(KStringView("ff")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}


NTSTATUS BasicKUriViewTest_Const()
{
    const KUriView v1("http://foo:82/a/b?x=y#ff");
    if (v1.IsEmpty() || !v1.IsValid())
    {
        return STATUS_UNSUCCESSFUL;
    }

    const KStringView sc = v1.Get(KUriView::eScheme);
    if (sc.Compare(KStringView("http")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    const KStringView host = v1.Get(KUriView::eHost);
    if (host.Compare(KStringView("foo")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    ULONG Port = v1.GetPort();
    if (Port != 82)
    {
        return STATUS_UNSUCCESSFUL;
    }

    const KStringView auth = v1.Get(KUriView::eAuthority);
    if (auth.Compare(KStringView("foo:82")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    const KStringView path = v1.Get(KUriView::ePath);
    if (path.Compare(KStringView("a/b")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    const KStringView q = v1.Get(KUriView::eQueryString);
    if (q.Compare(KStringView("x=y")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    const KStringView fr = v1.Get(KUriView::eFragment);
    if (fr.Compare(KStringView("ff")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

VOID
ComparisonOperatorTests()
{
    KUriView uri1("fabric:/a");
    KUriView uri2("fabric:/a");
    KUriView uri3("fabric:/b");

    KInvariant(uri1 == uri2);
    KInvariant((uri1 == uri3) == FALSE);

    KInvariant(uri1 != uri3);
    KInvariant((uri1 != uri2) == FALSE);
}


NTSTATUS KUriViewTest2()
{
    KUriView v1("http://foo:82/a/b");
    if (v1.IsEmpty() || !v1.IsValid())
    {
        return STATUS_UNSUCCESSFUL;
    }

    KStringView sc = v1.Get(KUriView::eScheme);
    if (sc.Compare(KStringView("http")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    KStringView host = v1.Get(KUriView::eHost);
    if (host.Compare(KStringView("foo")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    ULONG Port = v1.GetPort();
    if (Port != 82)
    {
        return STATUS_UNSUCCESSFUL;
    }

    KStringView auth = v1.Get(KUriView::eAuthority);
    if (auth.Compare(KStringView("foo:82")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    KStringView path = v1.Get(KUriView::ePath);
    if (path.Compare(KStringView("a/b")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (v1.Has(KUriView::eFragment))
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (v1.Has(KUriView::eQueryString))
    {
        return STATUS_UNSUCCESSFUL;
    }


    KUriView v2("http://foo:82/a/b");

    if (!v2.Compare(KUriView::eScheme, v1))
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (!v2.Compare(KUriView::ePath, v1))
    {
        return STATUS_UNSUCCESSFUL;
    }

    KUriView v3("http://foo:82/c/b");
    if (v3.Compare(KUriView::ePath, v1))
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (v3.Compare(v1) == TRUE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (v3.Compare(KUriView("http://foo:82/c/b")) == FALSE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS KDynUriViewTest()
{
    KDynUri v2(KStringView("http://foo:82/a/bc?a1=b2|c3=d4"), *g_Allocator);
    KDynUri v1(*g_Allocator);
    v1.Set(v2);

    if (v1.IsEmpty() || !v1.IsValid())
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (v1.GetSegmentCount() != 2)
    {
        return STATUS_UNSUCCESSFUL;
    }


    KStringView seg;
    v1.GetSegment(0, seg);
    if (seg.Compare(KStringView("a")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    v1.GetSegment(1, seg);
    if (seg.Compare(KStringView("bc")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    ULONG QCount = v1.GetQueryStringParamCount();
    if (QCount != 2)
    {
        return STATUS_UNSUCCESSFUL;
    }

    KStringView p, v;

    v1.GetQueryStringParam(0, p, v);
    if (p.Compare(KStringView("a1")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }
    if (v.Compare(KStringView("b2")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    v1.GetQueryStringParam(1, p, v);
    if (p.Compare(KStringView("c3")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }
    if (v.Compare(KStringView("d4")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}


NTSTATUS
GetRelativeUri()
{
    KUri::SPtr RootUri;
    NTSTATUS Res;

    // Create two URIs for the sample

    Res = KUri::Create(KStringView("rvdtcp://foo:5003/rvd"), *g_Allocator, RootUri);
    if (!NT_SUCCESS(Res))
    {
        return Res;
    }

    KUri::SPtr FullUri;
    Res = KUri::Create(KStringView("rvdtcp://foo:5003/rvd/xx/yy/zz"), *g_Allocator, FullUri);
    if (!NT_SUCCESS(Res))
    {
        return Res;
    }


    // Now assuming we have the two URIs, get string view of the root

    KStringView RootView = RootUri->Get(KUriView::eRaw);

    KDynString Buf(*g_Allocator);
    if (!Buf.CopyFrom(FullUri->Get(KUriView::eRaw)))
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    ULONG Pos = 0;
    if (!Buf.Search(RootView, Pos, 0) || Pos != 0)  // Check that the URIs have the same prefix
    {
        return STATUS_INTERNAL_ERROR;   // URIs don't have the same prefix
    }

    Buf.Remove(0, RootView.Length() + 1);    // Already a KStringView so you can just use this
                                             // The +1 gets read of the leading slash
    Buf.SetNullTerminator();            // May or may not be necessary depending on what you do with it

    // Do whatever with it if you want a KWString

    KWString ResultString(*g_Allocator);

    ResultString = Buf;
    return STATUS_SUCCESS;
}



NTSTATUS
KUriTest()
{
    KUri::SPtr PUri;
    NTSTATUS Res;

    Res = KUri::Create(KStringView("http://foo:82/a/bc?a1=b2|c3=d4"), *g_Allocator, PUri);
    if (!NT_SUCCESS(Res))
    {
        return Res;
    }

    if (!PUri->IsValid())
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (PUri->GetSegmentCount() != 2)
    {
        return STATUS_UNSUCCESSFUL;
    }

    KUri::SPtr Copy;

    Res = PUri->Clone(Copy);
    if (!NT_SUCCESS(Res))
    {
        return Res;
    }

    if (PUri->Compare(*Copy) != TRUE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    KUri::SPtr Uri2;
    Res = KUri::Create(KStringView("http://foo:81/a/bc?a1=b2|c3=d4"), *g_Allocator, Uri2);
    if (!NT_SUCCESS(Res))
    {
        return Res;
    }

    // Only differs by port
    //
    if (PUri->Compare(*Uri2) == TRUE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (PUri->Compare(KUriView::eScheme | KUriView::eHost | KUriView::ePath | KUriView::eQueryString, *Uri2) != TRUE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    // Comparison operator test
    //
    if (*PUri != *Copy)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (*PUri <= *Uri2)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (*Uri2 >= *PUri)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PrefixTests()
{
    KDynUri Base(KStringView("foo://a/b"), *g_Allocator);

    KDynUri Candidate1(KStringView("foo://a/b?x=y"), *g_Allocator);
    KDynUri Candidate2(KStringView("foo://a/b"), *g_Allocator);
    KDynUri Candidate3(KStringView("foo://a/b/c"), *g_Allocator);
    KDynUri Candidate4(KStringView("foo://a/b/c#af"), *g_Allocator);
    KDynUri Candidate5(KStringView("foo://a/b#af"), *g_Allocator);

    KDynUri Candidate6(KStringView("foo://a/c"), *g_Allocator);
    KDynUri Candidate7(KStringView("foo://a:9/b/c"), *g_Allocator);
    KDynUri Candidate8(KStringView("fox://a/b/c"), *g_Allocator);


    if (Base.IsPrefixFor(Candidate1) != TRUE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (Base.IsPrefixFor(Candidate2) != TRUE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (Base.IsPrefixFor(Candidate3) != TRUE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (Base.IsPrefixFor(Candidate4) != TRUE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (Base.IsPrefixFor(Candidate5) != TRUE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    // These should not match

    if (Base.IsPrefixFor(Candidate6) == TRUE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (Base.IsPrefixFor(Candidate7) == TRUE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (Base.IsPrefixFor(Candidate8) == TRUE)
    {
        return STATUS_UNSUCCESSFUL;

    }

    // Test case from driver

    KDynUri RvdTestBase(KStringView("rvd://343659C03-01:5005"), *g_Allocator);
    KDynUri Other(KStringView("rvd://343659C03-01:5005/csi/{c43c8c05-0648-3d30-8d70-2b075e3092f0}"), *g_Allocator);

    if (RvdTestBase.IsPrefixFor(Other) == FALSE)
    {
        return STATUS_UNSUCCESSFUL;
    }
    return STATUS_SUCCESS;
}

NTSTATUS
ComposeTests()
{
    KDynUri Base(KStringView("foo://a/b"), *g_Allocator);
    KUriView Suffix("x/y");

    KUri::SPtr NewUri;
    NTSTATUS Res = KUri::Create(Base, Suffix, *g_Allocator, NewUri);
    if (!NT_SUCCESS(Res))
    {
        return Res;
    }
    KStringView Result = *NewUri;

    if (Result.Compare(KStringView("foo://a/b/x/y")) != 0)
    {
        return STATUS_UNSUCCESSFUL;
    }
    return STATUS_SUCCESS;
}

VOID
DumpOutput(
    __in KArray<KDomPath::PToken>& Output
    )
{
    KTestPrintf("\n-------------------------\n");
    for (ULONG i = 0; i < Output.Count(); i++)
    {
        KTestPrintf("Item [%d] ", i);

        switch (Output[i]._IdentType)
        {
            case KDomPath::eNone:
                KTestPrintf(" : <no type; internal error>\n");
                break;

            case KDomPath::eElement:
                KTestPrintf(" : element\n");
                break;

            case KDomPath::eAttribute:
                KTestPrintf(" : attribute\n");
                break;

            case KDomPath::eArrayElement:
                {
                    KTestPrintf(" : array ");
                    if (Output[i]._LiteralIndex == -1)
                    {
                        KTestPrintf(" ; bounds unspecified\n");
                    }
                    else
                    {
                        KTestPrintf(" ; literal index (%d)\n", Output[i]._LiteralIndex);
                    }
                }
                break;

        }

        KTestPrintf("   Ident = '%S'\n", LPSTR(*Output[i]._Ident));
        if (Output[i]._NsPrefix)
        {
            KTestPrintf("   Ns    = %S\n", LPSTR(*Output[i]._NsPrefix));
        }
        else
        {
            KTestPrintf("   Ns    = (empty)\n");
        }
    }
}


NTSTATUS KDomPathTests()
{
    LPCSTR GoodPaths[] = {
        "xns:ab",
        "a/@xns:ccc",
        "a/b/c[1]/d[2]/@x",
        "ab[2]/@x",
        "aa/bb[]",
        "ab",
        "a/b",
        "a/b.c/c_d",
        "a/b/@x",
        "a/b/c[1]/@x",
        "a/ns1:b/c[1]/ns2:d[2]/@ns3:x"
        };

    ULONG GoodPathsCount = sizeof(GoodPaths)/sizeof(LPCSTR);

    LPCSTR BadPaths[] = {
        "",
        "/a",
        "a/b/",
        "//",
        "a/b/@@x",
        "a/b/c[0]/@x@",
        "a/b/c]/@x",
        "a/[b]/c[1]/d[2]/@x"
        "a/ns1::b/"
        };

    ULONG BadPathsCount = sizeof(BadPaths)/sizeof(LPCSTR);


    for (ULONG i = 0; i < GoodPathsCount; i++)
    {
        KDomPath p(GoodPaths[i]);
        KArray<KDomPath::PToken> ParseOutput(*g_Allocator);

        NTSTATUS Res = p.Parse(*g_Allocator, ParseOutput);

        if (!NT_SUCCESS(Res))
        {
            return Res;
        }

        DumpOutput(ParseOutput);
    }

    for (ULONG i = 0; i < BadPathsCount; i++)
    {
        KDomPath p(BadPaths[i]);
        KArray<KDomPath::PToken> ParseOutput(*g_Allocator);

        NTSTATUS Res = p.Parse(*g_Allocator, ParseOutput);

        if (NT_SUCCESS(Res))
        {
            return STATUS_INTERNAL_ERROR;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
SuffixTests()
{
    NTSTATUS Status;
    KDynUri Tmp1(*g_Allocator);

    Status = Tmp1.Set(KStringView("foo://basic/uri"));

    if (!NT_SUCCESS(Status))
    {
        return STATUS_UNSUCCESSFUL;
    }

    KUri::SPtr Main;

    Status = KUri::Create(Tmp1, *g_Allocator, Main);
    if (!NT_SUCCESS(Status))
    {
        return STATUS_UNSUCCESSFUL;
    }

    Tmp1.Clear();
    Status = Tmp1.Set(KStringView("foo://basic/uri/longer/than/original"));
    if (!NT_SUCCESS(Status))
    {
        return STATUS_UNSUCCESSFUL;
    }

    KDynUri Diff(*g_Allocator);
    Status = Main->GetSuffix(Tmp1, Diff);
    if (!NT_SUCCESS(Status))
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (!Diff.Compare(KUriView("longer/than/original")))
    {
        return STATUS_UNSUCCESSFUL;
    }

    // Test query string & fragment


    Tmp1.Clear();
    Status = Tmp1.Set(KStringView("foo://basic/uri/longer/than/original?q1=v1&q2=v2#frag"));
    if (!NT_SUCCESS(Status))
    {
        return STATUS_UNSUCCESSFUL;
    }

    Status = Main->GetSuffix(Tmp1, Diff);
    if (!NT_SUCCESS(Status))
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (!Diff.Compare(KUriView("longer/than/original?q1=v1&q2=v2#frag")))
    {
        return STATUS_UNSUCCESSFUL;
    }


    // This test failed originally using Winfab URLs
    //
    KDynUri LongUri(*g_Allocator);

    Status = LongUri.Set(KStringView("http://RAYMCCMOBILE8:80/pav/svc/fabric:/rvd/testnodeservice:129905745079993957"));
    if (!NT_SUCCESS(Status))
    {
        return STATUS_UNSUCCESSFUL;
    }

    Status = Tmp1.Set(KStringView("http://RAYMCCMOBILE8:80/pav/svc/fabric:/rvd/testnodeservice:129905745079993957"));
    Diff.Clear();
    Status = LongUri.GetSuffix(Tmp1, Diff);
    if (!NT_SUCCESS(Status))
    {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Root URI
    //
    KDynUri RootUri(*g_Allocator);

    Status = RootUri.Set(KStringView("http://fred/"));
    if (!NT_SUCCESS(Status))
    {
        return STATUS_UNSUCCESSFUL;
    }

    Status = Tmp1.Set(KStringView("http://fred/nodes/mynode/blah"));
    Diff.Clear();
    Status = RootUri.GetSuffix(Tmp1, Diff);
    if (!NT_SUCCESS(Status))
    {
        return STATUS_UNSUCCESSFUL;
    }

    KDynUri RootUri2(*g_Allocator);

    Status = RootUri2.Set(KStringView("//fred/"));
    if (!NT_SUCCESS(Status))
    {
        return STATUS_UNSUCCESSFUL;
    }

    Status = Tmp1.Set(KStringView("//fred/nodes/mynode/blah"));
    Diff.Clear();
    Status = RootUri2.GetSuffix(Tmp1, Diff);
    if (!NT_SUCCESS(Status))
    {
        return STATUS_UNSUCCESSFUL;
    }


    return STATUS_SUCCESS;
}

NTSTATUS
WildcardTests()
{
    KUriView Auth("foo://*/blah/blah2");
    KUriView Test("foo://auth/blah/blah2");
    KUriView Test2("foo://xx/blah/blah2");

    if (Auth.Compare(Test))
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (Auth.Compare(KUriView::eLiteralHost, Test))
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (!Auth.Compare(KUriView::eHost, Test))
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (!Auth.Compare(KUriView::eHost, Test2))
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (!Auth.Compare(KUriView::eScheme | KUriView::ePath, Test))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}


NTSTATUS
TestSequence()
{
    KTestPrintf("Starting tests...\n");

    NTSTATUS Status;

    Status = WildcardTests();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }


    Status = SuffixTests();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }
    Status = KDomPathTests();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }


    Status = ComposeTests();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }


    Status = PrefixTests();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }


    Status = GetRelativeUri();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    Status = ValidationSuite();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    Status = BasicKUriViewTest();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    Status = BasicKUriViewTest_Const();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    ComparisonOperatorTests();

    Status = KUriViewTest2();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    Status = KDynUriViewTest();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    Status = KUriTest();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    KTestPrintf("Completed. Success.\n");

    return STATUS_SUCCESS;
}


VOID TestCommandLineUri(
    __in LPCSTR Test
    )
{
    KDynUri v(*g_Allocator);
    v.Set(KStringView(Test));
    DumpUri(v);
}


NTSTATUS
KUriTest(
    int argc, CHAR* args[]
    )
{
    KTestPrintf("KUriTest: STARTED\n");

    NTSTATUS Result;
    Result = KtlSystem::Initialize();
    if (!NT_SUCCESS(Result))
    {
        KTestPrintf("KtlSystem::Initializer() failure\n");
        return Result;
    }
    KFinally([&](){ KtlSystem::Shutdown(); });
    g_Allocator = &KtlSystem::GlobalNonPagedAllocator();

    ULONGLONG StartingAllocs = KAllocatorSupport::gs_AllocsRemaining;

    EventRegisterMicrosoft_Windows_KTL();

    if (argc == 0)
    {
        Result = TestSequence();
        if (! NT_SUCCESS(Result))
        {
            KTestPrintf("Test failed %x\n", Result);
        }
    }
    else
    {
        TestCommandLineUri(args[0]);
    }

    ULONGLONG Leaks = KAllocatorSupport::gs_AllocsRemaining - StartingAllocs;
    if (Leaks)
    {
        KTestPrintf("Leaks = %u\n", Leaks);
        Result = STATUS_UNSUCCESSFUL;
    }
    else
    {
        KTestPrintf("No leaks.\n");
    }

    EventUnregisterMicrosoft_Windows_KTL();

    KTestPrintf("KUriTest: COMPLETED\n");
    return Result;
}



#if CONSOLE_TEST
int
#if !defined(PLATFORM_UNIX)
wmain(int argc, CHAR* args[])
{
#else
main(int argc, char* cargs[])
{
    CONVERT_TO_ARGS(argc, cargs)
#endif
    if (argc > 0)
    {
        argc--;
        args++;
    }

    return KUriTest(argc, args);
}
#endif




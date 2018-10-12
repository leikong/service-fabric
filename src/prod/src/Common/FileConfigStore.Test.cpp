// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"
//#include "WexTestClass.h"
#include <algorithm>
#include "Common/Config.h"
#include "Common/FileConfigStore.h"


#include <boost/test/unit_test.hpp>

Common::StringLiteral const TraceType("FileConfigStoreTest");

using namespace Common;

//namespace Common
//{
    using std::string;
    using std::string;
    using std::for_each;

//    namespace 
//    {
        static void PrintStringCollection(StringCollection const & coll, string const & header)
        {
            Trace.WriteInfo(TraceType, "{0}:", header);
            for_each(coll.begin(), coll.end(), [](string const & elem) { Trace.WriteInfo(TraceType, "\t\"{0}\"", elem); });
        }

        static void CheckStringCollectionsEquality(StringCollection & coll1, StringCollection & coll2)
        {
            sort(coll1.begin(), coll1.end());
            sort(coll2.begin(), coll2.end());
            BOOST_REQUIRE(coll1 == coll2);
        }

        static const string section1 = "SimpleSection";
        static const string section2 = "Header/Section1";
        static const string section3 = "Header/Section2";
        static const string header = "header";
//    }

    // Tests FileConfigStore
    // For simplicity, it reads the config content from strings
    // Config tests load the config content from files.
    class TestFileConfigStore //: public WEX::TestClass<TestFileConfigStore>
    {
    public:
        //TEST_CLASS(TestFileConfigStore)

            //TEST_METHOD(TestLoadGetSectionsAndKeys)
            //TEST_METHOD(TestRead)
            //TEST_METHOD(BadConfig_IncorrectSectionName)
            //TEST_METHOD(BadConfig_IncompleteSectionName)
            //TEST_METHOD(BadConfig_TwoValuesPerRow)
            //TEST_METHOD(BadConfig_KeyValueOutsideSection)
            //TEST_METHOD(BadConfig_NoKeyValueDefined)
            //TEST_METHOD(BadConfig_Empty)
    };


    BOOST_FIXTURE_TEST_SUITE(Common, TestFileConfigStore)

    BOOST_AUTO_TEST_CASE(TestLoadGetSectionsAndKeys)
    //void TestFileConfigStore::TestLoadGetSectionsAndKeys()
    {
        // Read the config file with the following content:
        string input = "[SimpleSection]\r\n"
            "   intKey = 1453\r\n"
            "   stringKey=this is my long string key\r\n"
            "   stringCollKey=First,second,last\r\n"
            "   boolKey=true\r\n"
            "\r\n"
            "[Header/Section1]\r\n"
            "   commonKey1 = value1\r\n"
            "   commonKey2 = 160\r\n"
            "[Header/Section2]\r\n"
            "   commonKey1 = value1\r\n"
            "   commonKey2 = 322\r\n"
            "\r\n";

        FileConfigStore config(input, true);

        // Check that config has the correct sections
        StringCollection sections;
        config.GetSections(sections, "");

        StringCollection desiredSections;
        desiredSections.push_back(section1);
        desiredSections.push_back(section2);
        desiredSections.push_back(section3);

        PrintStringCollection(sections, "Parsed sections");
        CheckStringCollectionsEquality(desiredSections, sections);

        // Check that each section has the desired keys
        StringCollection keys;
        config.GetKeys(section1, keys, "");

        StringCollection desiredKeys;
        desiredKeys.push_back("intKey");
        desiredKeys.push_back("stringKey");
        desiredKeys.push_back("stringCollKey");
        desiredKeys.push_back("boolKey");

        PrintStringCollection(keys, "Parsed keys for section1");
        CheckStringCollectionsEquality(desiredKeys, keys);

        // The last 2 sections have the same keys
        desiredKeys.clear();
        desiredKeys.push_back("commonKey1");
        desiredKeys.push_back("commonKey2");

        keys.clear();
        config.GetKeys(section2, keys, "");
        PrintStringCollection(keys, "Parsed keys for section2");
        CheckStringCollectionsEquality(desiredKeys, keys);

        keys.clear();
        config.GetKeys(section3, keys, "");
        PrintStringCollection(keys, "Parsed keys for section3");
        CheckStringCollectionsEquality(desiredKeys, keys);

        // Check get sections that match a certain condition
        sections.clear();
        config.GetSections(sections, header);
        StringCollection desiredHeaders;
        desiredHeaders.push_back(section2);
        desiredHeaders.push_back(section3);
        PrintStringCollection(sections, "Sections that match *header*");
        CheckStringCollectionsEquality(desiredHeaders, sections);
    }

    BOOST_AUTO_TEST_CASE(TestRead)
    //void TestFileConfigStore::TestRead()
    {
        string input = "[SimpleSection]\r\n"
            "   stringKey=this is my long string key\r\n";

        FileConfigStore store(input, true);

        // Read existent/non-existent string key
        bool isEncrypted;
        string stringResult = store.ReadString(section1,  "stringKey", isEncrypted);
        ASSERT_IF(isEncrypted, "FileConfigStore does not support encrypted values");
        
        BOOST_REQUIRE(stringResult == "this is my long string key");
        
        stringResult = store.ReadString(section1,  "stringKey-nonExistent", isEncrypted);
        ASSERT_IF(isEncrypted, "FileConfigStore does not support encrypted values");

        BOOST_REQUIRE(stringResult.empty());
    }

    BOOST_AUTO_TEST_CASE(BadConfig_IncorrectSectionName)
    //void TestFileConfigStore::BadConfig_IncorrectSectionName()
    {
        string badInput = "[Se[ctio[n1] ; too many [ inside section name \r\n"
            "   key = value";

        // This should throw because the config file is not valid
        Assert::DisableDebugBreakInThisScope disableDebugBreakInThisScope;
        BOOST_REQUIRE_THROW(FileConfigStore config(badInput, true), std::system_error);
    }

    BOOST_AUTO_TEST_CASE(BadConfig_IncompleteSectionName)
    //void TestFileConfigStore::BadConfig_IncompleteSectionName()
    {
        string badInput = "[Section1]\r\n"
            "   key1=value1\r\n"
            "[Section2 ; missing ] to close section\r\n"
            "   key2 = value2";

        // This should throw because the config file is not valid
        Assert::DisableDebugBreakInThisScope disableDebugBreakInThisScope;
        BOOST_REQUIRE_THROW(FileConfigStore config(badInput, true), std::system_error);
    }

    BOOST_AUTO_TEST_CASE(BadConfig_TwoValuesPerRow)
    //void TestFileConfigStore::BadConfig_TwoValuesPerRow()
    {
        string badInput = 
            "[Section1]\r\n"
            "   key1=value1=value2 ; error because we have 2 = on the same line";

        // This should throw because the config file is not valid
        Assert::DisableDebugBreakInThisScope disableDebugBreakInThisScope;
        BOOST_REQUIRE_THROW(FileConfigStore config(badInput, true), std::system_error);
    }

    BOOST_AUTO_TEST_CASE(BadConfig_KeyValueOutsideSection)
    //void TestFileConfigStore::BadConfig_KeyValueOutsideSection()
    {
        string badInput = "key1=value1 ; error, keys defined outside a section\r\n"
            "key2 = value2";

        // This should throw because the config file is not valid
        Assert::DisableDebugBreakInThisScope disableDebugBreakInThisScope;
        BOOST_REQUIRE_THROW(FileConfigStore config(badInput, true), std::system_error);
    }

    BOOST_AUTO_TEST_CASE(BadConfig_NoKeyValueDefined)
    //void TestFileConfigStore::BadConfig_NoKeyValueDefined()
    {
        string badInput = "[Section]\r\n"
            "  key1 : value1 ; error, instead of = use :, which is not a valid separator";

        // This should throw because the config file is not valid
        Assert::DisableDebugBreakInThisScope disableDebugBreakInThisScope;
        BOOST_REQUIRE_THROW(FileConfigStore(badInput, true), std::system_error);
    }

    BOOST_AUTO_TEST_CASE(BadConfig_Empty)
    //void TestFileConfigStore::BadConfig_Empty()
    {
        string badInput = "[Section]\r\n"
            "  =EmptyKey; error, no key name is defined";

        // This should throw because the config file is not valid
        Assert::DisableDebugBreakInThisScope disableDebugBreakInThisScope;
        BOOST_REQUIRE_THROW(FileConfigStore config(badInput, true), std::system_error);
    }

    BOOST_AUTO_TEST_SUITE_END()
//}

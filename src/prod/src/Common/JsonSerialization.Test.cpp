﻿// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

#include <boost/test/unit_test.hpp>
#include "Common/boost-taef.h"

using namespace Common;
using namespace std;

namespace Common
{
    namespace ObjectKind
    {
        enum Enum
        {
            OBJECT_KIND_1 = 1,
            OBJECT_KIND_2 = 2,
            OBJECT_KIND_3 = 3,
            OBJECT_KIND_INVALID = 4
        };

        BEGIN_DECLARE_ENUM_JSON_SERIALIZER(Enum)
            ADD_ENUM_VALUE(OBJECT_KIND_1)
            ADD_ENUM_VALUE(OBJECT_KIND_2)
            ADD_ENUM_VALUE(OBJECT_KIND_3)
            ADD_ENUM_VALUE(OBJECT_KIND_INVALID)
        END_DECLARE_ENUM_SERIALIZER()

        void WriteToTextWriter(__in Common::TextWriter & w, Enum const & val)
        {
            switch (val)
            {
            case OBJECT_KIND_1:
                w << "OBJECT_KIND_1";
                return;
            case OBJECT_KIND_2:
                w << "OBJECT_KIND_2";
                return;
            case OBJECT_KIND_3:
                w << "OBJECT_KIND_3";
                return;
            case OBJECT_KIND_INVALID:
                w << "OBJECT_KIND_INVALID";
                return;
            default:
                Common::Assert::CodingError("Unknown ObjectKind value {0}", static_cast<uint>(val));
            }
        }
    }

    class TempObject;
    typedef shared_ptr<TempObject> TempObjectSPtr;
    class TempObject1;
    typedef shared_ptr<TempObject1> TempObject1SPtr;
    class ObjectBase;
    typedef shared_ptr<ObjectBase> ObjectBaseSPtr;

    class ObjectBase : public IFabricJsonSerializable
    {
    public:
        ObjectBase()
            : kind_(ObjectKind::OBJECT_KIND_INVALID)
        {
            StringUtility::GenerateRandomString(10, id_);
        }

        explicit ObjectBase(ObjectKind::Enum kind)
            : kind_(kind)
        {
            StringUtility::GenerateRandomString(10, id_);
        }

        ObjectBase(ObjectKind::Enum kind, string const &id)
            : kind_(kind)
            , id_(id)
        {
        }

        static ObjectBaseSPtr CreateSPtr(ObjectKind::Enum kind);

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY_ENUM("Kind", kind_)
            SERIALIZABLE_PROPERTY("ObjectId", id_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        JSON_TYPE_ACTIVATOR_METHOD(ObjectBase, ObjectKind::Enum, kind_, CreateSPtr)

        ObjectKind::Enum kind_;
        std::string id_;
    };

    class TempObject : public ObjectBase
    {
    public:
        TempObject()
            : ObjectBase(ObjectKind::OBJECT_KIND_1)
        {}

        TempObject(std::string name, int val)
            : ObjectBase(ObjectKind::OBJECT_KIND_1)
            , name_(name)
            , val_(val)
        {
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY_CHAIN()
            SERIALIZABLE_PROPERTY("ObjectName", name_)
            SERIALIZABLE_PROPERTY("ObjectValue", (LONG&)val_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        bool operator == (TempObject const& other) const
        {
            if ((val_ == other.val_)
                && (name_ == other.name_)
                && (id_ == other.id_))
            {
                return true;
            }
            return false;
        }

        std::string name_;
        int val_;
    };

    // field val_ is of type string instead of int
    class SimilarToTempObject : public ObjectBase
    {
    public:
        SimilarToTempObject()
            : ObjectBase(ObjectKind::OBJECT_KIND_1)
        {}

        SimilarToTempObject(std::string name, std::string val)
            : ObjectBase(ObjectKind::OBJECT_KIND_1)
            , name_(name)
            , val_(val)
        {
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY_CHAIN()
            SERIALIZABLE_PROPERTY("ObjectName", name_)
            SERIALIZABLE_PROPERTY("ObjectValue", val_)
            END_JSON_SERIALIZABLE_PROPERTIES()

            bool operator == (SimilarToTempObject const& other) const
        {
                if ((val_ == other.val_)
                    && (name_ == other.name_)
                    && (id_ == other.id_))
                {
                    return true;
                }
                return false;
            }

        std::string name_;

        // field val_ is of type string instead of int
        std::string val_;
    };

    class TempObject1 : public ObjectBase
    {
    public:
        TempObject1()
            : ObjectBase(ObjectKind::OBJECT_KIND_2)
        {}

        TempObject1(bool state, int val)
            : ObjectBase(ObjectKind::OBJECT_KIND_2)
            , state_(state)
            , val_(val)
        {
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY_CHAIN()
            SERIALIZABLE_PROPERTY("ObjectState", state_)
            SERIALIZABLE_PROPERTY("ObjectValue", (LONG&)val_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        bool operator == (TempObject1 const& other) const
        {
            if ((val_ == other.val_)
                && (state_ == other.state_)
                && (id_ == other.id_))
            {
                return true;
            }
            return false;
        }

        bool state_;
        int val_;
    };

    class TempObjectWithArray : public ObjectBase
    {
    public:
        TempObjectWithArray()
            : ObjectBase(ObjectKind::OBJECT_KIND_3)
        {}

        TempObjectWithArray(string id, vector<int> &&array)
            : ObjectBase(ObjectKind::OBJECT_KIND_3, id)
            , array_(move(array))
        {}

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY_CHAIN()
            SERIALIZABLE_PROPERTY("Array", array_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        bool operator == (TempObjectWithArray const& other) const
        {
            if (other.id_ != id_)
            {
                return false;
            }

            if (other.array_.size() != array_.size())
            {
                return false;
            }

            for (int i = 0; i < array_.size(); ++i)
            {
                if (array_[i] != other.array_[i])
                {
                    return false;
                }
            }

            return true;
        }

        vector<int> array_;
    };

    template<ObjectKind::Enum kind>
    class TypeActivator
    {
    public:
        static ObjectBaseSPtr CreateSPtr()
        {
            return make_shared<ObjectBase>(ObjectKind::OBJECT_KIND_INVALID);
        }
    };

    template<> class TypeActivator<ObjectKind::OBJECT_KIND_1>
    {
    public:
        static ObjectBaseSPtr CreateSPtr()
        {
            return make_shared<TempObject>();
        }
    };

    template<> class TypeActivator<ObjectKind::OBJECT_KIND_2>
    {
    public:
        static ObjectBaseSPtr CreateSPtr()
        {
            return make_shared<TempObject1>();
        }
    };

    template<> class TypeActivator<ObjectKind::OBJECT_KIND_3>
    {
    public:
        static ObjectBaseSPtr CreateSPtr()
        {
            return make_shared<TempObjectWithArray>();
        }
    };

    ObjectBaseSPtr ObjectBase::CreateSPtr(ObjectKind::Enum kind)
    {
        switch(kind)
        {
        case ObjectKind::OBJECT_KIND_1 : { return TypeActivator<ObjectKind::OBJECT_KIND_1>::CreateSPtr(); }
        case ObjectKind::OBJECT_KIND_2 : { return TypeActivator<ObjectKind::OBJECT_KIND_2>::CreateSPtr(); }
        case ObjectKind::OBJECT_KIND_3 : { return TypeActivator<ObjectKind::OBJECT_KIND_3>::CreateSPtr(); }
        default: return make_shared<ObjectBase>();
        }
    }

    class NestedObjectArray : public IFabricJsonSerializable
    {
    public:
        NestedObjectArray()
            : NestedObjectArray(0)
        {
        }

        NestedObjectArray(int elements)
        {
            for (int i = 0; i < elements; i++)
            {
                std::string str;
                Common::StringUtility::GenerateRandomString(10, str);
                TempObject obj(str, i);
                nestedArray_.push_back(obj);
                nestedArray1_.push_back(obj);
            }
            obj_.name_ = "objectname";
            obj_.val_ = elements;

            delimiter_ = 12345;
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("Array", nestedArray_)
            SERIALIZABLE_PROPERTY("Delimiter", delimiter_)
            SERIALIZABLE_PROPERTY("Array1", nestedArray1_)
            SERIALIZABLE_PROPERTY("TempObject", obj_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        bool operator == (NestedObjectArray const& other) const
        {
            if (nestedArray_.size() != other.nestedArray_.size())
            {
                return false;
            }

            for (size_t i = 0; i < nestedArray_.size(); i++)
            {
                if (!(nestedArray_[i] == other.nestedArray_[i])) { return false; }
            }

            return (obj_ == other.obj_);
        }

        std::vector<TempObject> nestedArray_;
        LONG delimiter_;
        TempObject obj_;
        std::vector<TempObject> nestedArray1_;
    };

    class ObjectSimilarToNestedObjectArray : public IFabricJsonSerializable
    {
    public:
        ObjectSimilarToNestedObjectArray(int elements)
        {
            for (int i = 0; i < elements; i++)
            {
                std::string temp_str;
                Common::StringUtility::GenerateRandomString(10, temp_str);
                TempObject obj(temp_str, i);
                nestedArray_.push_back(obj);
            }
            delimiter_ = 12345;
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("Array", nestedArray_)
            SERIALIZABLE_PROPERTY("Delimiter", delimiter_)
            SERIALIZABLE_PROPERTY("Array1", str)
        END_JSON_SERIALIZABLE_PROPERTIES()

        bool operator == (NestedObjectArray const& other) const
        {
            if (nestedArray_.size() != other.nestedArray_.size())
            {
                return false;
            }

            for (size_t i = 0; i < nestedArray_.size(); i++)
            {
                if (!(nestedArray_[i] == other.nestedArray_[i])) { return false; }
            }
            return true;
        }

        std::vector<TempObject> nestedArray_;
        LONG delimiter_;
        std::string str;
    };

    class SimpleArray : public IFabricJsonSerializable
    {
    public:
        SimpleArray(int elements)
        {
            for (int i = 0; i < elements; i++)
            {
                std::string str;
                Common::StringUtility::GenerateRandomString(10, str);
                stringArray_.push_back(str);
                intArray_.push_back(i);
            }
            delimiter_ = 12345;
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("Array", stringArray_)
            SERIALIZABLE_PROPERTY("Delimiter", delimiter_)
            SERIALIZABLE_PROPERTY("Array1", intArray_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        bool operator == (SimpleArray const& other) const
        {
            if (stringArray_.size() != other.stringArray_.size() ||
                intArray_.size() != other.intArray_.size())
            {
                return false;
            }

            for (size_t i = 0; i < stringArray_.size(); i++)
            {
                if (!(stringArray_[i] == other.stringArray_[i])) { return false; }
            }

            for (size_t i = 0; i < intArray_.size(); i++)
            {
                if (!(intArray_[i] == other.intArray_[i])) { return false; }
            }
            return true;
        }

        std::vector<string> stringArray_;
        LONG delimiter_;
        std::vector<LONG> intArray_;;
    };

    class ObjectA : public IFabricJsonSerializable
    {
    public:
        ObjectA(int elements)
        {
            i_ = elements;
            b_ = true;
            s_ = "String";

            for (int i = 0; i < elements; i++)
            {
                intArray_.push_back(i);
            }
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("i", i_)
            SERIALIZABLE_PROPERTY("b", b_)
            SERIALIZABLE_PROPERTY("s", s_)
            SERIALIZABLE_PROPERTY("array", intArray_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        LONG i_;
        bool b_;
        std::string s_;
        std::vector<LONG> intArray_;;
    };

    class OutOfOrderObjectA : public IFabricJsonSerializable
    {
    public:
        OutOfOrderObjectA(int elements)
        {
            i_ = elements;
            b_ = true;
            s_ = "String";

            for (int i = 0; i < elements; i++)
            {
                intArray_.push_back(i);
            }
        }

        bool operator == (ObjectA const& objA) const
        {
            if (intArray_.size() != objA.intArray_.size())
            {
                return false;
            }

            for (int i = 0; i < intArray_.size(); i++)
            {
                if (intArray_[i] != objA.intArray_[i]) { return false; }
            }

            if ((s_ != objA.s_) ||
                (b_ != objA.b_) ||
                (i_ != objA.i_))
            {
                return false;
            }

            return true;
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("s", s_)
            SERIALIZABLE_PROPERTY("i", i_)
            SERIALIZABLE_PROPERTY("b", b_)
            SERIALIZABLE_PROPERTY("array", intArray_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        LONG i_;
        bool b_;
        std::string s_;
        std::vector<LONG> intArray_;;
    };

    class NestedObject : public IFabricJsonSerializable
    {
    public:
        NestedObject()
        :objArray1_(0)
        {};

        NestedObject(string name, int val)
            : obj1_(name, val)
            , obj2_(name + "1", val+1)
            , obj3_(name + "2", val+2)
            , objArray1_(val)
        {
        }

        bool operator == (NestedObject const& other) const
        {
            return ((obj1_ == other.obj1_) &&
                    (obj2_ == other.obj2_) &&
                    (obj3_ == other.obj3_) &&
                    (objArray1_ == other.objArray1_) &&
                    (i_ == other.i_));
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("Obj1", obj1_)
            SERIALIZABLE_PROPERTY("ObjArray1", objArray1_)
            SERIALIZABLE_PROPERTY("Obj2", obj2_)
            SERIALIZABLE_PROPERTY("Int", i_)
            SERIALIZABLE_PROPERTY("Obj3", obj3_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        TempObject obj1_;
        NestedObjectArray objArray1_;
        LONG i_;
        TempObject obj2_;
        TempObject obj3_;
    };

    class ObjectWithMap : public IFabricJsonSerializable
    {
    public:
        ObjectWithMap(int count)
        {
            i_ = count;
            for (int i = 0; i < count; i++)
            {
                std::string str;
                Common::StringUtility::GenerateRandomString(10, str);
                TempObject obj(str, i);
                NestedObject nestedObject(str, i);
                map1_.insert(make_pair(str, obj));
                map2_.insert(make_pair(i, nestedObject));
            }
        }

        bool operator == (ObjectWithMap const& obj) const
        {

            if ((i_ != obj.i_) ||
                (map1_.size() != obj.map1_.size()) ||
                (map2_.size() != obj.map2_.size()))
            {
                return false;
            }

            auto item1 = obj.map1_.begin();
            for (auto item = map1_.begin(); item != map1_.end(); ++item, ++item1)
            {
                if ((item->first != item1->first) || !(item->second == item1->second))
                {
                    return false;
                }
            }

            auto map2item1 = obj.map2_.begin();
            for (auto map2item = map2_.begin(); map2item != map2_.end(); ++map2item, ++map2item1)
            {
                if (!(map2item->first == map2item1->first) || !(map2item->second == map2item1->second))
                {
                    return false;
                }
            }

            return true;
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("i", i_)
            SERIALIZABLE_PROPERTY("Map", map1_)
            SERIALIZABLE_PROPERTY("Map2", map2_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        LONG i_;
        map<string, TempObject> map1_;
        map<LONG, NestedObject> map2_;
    };


    class ObjectWithoutSPtr : public IFabricJsonSerializable
    {
    public:
        ObjectWithoutSPtr() {}

        ObjectWithoutSPtr(string name)
        {
            name_ = name;
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("ObjectName", name_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        std::string name_;
    };

    class SimpleMapTestObject : public IFabricJsonSerializable
    {
        public:
            SimpleMapTestObject() {}

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("StartMarkerObject", startMarkerObject_)
            SERIALIZABLE_PROPERTY_SIMPLE_MAP("TestMap", map1_)
            SERIALIZABLE_PROPERTY("EndMarkerObject", endMarkerObject_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        ObjectWithoutSPtr startMarkerObject_;
        map<string, string> map1_;
        ObjectWithoutSPtr endMarkerObject_;
    };

    class SimpleMapTestObject2 : public IFabricJsonSerializable
    {
    public:
        SimpleMapTestObject2() {}

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY_SIMPLE_MAP("TestMap", map1_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        map<string, vector<string>> map1_;
    };

    class WStringMapTestObject : public IFabricJsonSerializable
    {
        public:
            WStringMapTestObject() {}

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY_SIMPLE_MAP("WStringMap", map1_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        map<string, string> map1_;
    };

    class ObjectWithConstProperty : public IFabricJsonSerializable
    {
    public:

        ObjectWithConstProperty(bool x) 
        {
            this->conditionalField = x;
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZE_CONSTANT_IF("ConditionalConstantProperty", "TheValueIsTrue", conditionalField)
            SERIALIZE_CONSTANT_IF("ConditionalConstantProperty", "TheValueIsFalse", !conditionalField)
        END_JSON_SERIALIZABLE_PROPERTIES()

    private:
        bool conditionalField;
    };

    class ObjectWithSPtr : public IFabricJsonSerializable
    {
    public:
        ObjectWithSPtr() {}

        ObjectWithSPtr(string name)
        {
            name_ = name;
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("ObjectName", name_)
            SERIALIZABLE_PROPERTY("SPtr", tempObjSPtr_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        std::string name_;
        std::shared_ptr<TempObject> tempObjSPtr_;
    };

    class ObjectWithSPtrArray : public Common::IFabricJsonSerializable
    {
    public:
        ObjectWithSPtrArray() {}
        ObjectWithSPtrArray(string name)
        {
            name_ = name;
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("ObjectName", name_)
            SERIALIZABLE_PROPERTY("SPtrArray", objectSPtrArray_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        string name_;
        vector<shared_ptr<ObjectBase>> objectSPtrArray_;
    };

    class TestObjectV1 : public Common::IFabricJsonSerializable
    {
        public:
            TestObjectV1() {}
            TestObjectV1(int i, int j)
            {
                i_ = i;
                j_ = j;
            }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("i", i_)
            SERIALIZABLE_PROPERTY("j", j_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        int i_;
        int j_;
    };

    class TestObjectV2 : public Common::IFabricJsonSerializable
    {
        public:
            TestObjectV2() {}
            TestObjectV2(int i, int j, string foo)
            {
                i_ = i;
                j_ = j;
                foo_ = foo;
                nestedObjectArray_.push_back(std::make_shared<NestedObjectArray>(1));
            }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("i", i_)
            SERIALIZABLE_PROPERTY("NestedObjectArray", nestedObjectArray_)
            SERIALIZABLE_PROPERTY("j", j_)
            SERIALIZABLE_PROPERTY("Foo", foo_)
            SERIALIZABLE_PROPERTY("Map", map_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        std::vector<std::shared_ptr<NestedObjectArray>> nestedObjectArray_;
        int i_;
        int j_;
        string foo_;
        map<int, string> map_;
    };

    class TestObjectInt64AsNum : public Common::IFabricJsonSerializable
    {
    public:
        TestObjectInt64AsNum() {}
        TestObjectInt64AsNum(unsigned __int64 v1, __int64 v2)
        {
            uint64_ = v1;
            int64_ = v2;
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY_INT64_AS_NUM_IF("UInt64", uint64_, true)
            SERIALIZABLE_PROPERTY_INT64_AS_NUM_IF("Int64", int64_, true)
        END_JSON_SERIALIZABLE_PROPERTIES()

        bool operator == (TestObjectInt64AsNum const& other) const
        {
            return int64_ == other.int64_ && uint64_ == other.uint64_;
        }

        unsigned __int64 uint64_;
        __int64 int64_;
    };
    
    class ObjectWithNums : public Common::IFabricJsonSerializable
    {
    public:
        ObjectWithNums() {}
        ObjectWithNums(byte i, int j, unsigned int k, LONG l, ULONG m, __int64 n, unsigned __int64 o)
        {
            byte_ = i;
            int_ = j;
            uint_ = k;
            long_ = l;
            ulong_ = m;
            int64_ = n;
            uint64_ = o;
        }

        BEGIN_JSON_SERIALIZABLE_PROPERTIES()
            SERIALIZABLE_PROPERTY("Byte", byte_)
            SERIALIZABLE_PROPERTY("Int", int_)
            SERIALIZABLE_PROPERTY("UInt", uint_)
            SERIALIZABLE_PROPERTY("LONG", long_)
            SERIALIZABLE_PROPERTY("ULONG", ulong_)
            SERIALIZABLE_PROPERTY("Int64", int64_)
            SERIALIZABLE_PROPERTY("UInt64", uint64_)
        END_JSON_SERIALIZABLE_PROPERTIES()

        bool operator == (ObjectWithNums const& other) const
        {
            return byte_ == other.byte_ &&
                int_ == other.int_ &&
                uint_ == other.uint_ &&
                long_ == other.long_ &&
                ulong_ == other.ulong_ &&
                int64_ == other.int64_ &&
                uint64_ == other.uint64_;
        }

        byte byte_;
        int int_;
        unsigned int uint_;
        LONG long_;
        ULONG ulong_;
        __int64 int64_;
        unsigned __int64 uint64_;
    };


    class JSonSerializationTest
    {
    };

    BOOST_FIXTURE_TEST_SUITE(JSonSerializationTestSuite,JSonSerializationTest)

    BOOST_AUTO_TEST_CASE(SimpleMapWithVectorTest)
    {
        SimpleMapTestObject2 test;
        test.map1_[L"random"].push_back("val1");
        test.map1_[L"random"].push_back("val2");
        test.map1_[L"random2"].push_back("val1");
        string jsonString;

        auto error = JsonHelper::Serialize(test, jsonString);
        VERIFY_IS_TRUE(error.IsSuccess());

        SimpleMapTestObject2 test2;
        error = JsonHelper::Deserialize(test2, jsonString);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(test.map1_ == test2.map1_);
    }

    BOOST_AUTO_TEST_CASE(JsonTestNull)
    {
        TestObjectV2 test;
        test.j_ = 2;
        string jsonInput = "{\"NestedObjectArray\":null,\"i\":3,\"j\":null,\"Foo\":null,\"random\":null}";

        auto error = JsonHelper::Deserialize(test, jsonInput);

        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(test.foo_ == "");
        VERIFY_IS_TRUE(test.nestedObjectArray_.size() == 0);
        VERIFY_IS_TRUE(test.i_ == 3);
        VERIFY_IS_TRUE(test.j_ == 2);

        ObjectWithSPtr test2;
        jsonInput = "{\"ObjectName\":\"name\", \"SPtr\":{\"Kind\":null, \"ObjectName\":null, \"ObjectValue\":\"value\"}}";

        error = JsonHelper::Deserialize(test2, jsonInput);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(test2.name_ == "name");
        VERIFY_IS_TRUE(test2.tempObjSPtr_->name_ == "");
        VERIFY_IS_TRUE(test2.tempObjSPtr_->val_ == 0);
        VERIFY_IS_TRUE(test2.tempObjSPtr_->kind_ == ObjectKind::OBJECT_KIND_1);
    }


    BOOST_AUTO_TEST_CASE(OutofOrderFieldsTest)
    {
        ObjectA objA(3);
        OutOfOrderObjectA oobjA(0);

        //
        // Verify successful Serialization.
        //
        string data;
        auto error = JsonHelper::Serialize(objA, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        //
        // Verify successful DeSerialization.
        //
        error = JsonHelper::Deserialize(oobjA, data);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(oobjA == objA);
    }

    BOOST_AUTO_TEST_CASE(NestedObjectTest)
    {
        NestedObject obj1("NestedObject", 1);
        string data;

        //
        // Verify successful Serialization.
        //
        auto error = JsonHelper::Serialize(obj1, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        //
        // Verify successful DeSerialization.
        //
        NestedObject obj2;
        error = JsonHelper::Deserialize(obj2, data);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(obj1 == obj2);
    }

    BOOST_AUTO_TEST_CASE(MapTest)
    {
        ObjectWithMap obj1(2);

        //
        // Verify successful Serialization.
        //
        string data;
        auto error = JsonHelper::Serialize(obj1, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        ObjectWithMap obj2(0);

        //
        // Verify successful DeSerialization.
        //
        error = JsonHelper::Deserialize(obj2, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        VERIFY_IS_TRUE(obj1 == obj2);
    }

    BOOST_AUTO_TEST_CASE(NestedObjectArraySerializationTest)
    {
        NestedObjectArray objArray(2);
        string data;

        //
        // Verify successful Serialization.
        //
        auto error = JsonHelper::Serialize(objArray, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        //
        // Verify successful DeSerialization.
        //
        NestedObjectArray objArray1(0);
        error = JsonHelper::Deserialize(objArray1, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        VERIFY_IS_TRUE(objArray == objArray1);
    }

    BOOST_AUTO_TEST_CASE(NestedObjectArraySerializationFailureTest)
    {
        NestedObjectArray objArray(1);

        ComPointer<JsonWriter> jsonWriter = make_com<JsonWriter>();
        shared_ptr<ByteBuffer> bufferSPtr = make_shared<ByteBuffer>();

        JsonWriterVisitor writerVisitor(jsonWriter.GetRawPointer());

        //
        // Verify successful Serialization.
        //
        VERIFY_IS_TRUE(!FAILED(objArray.VisitSerializable(&writerVisitor, nullptr)));
        jsonWriter->GetBytes(*bufferSPtr.get());

        //
        // Trash the array and object end.
        //
        bufferSPtr->data()[bufferSPtr->size() - 1] = (BYTE) 0;
        bufferSPtr->data()[bufferSPtr->size() - 2] = (BYTE) 0;

        JsonReader::InitParams initParams((char*)(bufferSPtr->data()), (ULONG)bufferSPtr->size());
        ComPointer<JsonReader> jsonReader = make_com<JsonReader>(initParams);
        JsonReaderVisitor readerVisitor(jsonReader.GetRawPointer());
        NestedObjectArray objArray1(0);

        //
        // Verify Unsuccessful DeSerialization.
        //
        VERIFY_IS_TRUE(FAILED(readerVisitor.Deserialize(objArray1)));
    }

    BOOST_AUTO_TEST_CASE(SimpleArrayTest)
    {
        SimpleArray obj(2);
        string data;

        auto error = JsonHelper::Serialize(obj, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        SimpleArray objArray1(0);
        error = JsonHelper::Deserialize(objArray1, data);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(obj == objArray1);
    }

    BOOST_AUTO_TEST_CASE(EmptyVector)
    {
        NestedObjectArray objArray(0);
        string data;

        //
        // Verify successful Serialization.
        //
        auto error = JsonHelper::Serialize(objArray, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        NestedObjectArray objArray1(0);
        error = JsonHelper::Deserialize(objArray1, data);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(objArray1.nestedArray1_.size() == 0 && objArray1.nestedArray_.size() == 0);
    }

    BOOST_AUTO_TEST_CASE(WrongDataTest)
    {
        ObjectSimilarToNestedObjectArray obj(2);
        string data;

        //
        // Verify successful Serialization.
        //
        auto error = JsonHelper::Serialize(obj, data);

        //
        // Verify deserialization failure
        //
        NestedObjectArray objArray1(0);
        error = JsonHelper::Deserialize(objArray1, data);
        VERIFY_IS_TRUE(!error.IsSuccess());
    }

    
    BOOST_AUTO_TEST_CASE(SerializationOfObjectWithSPtr)
    {
        ObjectWithoutSPtr object1("objectOne");
        string data;
        auto error = JsonHelper::Serialize(object1, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        ObjectWithSPtr object2;
        error = JsonHelper::Deserialize(object2, data);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(object1.name_ == object2.name_);
        VERIFY_IS_TRUE(object2.tempObjSPtr_ == nullptr);

        string data1;
        error = JsonHelper::Serialize(object2, data1);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(data == data1);

        object2.tempObjSPtr_ = make_shared<TempObject>("object2", 1);

        error = JsonHelper::Serialize(object2, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        ObjectWithSPtr object3;
        error = JsonHelper::Deserialize(object3, data);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(*object2.tempObjSPtr_ == *object3.tempObjSPtr_);
    }

    BOOST_AUTO_TEST_CASE(DynamicActivationWithSPtr)
    {
        ObjectWithSPtrArray object("ObjectWithSPtrArray");

        object.objectSPtrArray_.push_back(make_shared<TempObject>("1.TempObject", 0xA5A5));
        object.objectSPtrArray_.push_back(make_shared<TempObject1>(false, 0xDEADBEEF));
        object.objectSPtrArray_.push_back(make_shared<TempObject>("2.TempObject", 0xF00D));

        string data;
        auto error = JsonHelper::Serialize(object, data, JsonSerializerFlags::EnumInStringFormat);
        VERIFY_IS_TRUE(error.IsSuccess());

        ObjectWithSPtrArray object1;
        error = JsonHelper::Deserialize(object1, data, JsonSerializerFlags::EnumInStringFormat);
        VERIFY_IS_TRUE(error.IsSuccess());

        string data1;
        error = JsonHelper::Serialize(object1, data1, JsonSerializerFlags::EnumInStringFormat);
        VERIFY_IS_TRUE(error.IsSuccess());

        VERIFY_IS_TRUE(data == data1);
    }

    BOOST_AUTO_TEST_CASE(DynamicActivationOfSPtr)
    {
        shared_ptr<TempObject> object = make_shared<TempObject>("1.TempObject", 0xA5A5);
        string data;
        auto error = JsonHelper::Serialize(*object, data, JsonSerializerFlags::EnumInStringFormat);
        VERIFY_IS_TRUE(error.IsSuccess());

        shared_ptr<ObjectBase> objectBase;
        error = JsonHelper::Deserialize(objectBase, data, JsonSerializerFlags::EnumInStringFormat);
        VERIFY_IS_TRUE(error.IsSuccess());

        shared_ptr<TempObject> tempObj = dynamic_pointer_cast<TempObject>(objectBase);
        VERIFY_IS_TRUE(tempObj != nullptr);

        string data1;
        error = JsonHelper::Serialize(*tempObj, data1, JsonSerializerFlags::EnumInStringFormat);
        VERIFY_IS_TRUE(error.IsSuccess());
        VERIFY_IS_TRUE(data1 == data);

        // Test if we are handling newlines correctly when parsing the object for dynamic activation.
        data = "{\"Kind\":\"OBJECT_KIND_3\",\"ObjectId\":\"SpBYuHd0Mi\",\"Array\": \r\n[\r\n      1,\r\n 2,\r\n 3\r\n]\r\n}";
        error = JsonHelper::Deserialize(objectBase, data, JsonSerializerFlags::EnumInStringFormat);
        VERIFY_IS_TRUE(error.IsSuccess());

        vector<int> numbers{1, 2, 3};
        TempObjectWithArray tmpObjWithArray("SpBYuHd0Mi", move(numbers));
        shared_ptr<TempObjectWithArray> tempObj1 = dynamic_pointer_cast<TempObjectWithArray>(objectBase);

        VERIFY_IS_TRUE(tmpObjWithArray == *tempObj1);
    }

    BOOST_AUTO_TEST_CASE(DynamicActivationWithSPtrRoot)
    {
        TempObject1 originalDerivedObject(false, 0xDEADBEEF);
        ObjectBase& baseRef = originalDerivedObject;

        string data;
        auto error = JsonHelper::Serialize(baseRef, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        TempObject1SPtr deserializedDerivedPtr;
        error = JsonHelper::Deserialize(deserializedDerivedPtr, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        VERIFY_IS_TRUE(originalDerivedObject == *deserializedDerivedPtr);
    }

    BOOST_AUTO_TEST_CASE(TestSerializeIntFromQuotedString)
    {
        // Serialize instance of "SimilarToTempObject" which will produce json string where json property "ObjectValue" is a quoted string.
        // Then deserialize that json to instance of "TempObject" which will convert proptery "ObjectValue" to type int.
        SimilarToTempObject obj1WithString("testObject1", "123");
        string jsonString;
        auto error = JsonHelper::Serialize(obj1WithString, jsonString);
        VERIFY_IS_TRUE(error.IsSuccess());

        // DeSerialization of json to instance of "TempObject"
        TempObject obj2WithInt;
        error = JsonHelper::Deserialize(obj2WithInt, jsonString);
        VERIFY_IS_TRUE(error.IsSuccess());

        // Compare field corresponding to json property "ObjectValue"
        // Assert that "123" was read as integer 123 was read correctly.
        VERIFY_ARE_EQUAL(123, obj2WithInt.val_);

        // Compare all other fields.
        VERIFY_ARE_EQUAL(obj1WithString.name_, obj2WithInt.name_);
        VERIFY_ARE_EQUAL(obj1WithString.id_, obj2WithInt.id_);
        VERIFY_ARE_EQUAL(obj1WithString.kind_, obj2WithInt.kind_);
    }

    BOOST_AUTO_TEST_CASE(CompatibilityTest)
    {
        TestObjectV1 object1(1, 2);
        string jsonString;
        auto error = JsonHelper::Serialize(object1, jsonString);
        VERIFY_IS_TRUE(error.IsSuccess());

        //
        // This tests if the new version of the object can deserialize
        // the old version of the object
        //
        TestObjectV2 object2;
        error = JsonHelper::Deserialize(object2, jsonString);
        VERIFY_IS_TRUE(error.IsSuccess());

        TestObjectV2 object3(2, 3, "Foo");
        error = JsonHelper::Serialize(object3, jsonString);
        VERIFY_IS_TRUE(error.IsSuccess());

        //
        // This tests if the old version of the object can deserialize
        // the new version of the object.
        //
        error = JsonHelper::Deserialize(object1, jsonString);
        VERIFY_IS_TRUE(error.IsSuccess());

        VERIFY_IS_TRUE(object1.i_ == object3.i_);
        VERIFY_IS_TRUE(object1.j_ == object3.j_);

        //
        // This is just a sanity test for the V2 object serialization.
        //
        TestObjectV2 object4;
        error = JsonHelper::Deserialize(object4, jsonString);
        VERIFY_IS_TRUE(error.IsSuccess());

        string jsonString1;
        error = JsonHelper::Serialize(object4, jsonString1);
        VERIFY_IS_TRUE(error.IsSuccess());

        VERIFY_IS_TRUE(jsonString == jsonString1);
    }

    BOOST_AUTO_TEST_CASE(SimpleMapTest)
    {
        SimpleMapTestObject object;

        string jsonInput = "{\"StartMarkerObject\":{\"ObjectName\":\"MyTest\"},\"TestMap\":{\"MyKey1\":\"MyValue1\",\"MyKey2\":\"MyValue2\",\"MyKey3\":\"MyValue3\"},\"EndMarkerObject\":{\"ObjectName\":\"MyTest\"}}";

        auto error = JsonHelper::Deserialize(object, jsonInput);
        VERIFY_IS_TRUE(error.IsSuccess());

        VERIFY_IS_TRUE(object.startMarkerObject_.name_ == "MyTest");
        VERIFY_IS_TRUE(object.endMarkerObject_.name_ == "MyTest");
        VERIFY_IS_TRUE(object.map1_.size() == 3);
        VERIFY_IS_TRUE(object.map1_[L"MyKey1"] == "MyValue1");
        VERIFY_IS_TRUE(object.map1_[L"MyKey2"] == "MyValue2");
        VERIFY_IS_TRUE(object.map1_[L"MyKey3"] == "MyValue3");

        string jsonOutput;
        error = JsonHelper::Serialize(object, jsonOutput);
        VERIFY_IS_TRUE(error.IsSuccess());

        Trace.WriteInfo("SimpleMapTest", "input={0} output={1}", jsonInput, jsonOutput);

        VERIFY_IS_TRUE(jsonInput == jsonOutput);
    }

    BOOST_AUTO_TEST_CASE(WStringMapTest)
    {
        WStringMapTestObject object;

        string jsonInput = "{\"WStringMap\":{\"MyKey1\":\"MyValue1\",\"MyKey2\":\"MyValue2\",\"MyKey3\":\"MyValue3\"}}";

        auto error = JsonHelper::Deserialize(object, jsonInput);
        VERIFY_IS_TRUE(error.IsSuccess());

        VERIFY_IS_TRUE(object.map1_.size() == 3);
        VERIFY_IS_TRUE(object.map1_[L"MyKey1"] == "MyValue1");
        VERIFY_IS_TRUE(object.map1_[L"MyKey2"] == "MyValue2");
        VERIFY_IS_TRUE(object.map1_[L"MyKey3"] == "MyValue3");

        string jsonOutput;
        error = JsonHelper::Serialize(object, jsonOutput);
        VERIFY_IS_TRUE(error.IsSuccess());

        Trace.WriteInfo("WStringMapTest", "input={0} output={1}", jsonInput, jsonOutput);

        VERIFY_IS_TRUE(jsonInput == jsonOutput);
    }

    BOOST_AUTO_TEST_CASE(EnumToStringTest)
    {
        string jsonOutput;
        TempObject object("TempObject", 10);
        auto error = JsonHelper::Serialize(object, jsonOutput, JsonSerializerFlags::EnumInStringFormat);
        VERIFY_IS_TRUE(error.IsSuccess());
        Trace.WriteInfo("EnumToStringTest", "output = {0}", jsonOutput);
        wprintf("%s", jsonOutput.c_str());

        TempObject object1;
        error = JsonHelper::Deserialize(object1, jsonOutput, JsonSerializerFlags::EnumInStringFormat);
        VERIFY_IS_TRUE(object == object1);
    }

    BOOST_AUTO_TEST_CASE(ConditionalConstantStringTest)
    {
        {
            string jsonOutput;
            ObjectWithConstProperty object(false);
            auto error = JsonHelper::Serialize(
                object,
                jsonOutput);

            VERIFY_IS_TRUE(error.IsSuccess());

            Trace.WriteInfo("ConditionalConstantStringTest(false)", "output = {0}", jsonOutput);
            VERIFY_IS_TRUE(StringUtility::Contains(jsonOutput, string("{\"ConditionalConstantProperty\":\"TheValueIsFalse\"}")));
        }
        {
            string jsonOutput;
            ObjectWithConstProperty object(true);
            auto error = JsonHelper::Serialize(
                object,
                jsonOutput);

            VERIFY_IS_TRUE(error.IsSuccess());

            Trace.WriteInfo("ConditionalConstantStringTest(true)", "output = {0}", jsonOutput);
            VERIFY_IS_TRUE(StringUtility::Contains(jsonOutput, string("{\"ConditionalConstantProperty\":\"TheValueIsTrue\"}")));
        }
    }

    BOOST_AUTO_TEST_CASE(Int64AsNumTest)
    {
        {
            string jsonOutput;
            unsigned __int64 value1 = 5000000000;
            __int64 value2 = 5000000000;

            TestObjectInt64AsNum object(value1, value2);

            auto error = JsonHelper::Serialize(
                object,
                jsonOutput);
            VERIFY_IS_TRUE(error.IsSuccess());

            Trace.WriteInfo("Int64AsNumTest", "output = {0}", jsonOutput);
            VERIFY_IS_TRUE(StringUtility::Contains(jsonOutput, string("{\"UInt64\":5000000000,\"Int64\":5000000000}")));

            TestObjectInt64AsNum object1;
            error = JsonHelper::Deserialize(object1, jsonOutput);
            Trace.WriteInfo("Int64AsNumTest", "deserialize = {0},{1}", object1.int64_, object1.uint64_);
            VERIFY_IS_TRUE(object == object1);
        }
        {
            string jsonOutput;
            unsigned __int64 value1 = 10;
            __int64 value2 = 20;

            TestObjectInt64AsNum object(value1, value2);

            auto error = JsonHelper::Serialize(
                object,
                jsonOutput);
            VERIFY_IS_TRUE(error.IsSuccess());

            Trace.WriteInfo("Int64AsNumTest", "output = {0}", jsonOutput);
            VERIFY_IS_TRUE(StringUtility::Contains(jsonOutput, string("{\"UInt64\":10,\"Int64\":20}")));

            TestObjectInt64AsNum object1;
            error = JsonHelper::Deserialize(object1, jsonOutput);
            Trace.WriteInfo("Int64AsNumTest", "deserialize = {0},{1}", object1.int64_, object1.uint64_);
            VERIFY_IS_TRUE(object == object1);
        }
        {
            string jsonOutput;
            unsigned __int64 value1 = 18446744073709551615;
            __int64 value2 = -9223372036854775807 - 1;

            TestObjectInt64AsNum object(value1, value2);

            auto error = JsonHelper::Serialize(
                object,
                jsonOutput);
            VERIFY_IS_TRUE(error.IsSuccess());

            Trace.WriteInfo("Int64AsNumTest", "output = {0}", jsonOutput);
            VERIFY_IS_TRUE(StringUtility::Contains(jsonOutput, string("{\"UInt64\":18446744073709551615,\"Int64\":-9223372036854775808}")));
        }
        {
            string jsonOutput;
            unsigned __int64 value1 = 18446744073709551615;
            __int64 value2 = 9223372036854775807;

            TestObjectInt64AsNum object(value1, value2);

            auto error = JsonHelper::Serialize(
                object,
                jsonOutput);
            VERIFY_IS_TRUE(error.IsSuccess());

            Trace.WriteInfo("Int64AsNumTest", "output = {0}", jsonOutput);
            VERIFY_IS_TRUE(StringUtility::Contains(jsonOutput, string("{\"UInt64\":18446744073709551615,\"Int64\":9223372036854775807}")));
        }
        {
            string jsonOutput;
            unsigned __int64 value1 = 10;
            __int64 value2 = -20;

            TestObjectInt64AsNum object(value1, value2);

            auto error = JsonHelper::Serialize(
                object,
                jsonOutput);
            VERIFY_IS_TRUE(error.IsSuccess());

            Trace.WriteInfo("Int64AsNumTest", "output = {0}", jsonOutput);
            VERIFY_IS_TRUE(StringUtility::Contains(jsonOutput, string("{\"UInt64\":10,\"Int64\":-20}")));

            TestObjectInt64AsNum object1;
            error = JsonHelper::Deserialize(object1, jsonOutput);
            Trace.WriteInfo("Int64AsNumTest", "deserialize = {0},{1}", object1.int64_, object1.uint64_);
            VERIFY_IS_TRUE(object == object1);
        }
    }
    
    BOOST_AUTO_TEST_CASE(NumValuesLowerBoundsTest)
    {
        {
            // All valid values
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 1,"
                "\"UInt\": 1,"
                "\"LONG\": 1,"
                "\"ULONG\": 1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            ObjectWithNums expected(1, 1, 1, 1, 1, 1, 1);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_TRUE(error.IsSuccess());
            VERIFY_IS_TRUE(object == expected);
        }
        {
            // Byte out-of-bounds
            string jsonInput = "{"
                "\"Byte\": -1,"
                "\"Int\": 1,"
                "\"UInt\": 1,"
                "\"LONG\": 1,"
                "\"ULONG\": 1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }
        {
            // Int out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": –2147483649,"
                "\"UInt\": 1,"
                "\"LONG\": 1,"
                "\"ULONG\": 1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }
        {
            // UInt out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 1,"
                "\"UInt\": -1,"
                "\"LONG\": 1,"
                "\"ULONG\": 1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }
        {
            // LONG out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 1,"
                "\"UInt\": 1,"
                "\"LONG\": –2147483649,"
                "\"ULONG\": 1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }
        {
            // ULONG out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 1,"
                "\"UInt\": 1,"
                "\"LONG\": 1,"
                "\"ULONG\": -1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }
        {
            // Int64 out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 1,"
                "\"UInt\": 1,"
                "\"LONG\": 1,"
                "\"ULONG\": 1,"
#if !defined(PLATFORM_UNIX)
                "\"Int64\": \"-9223372036854775809\","
#else
                // due to floating point (double) comparison precision on Linux,
                // use a number with a greater difference from the min
                "\"Int64\": \"-9223372036854785809\","
#endif
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }
        {
            // UInt64 out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 1,"
                "\"UInt\": 1,"
                "\"LONG\": 1,"
                "\"ULONG\": 1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"-1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }

        {
            // Int64 lower bound test.
            ObjectWithNums object(0, 0, 0, 0, 0, 0x8000000000000000, 0);
            string data;
            auto error = JsonHelper::Serialize(object, data);
            VERIFY_IS_TRUE(error.IsSuccess());

            ObjectWithNums object1;
            error = JsonHelper::Deserialize(object1, data);
            VERIFY_IS_TRUE(error.IsSuccess());
            VERIFY_IS_TRUE(object == object1);  
        }
    }
    
    BOOST_AUTO_TEST_CASE(NumValuesUpperBoundsTest)
    {
        {
            // All valid values
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 1,"
                "\"UInt\": 1,"
                "\"LONG\": 1,"
                "\"ULONG\": 1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            ObjectWithNums expected(1, 1, 1, 1, 1, 1, 1);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_TRUE(error.IsSuccess());
            VERIFY_IS_TRUE(object == expected);
        }
        {
            // Byte out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 256,"
                "\"Int\": 1,"
                "\"UInt\": 1,"
                "\"LONG\": 1,"
                "\"ULONG\": 1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }
        {
            // Int out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 2147483648,"
                "\"UInt\": 1,"
                "\"LONG\": 1,"
                "\"ULONG\": 1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }
        {
            // UInt out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 1,"
                "\"UInt\": 4294967296,"
                "\"LONG\": 1,"
                "\"ULONG\": 1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }
        {
            // LONG out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 1,"
                "\"UInt\": 1,"
                "\"LONG\": 2147483648,"
                "\"ULONG\": 1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }
        {
            // ULONG out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 1,"
                "\"UInt\": 1,"
                "\"LONG\": 1,"
                "\"ULONG\": 4294967296,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }
        {
            // Int64 out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 1,"
                "\"UInt\": 1,"
                "\"LONG\": 1,"
                "\"ULONG\": 1,"
#if !defined(PLATFORM_UNIX)
                "\"Int64\": \"9223372036854775808\","
#else
                // due to floating point (double) comparison precision on Linux,
                // use a number with a greater difference from the max
                "\"Int64\": \"9223372036854785808\","
#endif
                "\"UInt64\": \"1\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }

        {
            // UInt64 out-of-bounds
            string jsonInput = "{"
                "\"Byte\": 1,"
                "\"Int\": 1,"
                "\"UInt\": 1,"
                "\"LONG\": 1,"
                "\"ULONG\": 1,"
                "\"Int64\": \"1\","
                "\"UInt64\": \"18446744073709551616\"}";

            ObjectWithNums object(0, 0, 0, 0, 0, 0, 0);
            auto error = JsonHelper::Deserialize(object, jsonInput);
            VERIFY_IS_FALSE(error.IsSuccess());
        }

        {
            // Int64 upper bound test.
            ObjectWithNums object(0, 0, 0, 0, 0, 0x7FFFFFFFFFFFFFFF, 0);
            string data;
            auto error = JsonHelper::Serialize(object, data);
            VERIFY_IS_TRUE(error.IsSuccess());

            ObjectWithNums object1;
            error = JsonHelper::Deserialize(object1, data);
            VERIFY_IS_TRUE(error.IsSuccess());
            VERIFY_IS_TRUE(object == object1);  
        }

        {
            // UInt64 upper bound test.
            ObjectWithNums object {0, 0, 0, 0, 0, 0, 0xFFFFFFFFFFFFFFFF};
            string data;
            auto error = JsonHelper::Serialize(object, data);
            VERIFY_IS_TRUE(error.IsSuccess());
            
            ObjectWithNums object1;
            error = JsonHelper::Deserialize(object1, data);
            VERIFY_IS_TRUE(error.IsSuccess());
            VERIFY_IS_TRUE(object == object1);
        }
    }

   
    BOOST_AUTO_TEST_CASE(VectorOfObjects)
    {
        vector<TempObject> vectorObject;
        vectorObject.push_back(move(TempObject("TempObject", 10)));
        vectorObject.push_back(move(TempObject("TempObject1", 11)));

        ByteBufferUPtr data;
        auto error = JsonHelper::Serialize(vectorObject, data);
        VERIFY_IS_TRUE(error.IsSuccess());

        vector<TempObject> vectorObject1;
        error = JsonHelper::Deserialize(vectorObject1, data);
        VERIFY_IS_TRUE(error.IsSuccess());
    }

    BOOST_AUTO_TEST_SUITE_END()
}

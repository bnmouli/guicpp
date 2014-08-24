// Copyright 2014 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


// Test for functions in builder.h

#include "guicpp/internal/guicpp_builder.h"

#include <string>

#include "include/gtest/gtest.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/internal/guicpp_table.h"
#include "include/guicpp_test_helper.h"

namespace guicpp {
class Injector;

namespace internal {
class LocalContext;

}  // namespace internal
}  // namespace guicpp

using std::string;

using guicpp::Injector;
using guicpp::internal::LocalContext;
using guicpp::internal::TableEntry;
using guicpp::internal::TableEntryBase;
using guicpp::internal::TypeId;
using guicpp::internal::TypeIdProvider;
using guicpp::internal::TypeKey;
using guicpp::scoped_ptr;

template <typename T>
class TestBindTable: public TableEntry<T*> {
 public:
  TestBindTable() {}
  virtual ~TestBindTable() {}

  virtual T* Get(const Injector* injector,
                 const LocalContext* local_context) const {
    return new T();  // All out test classes here have default constructor
  };

  virtual typename TableEntryBase::BindType GetBindType() const {
    // We can return any value except INVALID_BIND
    return TableEntryBase::BIND_TO_CTOR;
  }
};

namespace test_namespace {
using guicpp_test::TestBaseClass;

// Compiler looks for GuicppGetDefaultEntry associate with the class in the
// class's namespace and in the global namespace. The following two classes
// implement these situations.

// GuicppGetDefaultEntry is in the same namespace as class.
class TestInjectInNamespace: public TestBaseClass {
 public:
  string GetClassName() const {
    return "TestInjectInNamespace";
  }
};

TestBindTable<TestInjectInNamespace> GuicppGetDefaultEntry(
    TypeKey<TestInjectInNamespace> typeKey,
    const TestInjectInNamespace* dummy1, GuicppEmptyGlobalClass dummy2) {
  return TestBindTable<TestInjectInNamespace>();
}

// GuicppGetDefaultEntry is in the global namespace.
class TestInjectInGlobal: public TestBaseClass {
 public:
  string GetClassName() const {
    return "TestInjectInGlobal";
  }
};

// The following two classes are copies of above two. These tests ensures that a
// call to GuicppGetDefaultEntry invokes correct implementation even in presence
// of similar functions for other classes.

// Copy of TestInjectInNamespace.
class TestInjectInNamespaceCopy: public TestBaseClass {
 public:
  string GetClassName() const {
    return "TestInjectInNamespaceCopy";
  }
};

// This won't conflict with GuicppGetDefaultEntry of
// TestInjectInNamespace as argument types are different.
// This will just overload the function name.

TestBindTable<TestInjectInNamespaceCopy> GuicppGetDefaultEntry(
    TypeKey<TestInjectInNamespaceCopy> typeKey,
    const TestInjectInNamespaceCopy* dummy1, GuicppEmptyGlobalClass dummy2) {
  return TestBindTable<TestInjectInNamespaceCopy>();
}

// Copy of TestInjectInGlobal.
class TestInjectInGlobalCopy: public TestBaseClass {
 public:
  string GetClassName() const {
    return "TestInjectInGlobalCopy";
  }
};

// The following are child classes. This is written to
// ensure GuicppGetDefaultEntry don't conflict even
// when classes are related in some way.

// Child class of TestInjectInNamespace.
class TestInjectInNamespaceChild: public TestInjectInNamespace {
 public:
  string GetClassName() const {
    return "TestInjectInNamespaceChild";
  }
};

// This won't conflict with GuicppGetDefaultEntry of
// TestInjectInNamespace as argument types are different and though
// TestInjectInNamespace and TestInjectInNamespaceChild are related,
// They can only serve as overloads of assContextMethodInvoker when
// this name is invoked without a specific template specialization.
TestBindTable<TestInjectInNamespaceChild> GuicppGetDefaultEntry(
    TypeKey<TestInjectInNamespaceChild> typeKey,
    const TestInjectInNamespaceChild* dummy1, GuicppEmptyGlobalClass dummy2) {
  return TestBindTable<TestInjectInNamespaceChild>();
}

// Child class of TestInjectInGlobal.
class TestInjectInGlobalChild: public TestInjectInGlobal {
 public:
  string GetClassName() const {
    return "TestInjectInGlobalChild";
  }
};

}  // namespace test_namespace

// defining GuicppGetDefaultEntry for TestInjectInGlobal in global namespace.
TestBindTable<test_namespace::TestInjectInGlobal> GuicppGetDefaultEntry(
    TypeKey<test_namespace::TestInjectInGlobal> typeKey,
    const test_namespace::TestInjectInGlobal* dummy1,
    GuicppEmptyGlobalClass dummy2) {
  return TestBindTable<test_namespace::TestInjectInGlobal>();
}

// defining GuicppGetDefaultEntry for TestInjectInGlobalCopy.
TestBindTable<test_namespace::TestInjectInGlobalCopy> GuicppGetDefaultEntry(
    TypeKey<test_namespace::TestInjectInGlobalCopy> typeKey,
    const test_namespace::TestInjectInGlobalCopy* dummy1,
    GuicppEmptyGlobalClass dummy2) {
  return TestBindTable<test_namespace::TestInjectInGlobalCopy>();
}

// defining GuicppGetDefaultEntry for TestInjectInGlobalChild.
TestBindTable<test_namespace::TestInjectInGlobalChild> GuicppGetDefaultEntry(
    TypeKey<test_namespace::TestInjectInGlobalChild> typeKey,
    const test_namespace::TestInjectInGlobalChild* dummy1,
    GuicppEmptyGlobalClass dummy2) {
  return TestBindTable<test_namespace::TestInjectInGlobalChild>();
}

namespace guicpp_test {
using test_namespace::TestInjectInGlobal;
using test_namespace::TestInjectInGlobalChild;
using test_namespace::TestInjectInGlobalCopy;
using test_namespace::TestInjectInNamespace;
using test_namespace::TestInjectInNamespaceChild;
using test_namespace::TestInjectInNamespaceCopy;

template <typename T>
TypeId GetDefaultEntryTestHelper() {
  return GuicppGetDefaultEntry(
      TypeKey<T>(), static_cast<T*>(0), GuicppEmptyGlobalClass()).GetTypeId();
}

// Verifies that a call to GuicppGetDefaultEntry, invokes correct implementation
// regardless of whether it is defined in the class's namespace, or in the
// global namespace.
TEST(GuicppGetDefaultEntry, SelectsCorrectImplementation) {
  EXPECT_EQ(TypeIdProvider<TestInjectInNamespace>::GetTypeId(),
            GetDefaultEntryTestHelper<TestInjectInNamespace>());

  EXPECT_EQ(TypeIdProvider<TestInjectInGlobal>::GetTypeId(),
            GetDefaultEntryTestHelper<TestInjectInGlobal>());
}

// Verifies that a call to GuicppGetDefaultEntry, invokes correct implementation
// and does not confuse with similar function in same namespace.
TEST(GuicppGetDefaultEntry, NoConflictsWithCopy) {
  EXPECT_EQ(TypeIdProvider<TestInjectInNamespaceCopy>::GetTypeId(),
            GetDefaultEntryTestHelper<TestInjectInNamespaceCopy>());

  EXPECT_EQ(TypeIdProvider<TestInjectInGlobalCopy>::GetTypeId(),
            GetDefaultEntryTestHelper<TestInjectInGlobalCopy>());
}

// Verifies that a call to GuicppGetDefaultEntry, invokes correct implementation
// and does not confuse with function written for base classes.
TEST(GuicppGetDefaultEntry, NoConflictsWithChild) {
  EXPECT_EQ(TypeIdProvider<TestInjectInNamespaceChild>::GetTypeId(),
            GetDefaultEntryTestHelper<TestInjectInNamespaceChild>());

  EXPECT_EQ(TypeIdProvider<TestInjectInGlobalChild>::GetTypeId(),
            GetDefaultEntryTestHelper<TestInjectInGlobalChild>());
}

using guicpp::internal::MacrosHelper;

TEST(GuicppBuilderTest, BindToExternFp_CreatesByInjectingCtorArgs) {
  scoped_ptr<guicpp::Injector> injector(guicpp_test::GetEmptyInjector());

  MacrosHelper::BindToExternFp<TestSimpleInjectableClass>::Type entry1;

  scoped_ptr<TestSimpleInjectableClass> test1(entry1.Get(injector.get(), NULL));
  EXPECT_EQ("TestSimpleInjectableClass", test1->GetClassName());

  MacrosHelper::BindToExternFp<TestSimpleClassUser>::Type entry2;
  scoped_ptr<TestSimpleClassUser> test2(entry2.Get(injector.get(), NULL));
  EXPECT_EQ("TestSimpleClassUser", test2->GetClassName());
  EXPECT_EQ("TestSimpleInjectableClass",
            test2->simple_object()->GetClassName());
}

TEST(GuicppBuilderTest, BindToInlineFp_CreatesByUsingSpecifiedCtor) {
  scoped_ptr<guicpp::Injector> injector(guicpp_test::GetEmptyInjector());

  MacrosHelper::BindToInlineFp<
      TestSimpleClassUser,
      TestSimpleClassUser* (*)()>::Type entry1;
  scoped_ptr<TestSimpleClassUser> test1(entry1.Get(injector.get(), NULL));

  EXPECT_EQ("TestSimpleClassUser", test1->GetClassName());
  EXPECT_EQ(NULL, test1->simple_object());

  MacrosHelper::BindToInlineFp<
      TestSimpleClassUser,
      TestSimpleClassUser* (*)(TestSimpleInjectableClass*)>::Type entry2;
  scoped_ptr<TestSimpleClassUser> test2(entry2.Get(injector.get(), NULL));

  EXPECT_EQ("TestSimpleClassUser", test2->GetClassName());
  ASSERT_THAT(test2->simple_object(), testing::NotNull());
  EXPECT_EQ("TestSimpleInjectableClass",
            test2->simple_object()->GetClassName());
}

}  // namespace guicpp_test

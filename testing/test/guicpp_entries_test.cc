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


// Test for classes declared in entries.h.

#include "guicpp/internal/guicpp_entries.h"

#include <string>

#include "guicpp/guicpp_binder.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/internal/guicpp_local_context.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/internal/guicpp_util.h"
#include "include/gmock/gmock.h"
#include "include/gtest/gtest.h"
#include "include/guicpp_test_helper.h"

namespace guicpp {
namespace internal {
using guicpp_test::DeleteFirstArgument;
using guicpp_test::MockCleanupAction;
using guicpp_test::TestBaseClass;
using guicpp_test::TestClassWithDeleteMarker;
using guicpp_test::TestCleanupAction;
using guicpp_test::TestDeleteMarker;
using guicpp_test::TestProvider;
using guicpp_test::TestSimpleInjectableClass;
using guicpp_test::TestTopLevelClass;
using testing::_;
using testing::Expectation;
using testing::MockFunction;

// Tests for BindToTypeEntry.
TEST(BindToTypeEntryTest, Get_ReturnsNewInstanceOfBoundTypeOnEachCall) {
  BindToTypeEntry<
      TestBaseClass*, EmptyAnnotations, TestSimpleInjectableClass*> entry;

  EXPECT_EQ(TypeIdProvider<TestBaseClass>::GetTypeId(), entry.GetTypeId());
  EXPECT_EQ(TypesCategory::IS_POINTER, entry.GetCategory());
  EXPECT_EQ(TableEntryBase::BIND_TO_TYPE, entry.GetBindType());

  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());
  internal::LocalContext local_context;

  TestBaseClass* object1 = entry.Get(injector.get(), &local_context);

  // entry.Get() returns an instance of the bound sub-class.
  EXPECT_EQ("TestSimpleInjectableClass", object1->GetClassName());

  // Checking entry returns new object on each call.
  TestBaseClass* object2 = entry.Get(injector.get(), &local_context);
  EXPECT_NE(object1, object2);

  delete object1;
  if (object1 != object2) {
    delete object2;
  }
}

// Tests for PointerTableEntry
TEST(PointerTableEntryTest, ReturnsPointerTakenInCtorAndUsesCleanupAction) {
  TestSimpleInjectableClass object;

  MockFunction<void(string description)>  checkpoint;
  // This checkpoint is invoked just before deleting the entry.
  Expectation delete_entry = EXPECT_CALL(checkpoint, Call("delete entry"));

  MockCleanupAction<TestSimpleInjectableClass> mock_cleanup;
  TestCleanupAction<TestSimpleInjectableClass> test_action(&mock_cleanup);
  EXPECT_CALL(mock_cleanup, Cleanup(&object)).After(delete_entry);

  PointerTableEntry<
      TestSimpleInjectableClass*,
      TestCleanupAction<TestSimpleInjectableClass> >* entry =
          new PointerTableEntry<
              TestSimpleInjectableClass*,
              TestCleanupAction<TestSimpleInjectableClass> >(
                  &object, test_action);

  EXPECT_EQ(TypeIdProvider<TestSimpleInjectableClass>::GetTypeId(),
            entry->GetTypeId());
  EXPECT_EQ(TypesCategory::IS_POINTER, entry->GetCategory());
  EXPECT_EQ(TableEntryBase::BIND_TO_INSTANCE, entry->GetBindType());

  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());
  internal::LocalContext local_context;

  EXPECT_EQ(&object, entry->Get(injector.get(), &local_context));

  checkpoint.Call("delete entry");
  delete entry;  // Invokes TestCleanupAction with the object
}

// Tests for ValueTableEntry
TEST(ValueTableEntryTest, Get_ReturnsTheValueTakenInCtor) {
  ValueTableEntry<int> entry(100);

  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());
  internal::LocalContext local_context;

  EXPECT_EQ(TypeIdProvider<int>::GetTypeId(), entry.GetTypeId());
  EXPECT_EQ(TypesCategory::IS_VALUE, entry.GetCategory());
  EXPECT_EQ(TableEntryBase::BIND_TO_VALUE, entry.GetBindType());

  EXPECT_EQ(100, entry.Get(injector.get(), &local_context));
}

// Tests BindToProviderEntry.
TEST(BindToProviderEntryTest, Get_UsesProviderToGetAnInstance) {
  bool is_provider_called = false;
  bool is_provider_deleted = false;
  TestProvider* provider =
      new TestProvider(&is_provider_called, &is_provider_deleted);

  // Ownership of provider is taken by provider_entry.
  BindToProviderEntry<TestTopLevelClass*, TestProvider, DeletePointer>*
      provider_entry =
      new BindToProviderEntry<TestTopLevelClass*, TestProvider, DeletePointer>(
          provider, DeletePointer());

  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());
  internal::LocalContext local_context;

  scoped_ptr<TestTopLevelClass> top_class(
      provider_entry->Get(injector.get(), &local_context));

  EXPECT_TRUE(is_provider_called);
  EXPECT_FALSE(is_provider_deleted);

  EXPECT_EQ("TestSimpleInjectableClass",
            top_class->simple_object()->GetClassName());

  delete provider_entry;

  // Provider is owned by BindToProviderEntry hence it is deleted in
  // its destructor.
  EXPECT_TRUE(is_provider_deleted);
}

}  // namespace internal
}  // namespace guicpp

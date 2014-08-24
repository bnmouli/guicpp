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


// Tests for Binder.

#include "guicpp/guicpp_binder.h"

#include <string>

#include "include/gmock/gmock.h"
#include "include/gtest/gtest.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/guicpp.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/internal/guicpp_local_context.h"
#include "guicpp/guicpp_module.h"
#include "guicpp/guicpp_provider.h"
#include "guicpp/internal/guicpp_table.h"
#include "include/guicpp_test_helper.h"
#include "guicpp/internal/guicpp_types.h"
#include "guicpp/internal/guicpp_util.h"

namespace guicpp {
using guicpp_test::DeleteFirstArgument;
using guicpp_test::MockCleanupAction;
using guicpp_test::TestBaseClass;
using guicpp_test::TestClassWithDeleteMarker;
using guicpp_test::TestCleanupAction;
using guicpp_test::TestDeleteMarker;
using guicpp_test::TestInjectableSubClass;
using guicpp_test::TestLabelOne;
using guicpp_test::TestLabelTwo;
using guicpp_test::TestProvider;
using guicpp_test::TestSimpleInjectableClass;
using guicpp_test::TestTopLevelClass;
using guicpp_test::TestValueProvider;
using testing::_;
using testing::Eq;
using testing::Expectation;
using testing::MockFunction;

class GuicppBinderTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    // Injector takes ownership of BindTable.
    injector_.reset(new Injector(new internal::BindTable()));

    // Binder does not take ownership of bind_table_
    binder_.reset(new Binder(injector_->bind_table_.get()));

    local_context_.reset(new internal::LocalContext());
  }

  // A test utility that gets an instance of T from TableEntryBase. This checks
  // if base_entry corresponds to right type before reading it using
  // TableEntryReader, this ensures that any error will result in test fail and
  // process itself will continue.
  //
  //    * object: Pointer to the object to be instantiated.
  //    * expected_bind_type: Expected bind type.
  //    * base_entry: Table entry base.
  template <typename T>
  inline void TestSafeGetInstance(
      internal::TableEntryBase::BindType expected_bind_type,
      typename internal::AtUtil::GetTypes<T>::ActualType* object) {
    typedef typename internal::AtUtil::GetTypes<T>::Annotations Annotations;
    typedef typename internal::AtUtil::GetTypes<T>::ActualType ActualType;

    EXPECT_EQ(0, binder_->num_errors());

    internal::TypeId bindId =
        internal::NormalInjectHandler<Annotations, ActualType>()
        .GetBindId(internal::TypeKey<internal::NormalInject>());

    const internal::TableEntryBase* base_entry =
        injector_->bind_table_->FindEntry(bindId);
    EXPECT_EQ(expected_bind_type, base_entry->GetBindType());

    internal::TypeId expected_tid = internal::TypeIdProvider<
        typename internal::TypeInfo<ActualType>::TypeSpecifier>::GetTypeId();

    // Check that this entry is for ActualType.
    //
    // It is not safe to go ahead with the rest of this test if the entry does
    // not correspond to ActualType.
    ASSERT_EQ(expected_tid, base_entry->GetTypeId());

    // If ActualType is pointer type base_entry should also correspond to
    // pointer type and vice-versa. It is not safe to go ahead if they are
    // not same.
    ASSERT_EQ(internal::TypeInfo<ActualType>::Category::value,
              base_entry->GetCategory());

    // In all our tests we look for exact match of IsConst.
    ASSERT_EQ(internal::TypeInfo<ActualType>::IsConst::value,
              base_entry->IsConst());

    *object = internal::TableEntryReader<ActualType>::Get(
        base_entry, injector_.get(), local_context_.get());
  }

  scoped_ptr<Injector> injector_;
  scoped_ptr<Binder> binder_;
  scoped_ptr<internal::LocalContext> local_context_;
};


TEST_F(GuicppBinderTest, Bind_BindsBaseClassToSubClass) {
  binder_->Bind<TestSimpleInjectableClass, TestInjectableSubClass>();

  TestSimpleInjectableClass* object1 = NULL;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<TestSimpleInjectableClass*>(
          internal::TableEntryBase::BIND_TO_TYPE, &object1));

  // entry->Get() returns an implementation of sub-class.
  EXPECT_EQ("TestInjectableSubClass", object1->GetClassName());

  // Checking entry returns new object on each call.
  TestSimpleInjectableClass* object2 = NULL;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<TestSimpleInjectableClass*>(
          internal::TableEntryBase::BIND_TO_TYPE, &object2));
  EXPECT_NE(object1, object2);

  delete object1;
  if (object1 != object2) {
    delete object2;
  }
}


TEST_F(GuicppBinderTest, Bind_BindsConstBaseClassToSubClass) {
  binder_->Bind<const TestSimpleInjectableClass, TestInjectableSubClass>();

  const TestSimpleInjectableClass* object1 = NULL;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<const TestSimpleInjectableClass*>(
          internal::TableEntryBase::BIND_TO_TYPE, &object1));

  EXPECT_EQ("TestInjectableSubClass", object1->GetClassName());
  delete object1;
}


TEST_F(GuicppBinderTest, Bind_BindsConstBaseClassToConstSubClass) {
  binder_->Bind<const TestSimpleInjectableClass,
      const TestInjectableSubClass>();

  const TestSimpleInjectableClass* object1 = NULL;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<const TestSimpleInjectableClass*>(
          internal::TableEntryBase::BIND_TO_TYPE, &object1));

  EXPECT_EQ("TestInjectableSubClass", object1->GetClassName());
  delete object1;
}


TEST_F(GuicppBinderTest, BindValueType_BindsValueTypeToOtherKey) {
  binder_->BindValueType<int, guicpp::At<TestLabelOne, int> >();
  binder_->BindToValue<guicpp::At<TestLabelOne, int> >(379009);

  int value1 = 0;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<int>(
          internal::TableEntryBase::BIND_TO_TYPE, &value1));

  EXPECT_EQ(379009, value1);
}

TEST_F(GuicppBinderTest, Bind_DistinguishesLabelledInterfaces) {
  binder_->Bind<TestBaseClass, TestSimpleInjectableClass>();
  binder_->Bind<At<TestLabelOne, TestBaseClass>, TestSimpleInjectableClass>();
  binder_->Bind<At<TestLabelTwo, TestBaseClass>, TestInjectableSubClass>();

  TestBaseClass* object = NULL;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<TestBaseClass*>(
          internal::TableEntryBase::BIND_TO_TYPE, &object));

  // TestBaseClass is bound to TestSimpleInjectableClass.
  EXPECT_EQ("TestSimpleInjectableClass", object->GetClassName());
  delete object;

  // Testing for entry with annotation TestLabelOne.
  TestBaseClass* object_a1 = NULL;
  ASSERT_NO_FATAL_FAILURE((
      TestSafeGetInstance<At<TestLabelOne, TestBaseClass*> >(
          internal::TableEntryBase::BIND_TO_TYPE, &object_a1)));

  // TestBaseClass annotated with TestLabelOne is bound to
  // TestSimpleInjectableClass.
  EXPECT_EQ("TestSimpleInjectableClass", object_a1->GetClassName());
  delete object_a1;

  // Testing for entry with annotation TestLabelTwo.
  TestBaseClass* object_a2 = NULL;
  ASSERT_NO_FATAL_FAILURE((
      TestSafeGetInstance<At<TestLabelTwo, TestBaseClass*> >(
          internal::TableEntryBase::BIND_TO_TYPE, &object_a2)));

  // TestBaseClass annotated with TestLabelOne is bound to
  // TestInjectableSubClass.
  EXPECT_EQ("TestInjectableSubClass", object_a2->GetClassName());
  delete object_a2;
}

TEST_F(GuicppBinderTest, BindToInstance_InvokesCleanupActionWhenDeleted) {
  TestClassWithDeleteMarker* object = new TestClassWithDeleteMarker();

  // This checkpoint is invoked after tests are complete
  // and just before deleting the injector.
  MockFunction<void(string description)>  checkpoint;
  Expectation begin_cleanup = EXPECT_CALL(checkpoint, Call("begin cleanup"));

  // object is deleted at the time of cleanup.
  TestDeleteMarker delete_marker;
  EXPECT_CALL(delete_marker, Call(object)).After(begin_cleanup);
  object->SetDeleteMarker(&delete_marker);

  binder_->BindToInstance<TestClassWithDeleteMarker>(object, DeletePointer());
  TestClassWithDeleteMarker* object1 = NULL;

  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<TestClassWithDeleteMarker*>(
          internal::TableEntryBase::BIND_TO_INSTANCE, &object1));
  EXPECT_EQ(object, object1);

  checkpoint.Call("begin cleanup");
  injector_.reset();  // Invokes cleanup action.
}

TEST_F(GuicppBinderTest, BindToInstance_BindsConstToNonConstInstance) {
  TestInjectableSubClass* object = new TestInjectableSubClass();
  binder_->BindToInstance<const TestBaseClass>(object, DeletePointer());

  const TestBaseClass* bound_object;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<const TestBaseClass*>(
      internal::TableEntryBase::BIND_TO_INSTANCE, &bound_object));

  EXPECT_EQ(object, bound_object);
}

TEST_F(GuicppBinderTest, BindToInstance_BindsConstToConstInstance) {
  const TestInjectableSubClass* object = new TestInjectableSubClass();
  binder_->BindToInstance<const TestBaseClass>(object, DeletePointer());

  const TestBaseClass* bound_object;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<const TestBaseClass*>(
      internal::TableEntryBase::BIND_TO_INSTANCE, &bound_object));

  EXPECT_EQ(object, bound_object);
}

TEST_F(GuicppBinderTest, BindToInstance_TypeCanBeAnnotated) {
  TestClassWithDeleteMarker* object = new TestClassWithDeleteMarker();

  MockFunction<void(string description)>  checkpoint;
  Expectation begin_cleanup = EXPECT_CALL(checkpoint, Call("begin cleanup"));

  TestDeleteMarker delete_marker;
  EXPECT_CALL(delete_marker, Call(object)).After(begin_cleanup);
  object->SetDeleteMarker(&delete_marker);

  binder_->BindToInstance<At<TestLabelOne, TestClassWithDeleteMarker> >(
      object, DeletePointer());

  TestClassWithDeleteMarker* object1 = NULL;
  ASSERT_NO_FATAL_FAILURE((
      TestSafeGetInstance<At<TestLabelOne, TestClassWithDeleteMarker*> >(
         internal::TableEntryBase::BIND_TO_INSTANCE, &object1)));
  EXPECT_EQ(object, object1);

  checkpoint.Call("begin cleanup");
  injector_.reset();
}

TEST_F(GuicppBinderTest, BindToInstance_ConstTypeCanBeAnnotated) {
  const TestInjectableSubClass* object = new TestInjectableSubClass();
  binder_->BindToInstance<At<TestLabelOne, const TestBaseClass> >(
      object, DeletePointer());

  const TestBaseClass* bound_object;
  ASSERT_NO_FATAL_FAILURE((
      TestSafeGetInstance<At<TestLabelOne, const TestBaseClass*> >(
          internal::TableEntryBase::BIND_TO_INSTANCE, &bound_object)));

  EXPECT_EQ(object, bound_object);
}

TEST_F(GuicppBinderTest, BindToValue_HoldsValueAndCanBeAnnotated) {
  binder_->BindToValue<int>(100);
  binder_->BindToValue<At<TestLabelOne, int> >(200);

  int value = 0;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<int>(
          internal::TableEntryBase::BIND_TO_VALUE, &value));

  EXPECT_EQ(100, value);

  int labeled_value = 0;
  ASSERT_NO_FATAL_FAILURE((TestSafeGetInstance<At<TestLabelOne, int> >(
          internal::TableEntryBase::BIND_TO_VALUE, &labeled_value)));

  EXPECT_EQ(200, labeled_value);
}

// Test for BindToProvider
// Binding to return type.
TEST_F(GuicppBinderTest, BindToProvider_UsesProviderToCreateObjects) {
  bool is_provider_called = false;
  bool is_provider_deleted = false;

  binder_->BindToProvider<TestTopLevelClass>(
      new TestProvider(&is_provider_called, &is_provider_deleted),
      DeletePointer());

  EXPECT_FALSE(is_provider_called);
  EXPECT_FALSE(is_provider_deleted);

  TestTopLevelClass* top_class = NULL;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<TestTopLevelClass*>(
          internal::TableEntryBase::BIND_TO_PROVIDER, &top_class));
  EXPECT_TRUE(is_provider_called);

  EXPECT_EQ("TestTopLevelSubClass", top_class->GetClassName());
  EXPECT_EQ("TestSimpleInjectableClass",
            top_class->simple_object()->GetClassName());

  EXPECT_EQ(top_class->simple_object(),
            top_class->simple_user()->simple_object());
  delete top_class;

  injector_.reset();  // This should delete the provider as well.
  EXPECT_TRUE(is_provider_deleted);
}

class TestConstPointerProvider:
  public guicpp::AbstractProvider<const TestSimpleInjectableClass* ()> {
 public:
  TestConstPointerProvider() {}
  ~TestConstPointerProvider() {}

  const TestSimpleInjectableClass* Get() {
    return new TestSimpleInjectableClass();
  }

 private:
  GUICPP_DISALLOW_COPY_AND_ASSIGN_(TestConstPointerProvider);
};

TEST_F(GuicppBinderTest, BindToProvider_CanBindConstObjects) {
  binder_->BindToProvider<const TestBaseClass>(new TestConstPointerProvider(),
                                               DeletePointer());
  const TestBaseClass* object;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<const TestBaseClass*>(
          internal::TableEntryBase::BIND_TO_PROVIDER, &object));

  EXPECT_EQ("TestSimpleInjectableClass", object->GetClassName());
  delete object;
}

// Test for BindToProvider
// Binding provider to a base-class of provider's return type.
TEST_F(GuicppBinderTest, BindToProvider_CanBeBoundToBaseClassOfReturnType) {
  bool is_provider_called = false;
  bool is_provider_deleted = false;

  binder_->BindToProvider<TestBaseClass>(
      new TestProvider(&is_provider_called, &is_provider_deleted),
      DeletePointer());

  EXPECT_FALSE(is_provider_called);
  EXPECT_FALSE(is_provider_deleted);

  TestBaseClass* top_class = NULL;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<TestBaseClass*>(
          internal::TableEntryBase::BIND_TO_PROVIDER, &top_class));

  EXPECT_TRUE(is_provider_called);
  EXPECT_EQ("TestTopLevelSubClass", top_class->GetClassName());

  delete top_class;

  injector_.reset();  // This should delete the provider as well.
  EXPECT_TRUE(is_provider_deleted);
}

// Test for BindValueToProvider.
TEST_F(GuicppBinderTest, BindValueToProvider_UsesProviderToGetTheValue) {
  bool is_provider_deleted = false;
  binder_->BindValueToProvider<int>(
      new TestValueProvider(&is_provider_deleted), DeletePointer());
  EXPECT_FALSE(is_provider_deleted);

  int value;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<int>(
          internal::TableEntryBase::BIND_TO_PROVIDER, &value));
  EXPECT_EQ(value, 0);

  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<int>(
          internal::TableEntryBase::BIND_TO_PROVIDER, &value));
  EXPECT_EQ(value, 1);

  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<int>(
          internal::TableEntryBase::BIND_TO_PROVIDER, &value));
  EXPECT_EQ(value, 2);

  injector_.reset();  // This should delete the provider as well.
  EXPECT_TRUE(is_provider_deleted);
}

namespace internal {

// Scope for testing.
class GuicppTestScope {
 public:
  // BindToScope expects a template method called ConfigureScope that must bind
  // the type T to an appropriate provider. But here, the test is limited to
  // testing whether BindToScope calls ConfigureScope with the right argument.
  // To simplify the test, we do BindToInstance instead of binding to provider.
  template <typename L, typename T>
  static void ConfigureScope(Binder* binder) {
    binder->BindToInstance<guicpp::At<L, T> >(new T(379009), DeletePointer());
  }
};

}  // namespace internal

TEST_F(GuicppBinderTest, BindToScope_IncludesBindingDoneInScopeConfigure) {
  binder_->BindToScope<TestSimpleInjectableClass, internal::GuicppTestScope>();
  binder_->BindToScope<guicpp::At<TestLabelOne, TestInjectableSubClass>,
      internal::GuicppTestScope>();

  TestSimpleInjectableClass* object1;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<TestSimpleInjectableClass*>(
      internal::TableEntryBase::BIND_TO_INSTANCE, &object1));

  EXPECT_EQ("TestSimpleInjectableClass", object1->GetClassName());
  EXPECT_EQ(379009, object1->value());

  TestInjectableSubClass* object2;
  ASSERT_NO_FATAL_FAILURE((
      TestSafeGetInstance<At<TestLabelOne, TestInjectableSubClass*> >(
          internal::TableEntryBase::BIND_TO_INSTANCE, &object2)));

  EXPECT_EQ("TestInjectableSubClass", object2->GetClassName());
  EXPECT_EQ(379009, object2->value());
}

class TestInstallModule: public Module {
 public:
  virtual void Configure(Binder* binder) const {
    binder->Bind<TestBaseClass, TestSimpleInjectableClass>();
  }
};

// Installing other module will include bindings specified in other module.
TEST_F(GuicppBinderTest, Install_IncludesBindingsFromOtherModule) {
  TestInstallModule module;
  binder_->Install(&module);

  TestBaseClass* object1 = NULL;
  ASSERT_NO_FATAL_FAILURE(TestSafeGetInstance<TestBaseClass*>(
      internal::TableEntryBase::BIND_TO_TYPE, &object1));

  // TestBaseClass is bound to TestSimpleInjectableClass in TestInstallModule.
  EXPECT_EQ("TestSimpleInjectableClass", object1->GetClassName());

  delete object1;
}

TEST_F(GuicppBinderTest, DoNothing_FunctorDoesNotDeleteThePointer) {
  TestClassWithDeleteMarker* object = new TestClassWithDeleteMarker();

  MockFunction<void(string description)>  checkpoint;
  Expectation begin_cleanup = EXPECT_CALL(checkpoint, Call("begin cleanup"));

  TestDeleteMarker delete_marker;
  EXPECT_CALL(delete_marker, Call(object)).After(begin_cleanup);
  object->SetDeleteMarker(&delete_marker);

  DoNothing()(object);

  checkpoint.Call("begin cleanup");
  delete object;
}

TEST_F(GuicppBinderTest, DeletePointer_FunctorDeletesThePointer) {
  TestClassWithDeleteMarker* object = new TestClassWithDeleteMarker();

  TestDeleteMarker delete_marker;
  EXPECT_CALL(delete_marker, Call(object));
  object->SetDeleteMarker(&delete_marker);

  DeletePointer()(object);
}

}  // namespace guicpp

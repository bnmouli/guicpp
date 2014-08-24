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


// Test for LazySingleton

#include "guicpp/guicpp_singleton.h"

#include <string>

#include "include/gmock/gmock.h"
#include "include/gtest/gtest.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_binder.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/guicpp_module.h"
#include "include/guicpp_test_helper.h"
#include "guicpp/guicpp_tools.h"

namespace guicpp {
using guicpp_test::TestBaseClass;
using guicpp_test::TestClassWithDeleteMarker;
using guicpp_test::TestDeleteMarker;
using testing::_;
using testing::InSequence;
using testing::MockFunction;

class TestUnexpectedCreation {
 public:
  TestUnexpectedCreation() {
    EXPECT_TRUE(false) << "Unexpected instance creation";
  }
};

GUICPP_INJECT_CTOR(TestUnexpectedCreation, ());
GUICPP_DEFINE(TestUnexpectedCreation);


TEST(GuicppSingletonTest, ObjectNotCreatedUntilItIsRequested) {
  internal::ScopeSetupContext context;
  internal::LazySingletonProvider<TestUnexpectedCreation> singleton(&context);

  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());
  context.Init(injector.get());

  context.Cleanup();
};

TEST(GuicppSingletonTest, ReturnsSameObjectEverytime) {
  internal::ScopeSetupContext context;
  internal::LazySingletonProvider<TestClassWithDeleteMarker>
      singleton(&context);

  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());
  context.Init(injector.get());

  TestClassWithDeleteMarker* object1 = singleton.Get();

  // Checking singleton returns the same object on each call.
  TestClassWithDeleteMarker* object2 = singleton.Get();
  EXPECT_EQ(object1, object2);

  // context deletes all singleton objects on Cleanup();
  context.Cleanup();
}

TEST(GuicppSingletonTest, DeletesCreatedObjectOnCleanup) {
  internal::ScopeSetupContext context;
  internal::LazySingletonProvider<TestClassWithDeleteMarker>
      singleton(&context);

  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());
  context.Init(injector.get());

  TestClassWithDeleteMarker* object1 = singleton.Get();

  // Setting delete expectations for object1.
  TestDeleteMarker delete_marker;
  EXPECT_CALL(delete_marker, Call(object1));
  object1->SetDeleteMarker(&delete_marker);

  // context deletes all singleton objects on Cleanup();
  context.Cleanup();
}

// Mock scope used to test ScopeSetupContext.
class MockScopeProvider: public internal::SetupInterface {
 public:
  MOCK_METHOD1(Init, void(const Injector* injector));
  MOCK_METHOD0(Cleanup, void());
};

TEST(GuicppSingletonTest, ScopeSetupContext_CallsInitInOrderOfAddition) {
  internal::ScopeSetupContext context;
  MockScopeProvider provider1;
  MockScopeProvider provider2;
  MockScopeProvider provider3;

  // provider.Init() should not be called before context.Init().
  EXPECT_CALL(provider1, Init(_)).Times(0);
  EXPECT_CALL(provider2, Init(_)).Times(0);
  EXPECT_CALL(provider3, Init(_)).Times(0);

  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());
  MockFunction<void(string description)>  checkpoint;
  {
    InSequence sequence;

    // This checkpoint is invoked just before calling context.Init().
    EXPECT_CALL(checkpoint, Call("call init"));

    // Init must be called in this sequence.
    EXPECT_CALL(provider1, Init(injector.get()));
    EXPECT_CALL(provider2, Init(injector.get()));
    EXPECT_CALL(provider3, Init(injector.get()));
  }

  context.AddToInitList(&provider1);
  context.AddToInitList(&provider2);
  context.AddToInitList(&provider3);

  checkpoint.Call("call init");
  context.Init(injector.get());
}


TEST(GuicppSingletonTest, ScopeSetupContext_CallsCleanupInReverseOrder) {
  internal::ScopeSetupContext context;
  MockScopeProvider provider1;
  MockScopeProvider provider2;
  MockScopeProvider provider3;

  EXPECT_CALL(provider1, Init(_));
  EXPECT_CALL(provider2, Init(_));
  EXPECT_CALL(provider3, Init(_));

  // provider.Cleanup() should not be called before context.Cleanup().
  EXPECT_CALL(provider1, Cleanup()).Times(0);
  EXPECT_CALL(provider2, Cleanup()).Times(0);
  EXPECT_CALL(provider3, Cleanup()).Times(0);

  MockFunction<void(string description)>  checkpoint;
  {
    InSequence sequence;

    // This checkpoint is invoked just before calling context.Cleanup().
    EXPECT_CALL(checkpoint, Call("call cleanup"));

    // Cleanup must be called in this sequence.
    EXPECT_CALL(provider1, Cleanup());
    EXPECT_CALL(provider3, Cleanup());
    EXPECT_CALL(provider2, Cleanup());
  }


  context.AddToInitList(&provider1);
  context.AddToInitList(&provider2);
  context.AddToInitList(&provider3);

  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());
  context.Init(injector.get());

  // Deliberately changing the order to ensure this is not effected
  // by the sequence in which they are added to init list.
  context.AddToCleanupList(&provider2);
  context.AddToCleanupList(&provider3);
  context.AddToCleanupList(&provider1);

  checkpoint.Call("call cleanup");
  context.Cleanup();
}


class TestLazySingletonUnrequestedModule: public Module {
 public:
  void Configure(Binder* binder) const {
    binder->BindToScope<TestUnexpectedCreation, LazySingleton>();
  }
};

// Tests for BindToScope

TEST(GuicppSingletonTest, LazySingletonObjectsAreNotCreatedUnlessRequested) {
  // This module binds TestUnexpectedCreation to LazySingleton.
  TestLazySingletonUnrequestedModule module;

  // Note: we must use guicpp::CreateInjector() for binding to
  // LazySingleton work.
  scoped_ptr<Injector> injector(guicpp::CreateInjector(&module));

  // We don't request for any instances of TestUnexpectedCreation.
  // Hence TestUnexpectedCreation should not be instantiated.
}


class TestLazySingletonModule: public Module {
 public:
  void Configure(Binder* binder) const {
    binder->BindToScope<TestClassWithDeleteMarker, LazySingleton>();
    binder->Bind<TestBaseClass, TestClassWithDeleteMarker>();
  }
};


TEST(GuicppSingletonTest, BindingToSingletonScopeWillReturnSameObject) {
  TestLazySingletonModule module;
  // Note: we must use guicpp::CreateInjector() for binding to
  // LazySingleton work.
  scoped_ptr<Injector> injector(guicpp::CreateInjector(&module));

  TestClassWithDeleteMarker* object1 =
      injector->Get<TestClassWithDeleteMarker*>();

  // No new instance is created, it will return the same object.
  TestClassWithDeleteMarker* object2 =
      injector->Get<TestClassWithDeleteMarker*>();
  EXPECT_EQ(object1, object2);

  // Will return the same object even if the request for object is
  // indirect. In this case Get() for TestBaseClass will result in
  // Get() of TestClassWithDeleteMarker.
  TestBaseClass* object3 = injector->Get<TestBaseClass*>();
  EXPECT_EQ(object1, object3);
}

TEST(GuicppSingletonTest, SingletonObjectsAreDeletedWithInjector) {
  TestLazySingletonModule module;
  // Note: we must use guicpp::CreateInjector() for binding to
  // LazySingleton work.
  scoped_ptr<Injector> injector(guicpp::CreateInjector(&module));

  TestClassWithDeleteMarker* object =
      injector->Get<TestClassWithDeleteMarker*>();

  // Setting delete expectations for object.
  TestDeleteMarker delete_marker;
  EXPECT_CALL(delete_marker, Call(object));
  object->SetDeleteMarker(&delete_marker);

  injector.reset();
}


TEST(GuicppSingletonTest, ConstAndNonConstSingletonReturnSameInstance) {
  TestLazySingletonModule module;
  // Note: we must use guicpp::CreateInjector() for binding to
  // LazySingleton work.
  scoped_ptr<Injector> injector(guicpp::CreateInjector(&module));

  TestClassWithDeleteMarker* object =
      injector->Get<TestClassWithDeleteMarker*>();

  const TestClassWithDeleteMarker* const_object =
      injector->Get<const TestClassWithDeleteMarker*>();

  // Request for const and non-const returns the same instance.
  EXPECT_EQ(object, const_object);

  // Setting delete expectations for object.
  TestDeleteMarker delete_marker;
  EXPECT_CALL(delete_marker, Call(object));
  object->SetDeleteMarker(&delete_marker);

  injector.reset();
}


// Binds const pointer to singleton
class TestConstLazySingletonModule: public Module {
 public:
  void Configure(Binder* binder) const {
    binder->BindToScope<const TestClassWithDeleteMarker, LazySingleton>();
    binder->Bind<const TestBaseClass, const TestClassWithDeleteMarker>();
  }
};


TEST(GuicppSingletonTest, BindingToSingleton_CanBindConstTypes) {
  TestConstLazySingletonModule module;
  // Note: we must use guicpp::CreateInjector() for binding to
  // LazySingleton work.
  scoped_ptr<Injector> injector(guicpp::CreateInjector(&module));

  const TestClassWithDeleteMarker* object1 =
      injector->Get<const TestClassWithDeleteMarker*>();

  const TestBaseClass* object2 = injector->Get<const TestBaseClass*>();
  EXPECT_EQ(object1, object2);
}

}  // namespace guicpp

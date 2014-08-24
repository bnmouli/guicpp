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


// This file declares a few classes used in many Guic++ tests.

#ifndef GUICPP_TEST_HELPER_H_
#define GUICPP_TEST_HELPER_H_

#include <string>

#include "include/gmock/gmock.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_annotations.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/guicpp_factory.h"
#include "guicpp/guicpp_macros.h"
#include "guicpp/guicpp_provider.h"
#include "guicpp/internal/guicpp_table.h"

namespace guicpp {
using std::string;
class Injector;

namespace internal {
class LocalContext;
}  // namespace internal
}  // namespace guicpp

namespace guicpp_test {
using std::string;

// Test labels.
class TestLabelOne: public guicpp::Label {};
class TestLabelTwo: public guicpp::Label {};

// This class is meant to be subclassed.
class TestBaseClass {
 public:
  explicit TestBaseClass(int val = 0): value_(val) {}
  virtual ~TestBaseClass() {}

  // Returns the actual class name.
  // All subclasses override this method and return their class name.
  // This is used by tests to verify actual type of the object.
  virtual string GetClassName() const = 0;

  int value() const {
    return value_;
  }

 public:
  int value_;
};

GUICPP_INJECTABLE(TestBaseClass);

// Simple injectable class.
class TestSimpleInjectableClass: public TestBaseClass {
 public:
  TestSimpleInjectableClass(): TestBaseClass(0) {}
  explicit TestSimpleInjectableClass(int val): TestBaseClass(val) {}

  virtual string GetClassName() const {
    return "TestSimpleInjectableClass";
  }
};

GUICPP_INJECT_CTOR(TestSimpleInjectableClass, ());

// Injectable sub-class.
class TestInjectableSubClass: public TestSimpleInjectableClass {
 public:
  TestInjectableSubClass(): TestSimpleInjectableClass(0) {}
  explicit TestInjectableSubClass(int val): TestSimpleInjectableClass(val) {}

  virtual string GetClassName() const {
    return "TestInjectableSubClass";
  }
};

GUICPP_INJECT_CTOR(TestInjectableSubClass, ());

// User of simple class. This takes TestInjectableSubClass as constructor
// argument.
class TestSimpleClassUser {
 public:
  explicit TestSimpleClassUser(TestSimpleInjectableClass* simple_object)
      : simple_object_(simple_object) {}

  TestSimpleClassUser(): simple_object_(NULL) {}

  string GetClassName() const {
    return "TestSimpleClassUser";
  }

  TestSimpleInjectableClass* simple_object() const {
    return simple_object_.get();
  }

 private:
  scoped_ptr<TestSimpleInjectableClass> simple_object_;
};

GUICPP_INJECT_CTOR(TestSimpleClassUser, (
    TestSimpleInjectableClass* simple_object));

// Similar to above but expects TestSimpleInjectableClass to be a factory
// argument.
class TestSimpleAssistedArgumentUser {
 public:
  explicit TestSimpleAssistedArgumentUser(
      TestSimpleInjectableClass* simple_object)
      : simple_object_(simple_object) {}

  string GetClassName() const {
    return "TestSimpleAssistedArgumentUser";
  }

  TestSimpleInjectableClass* simple_object() const {
    return simple_object_.get();
  }

 private:
  scoped_ptr<TestSimpleInjectableClass> simple_object_;
};

GUICPP_INJECT_CTOR(TestSimpleAssistedArgumentUser, (
    guicpp::At<guicpp::Assisted, TestSimpleInjectableClass*> simple_object));


// Test top level classes.
class TestTopLevelClass: public TestBaseClass {
 public:
  TestTopLevelClass(TestSimpleAssistedArgumentUser* simple_user,
                    TestSimpleInjectableClass* simple_object)
      : TestBaseClass(0), simple_user_(simple_user),
        simple_object_(simple_object) {}

  string GetClassName() const {
    return "TestTopLevelClass";
  }

  TestSimpleAssistedArgumentUser* simple_user() const {
    return simple_user_.get();
  }

  TestSimpleInjectableClass* simple_object() const {
    return simple_object_;
  }

 private:
  scoped_ptr<TestSimpleAssistedArgumentUser> simple_user_;
  TestSimpleInjectableClass* simple_object_;
};

GUICPP_INJECT_CTOR(TestTopLevelClass, (
    TestSimpleAssistedArgumentUser* simple_user,
    guicpp::At<guicpp::Assisted, TestSimpleInjectableClass*> simple_object));

class TestTopLevelSubClass: public TestTopLevelClass {
 public:
  TestTopLevelSubClass(TestSimpleAssistedArgumentUser* simple_user,
                       TestSimpleInjectableClass* simple_object)
      : TestTopLevelClass(simple_user, simple_object) {}

  string GetClassName() const {
    return "TestTopLevelSubClass";
  }
};

GUICPP_INJECT_CTOR(TestTopLevelSubClass, (
    TestSimpleAssistedArgumentUser* simple_user,
    guicpp::At<guicpp::Assisted, TestSimpleInjectableClass*> simple_object));


// Used to track memory deletions.
typedef ::testing::MockFunction<void(const void* ptr)> TestDeleteMarker;

// Test class that reports memory deletion by calling delete_marker_->Call().
class TestClassWithDeleteMarker: public TestBaseClass {
 public:
  TestClassWithDeleteMarker(): TestBaseClass(0), delete_marker_(NULL) {}

  ~TestClassWithDeleteMarker() {
    if (delete_marker_) {
      delete_marker_->Call(this);  // Report deletion of this object.
    }
  }

  // Used when marker.
  void SetDeleteMarker(TestDeleteMarker* marker) {
    delete_marker_ = marker;
  }

  virtual string GetClassName() const {
    return "TestClassWithDeleteMarker";
  }

 private:
  TestDeleteMarker* delete_marker_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(TestClassWithDeleteMarker);
};

GUICPP_INJECT_CTOR(TestClassWithDeleteMarker, ());

// Factory to create TestTopLevelClass that takes
// TestSimpleInjectableClass as runtime argument.
class TestFactoryInterface: public guicpp::Factory<
      TestTopLevelClass* (TestSimpleInjectableClass* simple_object)> {};

// Abstract provider that creates TestTopLevelClass and needs an instance
// of TestSimpleInjectableClass to create it.
class TestProvider: public guicpp::AbstractProvider<
      TestTopLevelClass* (TestSimpleInjectableClass* simple_object)> {
 public:
  TestProvider(bool* is_provider_called_once, bool* is_provider_deleted);
  virtual ~TestProvider();

  TestTopLevelClass* Get(TestSimpleInjectableClass* simple_object);

 private:
  bool* is_provider_called_once_;
  bool* is_provider_deleted_;
};


// Factory to test non-pointer return type.
class TestValueFactoryInterface: public guicpp::Factory<int()> {};

// Provider to test non-pointer return type.
// A custom provider that returns next number each time starting from 0.
class TestValueProvider: public guicpp::AbstractProvider<int ()> {
 public:
  explicit TestValueProvider(bool* is_provider_deleted)
      : is_provider_deleted_(is_provider_deleted), count_(0) {
    *is_provider_deleted_ = false;
  }

  virtual ~TestValueProvider() {
    *is_provider_deleted_ = true;
  }

  int Get() {
    return count_++;
  }

 private:
  bool* is_provider_deleted_;
  int count_;
};

// Creates injector without any bindings.
guicpp::Injector* GetEmptyInjector();


// Table entries for testing

// pointer type
template <typename T>
class TestPointerEntry: public guicpp::internal::TableEntry<T*> {
 public:
  explicit TestPointerEntry(T* ptr): ptr_(ptr) {}

  virtual T* Get(
      const guicpp::Injector* /* injector */,
      const guicpp::internal::LocalContext* /* local_context */) const {
    return ptr_;
  }

  virtual guicpp::internal::TableEntryBase::BindType GetBindType() const {
    // We can return any value except INVALID_BIND
    return TestPointerEntry::BIND_TO_INSTANCE;
  }

 private:
  T* ptr_;
};

// non-pointer type
template <typename T>
class TestValueEntry: public guicpp::internal::TableEntry<T> {
 public:
  explicit TestValueEntry(T value): value_(value) {}

  virtual T Get(
      const guicpp::Injector* /* injector */,
      const guicpp::internal::LocalContext* /* local_context */) const {
    return value_;
  }

  virtual guicpp::internal::TableEntryBase::BindType GetBindType() const {
    // We can return any value except INVALID_BIND
    return TestValueEntry::BIND_TO_VALUE;
  }

 private:
  T value_;
};



// Mock cleanup action.
template <typename T>
struct MockCleanupAction {
  MOCK_METHOD1_T(Cleanup, void(T* ptr));
};

template <typename T>
struct TestCleanupAction {
  explicit TestCleanupAction(MockCleanupAction<T>* m): mock_action(m) {}

  void operator()(T* ptr) {
    mock_action->Cleanup(ptr);
  }

  MockCleanupAction<T>* mock_action;
};

ACTION(DeleteFirstArgument) { delete arg0; }

}  // namespace guicpp_test
#endif  // GUICPP_TEST_HELPER_H_

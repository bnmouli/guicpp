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


#include "include/guicpp_test_helper.h"

#include "include/gtest/gtest.h"
#include "guicpp/guicpp_injector.h"
#include "include/guicpp_test_modules.h"

namespace guicpp_test {
GUICPP_DEFINE(TestSimpleInjectableClass);
GUICPP_DEFINE(TestInjectableSubClass);
GUICPP_DEFINE(TestSimpleClassUser);
GUICPP_DEFINE(TestSimpleAssistedArgumentUser);
GUICPP_DEFINE(TestTopLevelClass);
GUICPP_DEFINE(TestTopLevelSubClass);
GUICPP_DEFINE(TestClassWithDeleteMarker);

// Creates injector without any bindings.
guicpp::Injector* GetEmptyInjector() {
  EmptyModule empty_module;
  return guicpp::Injector::Create(&empty_module);
}

TestProvider::TestProvider(bool* is_provider_called_once,
                           bool* is_provider_deleted)
  : is_provider_called_once_(is_provider_called_once),
    is_provider_deleted_(is_provider_deleted) {
  *is_provider_called_once = false;
  *is_provider_deleted = false;
}

TestProvider::~TestProvider() {
  *is_provider_deleted_ = true;
}

TestTopLevelClass* TestProvider::Get(TestSimpleInjectableClass* simple_object) {
  // provider is not called more than once in any test.
  EXPECT_FALSE(*is_provider_called_once_);
  *is_provider_called_once_ = true;

  EXPECT_TRUE(NULL != simple_object);

  TestSimpleAssistedArgumentUser* simple_user =
      new TestSimpleAssistedArgumentUser(simple_object);
  return new TestTopLevelSubClass(simple_user, simple_object);
}

}  // namespace guicpp_test

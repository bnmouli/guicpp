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


#include "include/guicpp_test_modules.h"

#include "guicpp/guicpp_at.h"
#include "guicpp/guicpp_binder.h"
#include "include/guicpp_test_helper.h"

namespace guicpp_test {
// Does no binding.
void EmptyModule::Configure(guicpp::Binder* binder) const {
}

// Binds TestSimpleInjectableClass to TestInjectableSubClass.
void TestSimpleInjectableClassModule::Configure(guicpp::Binder* binder) const {
  binder->Bind<TestSimpleInjectableClass, TestInjectableSubClass>();
}

// Binds TestBaseClass to TestSimpleInjectableClass.
void TestBaseClassModule::Configure(guicpp::Binder* binder) const {
  binder->Bind<TestBaseClass, TestSimpleInjectableClass>();
}

// Module binds TestBaseClass with TestLabelOne to TestSimpleInjectableClass.
void TestBaseClassLabelOneModule::Configure(guicpp::Binder* binder) const {
  binder->Bind<guicpp::At<TestLabelOne, TestBaseClass>,
      TestSimpleInjectableClass>();
}

// Module binds:
//  TestBaseClass to TestSimpleInjectableClass
//  TestBaseClass with TestLabelOne to TestSimpleInjectableClass
//  TestBaseClass with TestLabelTwo to TestInjectableSubClass
void TestBaseClassMultiBindModule::Configure(guicpp::Binder* binder) const {
  binder->Bind<TestBaseClass, TestSimpleInjectableClass>();

  binder->Bind<guicpp::At<TestLabelOne, TestBaseClass>,
      TestSimpleInjectableClass>();

  binder->Bind<guicpp::At<TestLabelTwo, TestBaseClass>,
      TestInjectableSubClass>();
}

// Module used for testing factories
void TestTopLevelSubClassBindModule::Configure(guicpp::Binder* binder) const {
  binder->Bind<guicpp::At<TestLabelOne, TestTopLevelClass>,
      TestTopLevelSubClass>();
}

}  // namespace guicpp_test

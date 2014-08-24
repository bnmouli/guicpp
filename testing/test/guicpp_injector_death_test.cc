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


// Death tests for Injector class.

#include "guicpp/guicpp_injector.h"

#include "include/gtest/gtest.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/guicpp_binder.h"
#include "guicpp/guicpp.h"
#include "guicpp/guicpp_module.h"
#include "include/guicpp_test_helper.h"
#include "include/guicpp_test_modules.h"

namespace guicpp {
using guicpp_test::TestBaseClass;
using guicpp_test::TestBaseClassLabelOneModule;
using guicpp_test::TestBaseClassModule;
using guicpp_test::TestLabelOne;
using guicpp_test::TestLabelTwo;
using guicpp_test::TestSimpleInjectableClass;
using guicpp_test::TestSimpleInjectableClassModule;

// Module with duplicate binding.
class DuplicateBindModule: public Module {
  void Configure(Binder* binder) const {
    binder->Bind<TestBaseClass, TestSimpleInjectableClass>();
    binder->Bind<TestBaseClass, TestSimpleInjectableClass>();  // Duplicate
  }
};

TEST(GuicppInjectorDeathTest, Compile_FailsOnDuplicateBinding) {
  DuplicateBindModule module;
  // Duplicate binding cause Create() to fail fatally.
  EXPECT_DEATH(Injector::Create(&module),
               "Creation of Injector failed: .* 1 errors.*");
}

// Module with triplicate binding.
class TriplicateBindModule: public Module {
  void Configure(Binder* binder) const {
    binder->Bind<TestBaseClass, TestSimpleInjectableClass>();
    binder->Bind<TestBaseClass, TestSimpleInjectableClass>();  // Duplicate
    binder->Bind<TestBaseClass, TestSimpleInjectableClass>();  // Triplicate
  }
};

TEST(GuicppInjectorDeathTest, Compile_KeepsTrackOfNumberOfErrors) {
  TriplicateBindModule module;
  // Verify number of errors counted correctly.
  EXPECT_DEATH(Injector::Create(&module),
               "Creation of Injector failed: .* 2 errors.*");
}

// Tests for Injector::Get()

TEST(GuicppInjectorDeathTest, Get_FailsForAbstractClassIfNotBound) {
  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());

  EXPECT_DEATH(injector->Get<TestBaseClass*>(), "can not be instantiated");
}

TEST(GuicppInjectorTest, Get_FailsWhenBaseClassIsBoundButGetWithAnnotation) {
  // This module binds TestBaseClass to TestSimpleInjectableClass.
  TestBaseClassModule module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  // TestBaseClass is bound to TestSimpleInjectableClass but TestBaseClass
  // annotated with a label requires a separate binding.
  EXPECT_DEATH((injector->Get<At<TestLabelOne,
                TestBaseClass*> >()), "can not be instantiated");
}

TEST(GuicppInjectorTest, Get_WithoutAnnotationFailsWhenBoundWithAnnotation) {
  // This module binds TestBaseClass to TestSimpleInjectableClass.
  TestBaseClassLabelOneModule module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  // TestBaseClass with label TestLabelOne, is bound to
  // TestSimpleInjectableClass. That binding does not apply to TestBaseClass
  // without label.
  EXPECT_DEATH(injector->Get<TestBaseClass*>(), "can not be instantiated");
}

TEST(GuicppInjectorTest, Get_FailsForValueTypeIfNotBound) {
  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());

  // int with LabelOne is not bound.
  EXPECT_DEATH((injector->Get<At<TestLabelOne, int> >()), "missing binding");
}

TEST(GuicppInjectorTest, Get_ValueTypeFailsIfBindTypeIsPointer) {
  // This module binds TestSimpleInjectableClass to TestInjectableSubClass.
  TestSimpleInjectableClassModule module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  // TestSimpleInjectableClass is bound to pointer type but requested for value
  // type. This fails fatally.
  EXPECT_DEATH(injector->Get<TestSimpleInjectableClass>(),
               "Can not convert.*T");
}

// Binds TestSimpleInjectableClass to a value.
class TestSimpleInjectableClassValueModule: public Module {
  void Configure(Binder* binder) const {
    TestSimpleInjectableClass value(100);
    binder->BindToValue<TestSimpleInjectableClass>(value);
  }
};

TEST(GuicppInjectorTest, Get_PointerTypeFailsIfBoundToValue) {
  TestSimpleInjectableClassValueModule module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  // TestSimpleInjectableClass is bound to value type but requested for pointer
  // type. This fails fatally.
  EXPECT_DEATH(injector->Get<TestSimpleInjectableClass*>(),
               "Can not convert.*T");
}

}  // namespace guicpp

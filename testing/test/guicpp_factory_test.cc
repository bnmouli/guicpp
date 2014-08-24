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


// Tests for RealFactory in factory_helpers.h

#include "guicpp/guicpp_factory.h"

#include "include/gtest/gtest.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/internal/guicpp_util.h"
#include "include/guicpp_test_helper.h"
#include "include/guicpp_test_modules.h"

#include "guicpp/guicpp_factory.h"

namespace guicpp {
namespace internal {
using guicpp_test::TestBaseClass;
using guicpp_test::TestTopLevelSubClassBindModule;
using guicpp_test::TestFactoryInterface;
using guicpp_test::TestLabelOne;
using guicpp_test::TestSimpleInjectableClass;
using guicpp_test::TestTopLevelClass;
using guicpp_test::TestTopLevelSubClass;

TEST(RealFactoryTest, Get_PassesArgsToAllObjectsThatRequireIt) {
  TestTopLevelSubClassBindModule module;
  scoped_ptr<Injector> injector(Injector::Create(&module));
  RealFactory<
      EmptyAnnotations,
      TestFactoryInterface,
      TestFactoryInterface::GuicppGetSignature> real_factory(injector.get());

  // Note: the ownership of this pointer is taken by TestSimpleClassUser
  // which is created by the factory. There is no neat way of finding/deciding
  // who should own the pointers passed to factory. As of now, user has to
  // manually take care that exactly one recipient of this pointer owns this
  // pointer.
  TestSimpleInjectableClass* object = new TestSimpleInjectableClass(100);

  scoped_ptr<TestTopLevelClass> top_object(real_factory.Get(object));
  EXPECT_EQ("TestTopLevelClass", top_object->GetClassName());

  // Check both TestTopLevelClass and TestSimpleClassUser get "object"
  // passed to real_factory.Get().
  EXPECT_EQ(object, top_object->simple_object());
  EXPECT_EQ(object, top_object->simple_user()->simple_object());
}

TEST(RealFactoryTest,
     LabelAppliedToFactoryIsTreatedAsIfItIsAppliedToReturnType) {
  TestTopLevelSubClassBindModule module;
  scoped_ptr<Injector> injector(Injector::Create(&module));

  // Factory is annotated with TestLabelOne. It is treated as if it
  // is applied to return type of the factory. Here TestTopLevelClass
  // is bound to TestTopLevelSubClass in TestTopLevelSubClassBindModule
  // and hence this factory creates an instance of TestTopLevelSubClass.
  RealFactory<Annotations<TestLabelOne>, TestFactoryInterface,
      TestFactoryInterface::GuicppGetSignature> real_factory(injector.get());

  TestSimpleInjectableClass* object = new TestSimpleInjectableClass(100);
  scoped_ptr<TestTopLevelClass> top_object(real_factory.Get(object));

  // Check factory creates an instance of TestTopLevelSubClass.
  EXPECT_EQ("TestTopLevelSubClass", top_object->GetClassName());

  // Check both TestTopLevelClass and TestSimpleClassUser get "object"
  // passed to real_factory.Get().
  EXPECT_EQ(object, top_object->simple_object());
  EXPECT_EQ(object, top_object->simple_user()->simple_object());
}

}  // namespace internal
}  // namespace guicpp

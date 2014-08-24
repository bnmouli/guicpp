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


// Tests for Injector class.

#include "guicpp/guicpp_injector.h"

#include "include/gtest/gtest.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_annotations.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/guicpp_binder.h"
#include "guicpp/guicpp_macros.h"
#include "guicpp/guicpp_module.h"
#include "include/guicpp_test_helper.h"
#include "include/guicpp_test_modules.h"

namespace guicpp {
using guicpp_test::EmptyModule;
using guicpp_test::TestBaseClass;
using guicpp_test::TestBaseClassLabelOneModule;
using guicpp_test::TestBaseClassModule;
using guicpp_test::TestBaseClassMultiBindModule;
using guicpp_test::TestTopLevelSubClassBindModule;
using guicpp_test::TestFactoryInterface;
using guicpp_test::TestLabelOne;
using guicpp_test::TestLabelTwo;
using guicpp_test::TestSimpleInjectableClass;
using guicpp_test::TestSimpleInjectableClassModule;
using guicpp_test::TestTopLevelClass;


TEST(GuicppInjectorTest, Get_InstantiatesSameClassByDefault) {
  EmptyModule module;
  scoped_ptr<Injector> injector(Injector::Create(&module));

  scoped_ptr<TestSimpleInjectableClass> object(
      injector->Get<TestSimpleInjectableClass*>());

  EXPECT_EQ("TestSimpleInjectableClass", object->GetClassName());
}

TEST(GuicppInjectorTest, Get_ConstPointerCanBeRequstedWhenBindingIsDefault) {
  EmptyModule module;
  scoped_ptr<Injector> injector(Injector::Create(&module));

  scoped_ptr<const TestSimpleInjectableClass> object(
      injector->Get<const TestSimpleInjectableClass*>());

  EXPECT_EQ("TestSimpleInjectableClass", object->GetClassName());
}

TEST(GuicppInjectorTest, Get_InstantiatesBoundTypeForConcreteClass) {
  // This module binds TestSimpleInjectableClass to TestInjectableSubClass.
  TestSimpleInjectableClassModule module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  scoped_ptr<TestSimpleInjectableClass> object(
      injector->Get<TestSimpleInjectableClass*>());

  EXPECT_EQ("TestInjectableSubClass", object->GetClassName());
}

TEST(GuicppInjectorTest, Get_ConstPointerCanBeRequestedWhenExplicitlyBound) {
  // This module binds TestSimpleInjectableClass to TestInjectableSubClass.
  TestSimpleInjectableClassModule module;
  scoped_ptr<Injector> injector(Injector::Create(&module));

  scoped_ptr<const TestSimpleInjectableClass> object(
      injector->Get<const TestSimpleInjectableClass*>());

  EXPECT_EQ("TestInjectableSubClass", object->GetClassName());
}

TEST(GuicppInjectorTest, Get_InstantiatesBoundTypeForAbstractClass) {
  // This module binds TestBaseClass to TestSimpleInjectableClass.
  TestBaseClassModule module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  // Get() of TestBaseClass returns TestSimpleInjectableClass
  // as it is bound in TestBaseClassModule.
  scoped_ptr<TestBaseClass> object(injector->Get<TestBaseClass*>());
  EXPECT_EQ("TestSimpleInjectableClass", object->GetClassName());
}

TEST(GuicppInjectorTest, Get_InstantiatesObjectsBoudWithLabel) {
  // This module binds TestBaseClass to TestSimpleInjectableClass.
  TestBaseClassLabelOneModule module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  // Get() of TestBaseClass annotated with TestLabelOne returns
  // TestSimpleInjectableClass as it is bound in TestBaseClassModule.
  scoped_ptr<TestBaseClass> object(
      injector->Get<At<TestLabelOne, TestBaseClass*> >());
  EXPECT_EQ("TestSimpleInjectableClass", object->GetClassName());
}

TEST(GuicppInjectorTest, Get_SelectsCorrectlyLabeledBinding) {
  // This module binds TestBaseClass to TestSimpleInjectableClass.
  TestBaseClassMultiBindModule module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  // Get() returns different instances based on what is bound.
  scoped_ptr<TestBaseClass> object(injector->Get<TestBaseClass*>());
  EXPECT_EQ("TestSimpleInjectableClass", object->GetClassName());

  scoped_ptr<TestBaseClass> object_l1(
      injector->Get<At<TestLabelOne, TestBaseClass*> >());
  EXPECT_EQ("TestSimpleInjectableClass", object_l1->GetClassName());

  scoped_ptr<TestBaseClass> object_l2(
      injector->Get<At<TestLabelTwo, TestBaseClass*> >());
  EXPECT_EQ("TestInjectableSubClass", object_l2->GetClassName());
}

TEST(GuicppInjectorTest, Get_ReturnsFactoryImplementation) {
  TestTopLevelSubClassBindModule module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  scoped_ptr<TestFactoryInterface> factory(
      injector->Get<TestFactoryInterface*>());

  TestSimpleInjectableClass* simple_1 =
      new TestSimpleInjectableClass(100);
  scoped_ptr<TestTopLevelClass> top_class_1(factory->Get(simple_1));

  TestSimpleInjectableClass* simple_2 =
      new TestSimpleInjectableClass(200);
  scoped_ptr<TestTopLevelClass> top_class_2(factory->Get(simple_2));

  EXPECT_EQ("TestTopLevelClass", top_class_1->GetClassName());
  EXPECT_EQ(100, top_class_1->simple_object()->value());
  EXPECT_EQ("TestTopLevelClass", top_class_2->GetClassName());
  EXPECT_EQ(200, top_class_2->simple_object()->value());
}

TEST(GuicppInjectorTest, Get_AnnotationToFactoryAppliesToReturnType) {
  TestTopLevelSubClassBindModule module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  scoped_ptr<TestFactoryInterface> factory(
      injector->Get<At<TestLabelOne, TestFactoryInterface*> >());

  TestSimpleInjectableClass* simple_1 =
      new TestSimpleInjectableClass(100);
  scoped_ptr<TestTopLevelClass> top_class_1(factory->Get(simple_1));

  EXPECT_EQ("TestTopLevelSubClass", top_class_1->GetClassName());
}

TEST(GuicppInjectorTest, Get_ReturnsConstFactoryImplementation) {
  TestTopLevelSubClassBindModule module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  scoped_ptr<const TestFactoryInterface> factory(
      injector->Get<const TestFactoryInterface*>());

  TestSimpleInjectableClass* simple_1 =
      new TestSimpleInjectableClass(100);
  scoped_ptr<TestTopLevelClass> top_class_1(factory->Get(simple_1));

  TestSimpleInjectableClass* simple_2 =
      new TestSimpleInjectableClass(200);
  scoped_ptr<TestTopLevelClass> top_class_2(factory->Get(simple_2));

  EXPECT_EQ("TestTopLevelClass", top_class_1->GetClassName());
  EXPECT_EQ(100, top_class_1->simple_object()->value());
  EXPECT_EQ("TestTopLevelClass", top_class_2->GetClassName());
  EXPECT_EQ(200, top_class_2->simple_object()->value());
}

TEST(GuicppInjectorTest, Get_ConstFactoryWithAnnotation) {
  TestTopLevelSubClassBindModule module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  scoped_ptr<const TestFactoryInterface> factory(
      injector->Get<At<TestLabelOne, const TestFactoryInterface*> >());

  TestSimpleInjectableClass* simple_1 =
      new TestSimpleInjectableClass(100);
  scoped_ptr<TestTopLevelClass> top_class_1(factory->Get(simple_1));

  EXPECT_EQ("TestTopLevelSubClass", top_class_1->GetClassName());
}

class TestPortNumberLabel: public Label {};
class TestIpAddressLabel: public Label {};

// Container struct for IP address.
struct IpAddress {
  explicit IpAddress(uint32 ip): value(ip) {}

  const uint32 value;
};

GUICPP_INJECTABLE(IpAddress);

class TestValueBinderClass: public Module {
 public:
  void Configure(Binder* binder) const {
    binder->BindToValue<TestSimpleInjectableClass>(
        TestSimpleInjectableClass(300));

    // Each of the following can have different bindings.
    //   * uint32
    //   * At<TestPortNumberLabel, uint32>
    //   * At<TestIpAddressLabel, uint32>
    //   * IpAddress
    //   * At<TestIpAddressLabel, IpAddress>

    // Note: it is bad idea to bind values to primitive types
    // without any annotation. However, it works.
    binder->BindToValue<uint32>(1000U);

    binder->BindToValue<At<TestPortNumberLabel, uint32> >(80);
    binder->BindToValue<At<TestIpAddressLabel, uint32> >(100);

    binder->BindToValue<IpAddress>(IpAddress(200));

    binder->BindToValue<At<TestIpAddressLabel, IpAddress> >(IpAddress(300));
  };
};

TEST(GuicppInjectorTest, Get_CanGetNonPointerType) {
  TestValueBinderClass module;
  scoped_ptr<Injector> injector(Injector::Create(& module));

  TestSimpleInjectableClass object =
      injector->Get<TestSimpleInjectableClass>();

  EXPECT_EQ(300, object.value());

  uint32 value = injector->Get<uint32>();
  EXPECT_EQ(1000U, value);

  uint32 port = injector->Get<At<TestPortNumberLabel, uint32> >();
  EXPECT_EQ(80, port);

  uint32 ip = injector->Get<At<TestIpAddressLabel, uint32> >();
  EXPECT_EQ(100, ip);

  IpAddress ip_address_1 = injector->Get<IpAddress>();
  EXPECT_EQ(200, ip_address_1.value);

  IpAddress ip_address_2 = injector->Get<At<TestIpAddressLabel, IpAddress> >();
  EXPECT_EQ(300, ip_address_2.value);
}

TEST(GuicppInjectorTest, Get_ReturnsThisWhenCalledForInjector) {
  EmptyModule module;
  scoped_ptr<Injector> injector(Injector::Create(&module));

  EXPECT_EQ(injector.get(), injector->Get<Injector*>());
}

TEST(GuicppInjectorTest, Get_ReturnsThisWhenCalledForConstInjector) {
  EmptyModule module;
  scoped_ptr<Injector> injector(Injector::Create(&module));

  EXPECT_EQ(injector.get(), injector->Get<const Injector*>());
}

// Tests for Injector::Create()

TEST(GuicppInjectorTest, Compile_CreatesInjector) {
  TestSimpleInjectableClassModule module;
  scoped_ptr<Injector> injector(Injector::Create(&module));
  // Verify injector created successfully.
  EXPECT_TRUE(injector.get() != NULL);
}

// Tests for Injector::Get()
}  // namespace guicpp

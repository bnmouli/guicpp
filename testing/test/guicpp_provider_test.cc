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


#include "guicpp/guicpp_provider.h"

#include "guicpp/guicpp_binder.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/guicpp_macros.h"
#include "guicpp/guicpp_module.h"
#include "guicpp/internal/guicpp_local_context.h"
#include "guicpp/internal/guicpp_port.h"
#include "include/gmock/gmock.h"
#include "include/gtest/gtest.h"
#include "include/guicpp_test_helper.h"

namespace guicpp {
using guicpp_test::GetEmptyInjector;
using guicpp_test::TestSimpleClassUser;
using guicpp_test::TestSimpleInjectableClass;
using testing::NotNull;

class GuicppProviderTest: public ::testing::Test {
 protected:
  template <typename R, typename Provider>
  R CallInvokeGet(const Injector* injector,
                  internal::LocalContext* local_context,
                  Provider* provider) {
    return provider->InvokeGet(injector, local_context);
  }
};

class TestProviderMock : public AbstractProvider<
      TestSimpleClassUser* (TestSimpleInjectableClass* s)> {
 public:
  MOCK_METHOD1(Get, TestSimpleClassUser* (TestSimpleInjectableClass* s));
};

ACTION(ReturnNewTestSimpleClassUser) {
  return new TestSimpleClassUser(arg0);
}

TEST_F(GuicppProviderTest, InvokeGet_CallsGetToCreateObject) {
  TestProviderMock provider_mock;

  EXPECT_CALL(provider_mock, Get(NotNull()))
      .WillOnce(ReturnNewTestSimpleClassUser());

  internal::LocalContext local_context;
  scoped_ptr<Injector> injector(GetEmptyInjector());

  scoped_ptr<TestSimpleClassUser> user(
      CallInvokeGet<TestSimpleClassUser*>(
          injector.get(), &local_context, &provider_mock));

  EXPECT_THAT(user->simple_object(), NotNull());
}

// To test arguments of provider may not be injectable at place where it is used
// for binding.

class ProviderArgClass;

class ProvidedClass {
 public:
  ~ProvidedClass();

  static ProvidedClass* Create(ProviderArgClass* arg) {
    return new ProvidedClass(arg);
  }

  ProviderArgClass* getArg() {
    return arg_.get();
  }

 private:
  ProvidedClass(ProviderArgClass* arg);

  scoped_ptr<ProviderArgClass> arg_;
};

GUICPP_INJECTABLE(ProvidedClass);

class TestProvider: public AbstractProvider<
                    ProvidedClass* (ProviderArgClass* arg)> {
 public:
  // Constructor can't be defined here as at this point ProviderArgClass
  // is not injectable.
  TestProvider();

  ProvidedClass* Get(ProviderArgClass* arg) {
    return ProvidedClass::Create(arg);
  }
};

class ProviderTestModule: public Module {
  void Configure(Binder* binder) const {
    binder->BindToProvider<ProvidedClass>(new TestProvider(), DeletePointer());
  }
};

TEST_F(GuicppProviderTest, InvokeGet_ArgsCanBeNonInjectableAtInvocation) {
  ProviderTestModule module;
  scoped_ptr<Injector> injector(Injector::Create(&module));
  scoped_ptr<ProvidedClass>  obj(injector->Get<ProvidedClass*>());
  EXPECT_THAT(obj->getArg(), NotNull());
}

class ProviderArgClass {};
GUICPP_INJECT_CTOR(ProviderArgClass, ());
GUICPP_DEFINE(ProviderArgClass);

ProvidedClass::ProvidedClass(ProviderArgClass* arg): arg_(arg) {}
ProvidedClass::~ProvidedClass() {}

TestProvider::TestProvider() {}

}  // namespace guicpp

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


#include "guicpp/internal/guicpp_local_context.h"

#include "include/gtest/gtest.h"
#include "include/gmock/gmock.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/guicpp.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/internal/guicpp_table.h"
#include "include/guicpp_test_helper.h"
#include "guicpp/internal/guicpp_types.h"
#include "guicpp/internal/guicpp_util.h"

namespace guicpp {
namespace internal {
using guicpp_test::TestLabelOne;
using guicpp_test::TestLabelTwo;
using guicpp_test::TestSimpleInjectableClass;
using testing::Eq;

TEST(FactoryArgumentEntryTest, Get_ReturnsObjectTakenInCtor) {
  TestSimpleInjectableClass object1(100);
  FactoryArgumentEntry<TestSimpleInjectableClass> entry(object1);

  EXPECT_EQ(TypeIdProvider<TestSimpleInjectableClass>::GetTypeId(),
            entry.GetTypeId());
  EXPECT_EQ(TypesCategory::IS_VALUE, entry.GetCategory());
  EXPECT_EQ(TableEntryBase::BIND_FACTORY_ARGUMENT, entry.GetBindType());

  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());
  internal::LocalContext local_context;

  const TestSimpleInjectableClass object2 =
      entry.Get(injector.get(), &local_context);
  EXPECT_EQ(100, object2.value());
}

TEST(FactoryArgumentEntryTest, Get_ReturnsPointerTakenInCtor) {
  TestSimpleInjectableClass object1;
  FactoryArgumentEntry<TestSimpleInjectableClass*> entry(&object1);

  EXPECT_EQ(TypeIdProvider<TestSimpleInjectableClass>::GetTypeId(),
            entry.GetTypeId());
  EXPECT_EQ(TypesCategory::IS_POINTER, entry.GetCategory());
  EXPECT_EQ(TableEntryBase::BIND_FACTORY_ARGUMENT, entry.GetBindType());

  scoped_ptr<Injector> injector(guicpp_test::GetEmptyInjector());
  internal::LocalContext local_context;

  const TestSimpleInjectableClass* object2 =
      entry.Get(injector.get(), &local_context);
  EXPECT_EQ(object2, &object1);
}

TEST(LocalContextTest, FindEntry_FindsEntryMatchingTypeId) {
  FactoryArgumentEntry<int> entry1(10);
  TypeId type_id1 =
      InjectorUtil::GetFactoryArgsBindId<At<TestLabelOne, int> >();

  FactoryArgumentEntry<int> entry2(20);
  TypeId type_id2 =
      InjectorUtil::GetFactoryArgsBindId<At<TestLabelTwo, int> >();
  TypeId type_id3 =
      InjectorUtil::GetFactoryArgsBindId<At<TestLabelOne, double> >();

  const TypeIdArgumentPair argument_list[] = {
    { type_id1, &entry1 },
    { type_id2, &entry2 },
  };

  LocalContext local_context(argument_list, arraysize(argument_list));

  const TableEntryBase* bentry1 = local_context.FindEntry(type_id1);
  EXPECT_EQ(bentry1, &entry1);

  const TableEntryBase* bentry2 = local_context.FindEntry(type_id2);
  EXPECT_EQ(bentry2, &entry2);

  EXPECT_EQ(NULL, local_context.FindEntry(type_id3));
}

}  // namespace internal
}  // namespace guicpp

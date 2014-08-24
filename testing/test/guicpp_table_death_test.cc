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


// Test for classes declared in table.h.

#include "guicpp/internal/guicpp_table.h"

#include "include/gtest/gtest.h"
#include "guicpp/internal/guicpp_port.h"
#include "include/guicpp_test_helper.h"

namespace guicpp {
namespace internal {
using guicpp_test::TestPointerEntry;
using guicpp_test::TestSimpleInjectableClass;
using guicpp_test::TestValueEntry;

TEST(BindTableTest, TableEntryReader_GetDiesOnValueToPointerConversion) {
  TestSimpleInjectableClass test_value(10);
  TestValueEntry<TestSimpleInjectableClass> test_entry(test_value);
  TableEntryBase* entry_base = &test_entry;

  EXPECT_DEATH(TableEntryReader<TestSimpleInjectableClass*>::
               Get(entry_base, NULL, NULL),
               "Can not convert.*value of type T.*pointer to T");
}

TEST(BindTableTest, TableEntryReader_GetDiesOnPointerToValueConversion) {
  TestSimpleInjectableClass test_value(10);
  TestPointerEntry<TestSimpleInjectableClass> test_entry(&test_value);
  TableEntryBase* entry_base = &test_entry;

  EXPECT_DEATH(TableEntryReader<TestSimpleInjectableClass>::
               Get(entry_base, NULL, NULL),
               "Can not convert.*pointer to T.*value of type T");
}

TEST(BindTableTest, TableEntryReader_GetReadsConstPointerType) {
  const TestSimpleInjectableClass test_value(10);
  TestPointerEntry<const TestSimpleInjectableClass> test_entry(&test_value);
  TableEntryBase* entry_base = &test_entry;

  EXPECT_DEATH(TableEntryReader<TestSimpleInjectableClass*>::
               Get(entry_base, NULL, NULL),
               "Can not convert.*const pointer to T.*pointer to T");
}

}  // namespace internal
}  // namespace guicpp

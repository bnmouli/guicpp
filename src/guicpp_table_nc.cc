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


// Negative compilation tests for the DefaultEntryUtil.
// Compilation errors in this file are verified in table_nc_test.py.

#include "guicpp/internal/guicpp_table.h"
#include "include/guicpp_test_helper.h"

namespace guicpp {
namespace internal {
using guicpp_test::TestNonInjectableClass;
using guicpp_test::TestNonInjectableSubClass;
using guicpp_test::TestSimpleInjectableClass;

#if defined(TEST_NON_INJECTABLE)
void Test() {
  DefaultEntryUtil<TestNonInjectableClass>::GetTypeId();
}

#elif defined(TEST_NON_INJECTABLE_SUBCLASS)
void Test() {
  DefaultEntryUtil<TestNonInjectableSubClass>::GetTypeId();
}

#else
// Sanity test, DefaultEntryUtil should work for TestSimpleInjectableClass.
void Test() {
  DefaultEntryUtil<TestSimpleInjectableClass>::GetTypeId();
}

#endif

}  // namespace internal
}  // namespace guicpp

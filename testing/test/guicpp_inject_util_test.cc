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


// Most of the code in guicpp_inject_util.h gets tested by
// guicpp_injector_test.cc, we can add more tests when and
// if required.

#include "guicpp/internal/guicpp_inject_util.h"

#include "guicpp/guicpp_annotations.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/internal/guicpp_types.h"
#include "guicpp/internal/guicpp_util.h"
#include "include/gtest/gtest.h"
#include "include/guicpp_test_helper.h"

namespace guicpp {
namespace internal {
using guicpp_test::TestSimpleInjectableClass;

template <typename ExpectedType, typename ActualType>
bool IsSameType(ActualType t) {
  return is_same<ExpectedType, ActualType>::value;
}


TEST(GuicppGetInjectHandler, SelectsNormalInjectHandlerForClasses) {
  EXPECT_TRUE((IsSameType<NormalInjectHandler<
                              EmptyAnnotations, TestSimpleInjectableClass> >(
               GuicppGetInjectHandler(
                   TypeKey3<NormalInject, EmptyAnnotations,
                            TestSimpleInjectableClass>(),
                   true_type()))));
}

TEST(GuicppGetInjectHandler,
     SelectsMissingDefaultTypeHandlerForPrimitiveTypes) {
  EXPECT_TRUE((IsSameType<MissingDefaultTypeHandler<EmptyAnnotations, int> >(
               GuicppGetInjectHandler(
                   TypeKey3<NormalInject, EmptyAnnotations, int>(),
                   true_type()))));
}

TEST(GuicppGetInjectHandler, SelectsInternalTypeInjectHandlerForInternalTypes) {
  EXPECT_TRUE((IsSameType<InternalTypeInjectHandler<
                              EmptyAnnotations, Injector> >(
               GuicppGetInjectHandler(
                   TypeKey3<NormalInject, EmptyAnnotations, Injector>(),
                   true_type()))));
}

TEST(GuicppGetInjectHandler, SelectsAssistedInjectHandlerForAssistedTypes) {
  EXPECT_TRUE((IsSameType<AssistedInjectHandler<
                              EmptyAnnotations, TestSimpleInjectableClass> >(
               GuicppGetInjectHandler(
                   TypeKey3<Assisted, EmptyAnnotations,
                            TestSimpleInjectableClass>(),
                   true_type()))));
}

}  // namespace internal
}  // namespace guicpp

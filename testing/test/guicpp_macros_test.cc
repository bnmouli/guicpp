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


// Tests for Guic++ macros.

#include "guicpp/guicpp_macros.h"

#include "guicpp/internal/guicpp_inject_util.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/internal/guicpp_util.h"
#include "include/gtest/gtest.h"

using guicpp::internal::TypeIdProvider;

class SimpleConcreteClass {};

GUICPP_INJECT_CTOR(SimpleConcreteClass, ());
GUICPP_DEFINE(SimpleConcreteClass);

class SimpleAbstractClass {};
GUICPP_INJECTABLE(SimpleAbstractClass);

template <typename T>
class SimpleTemplateClass {};

template <typename T>
GUICPP_TEMPLATE_INJECT_CTOR((SimpleTemplateClass<T>), ());

template <typename T>
class SimpleTemplateAbstractClass {};

template <typename T>
GUICPP_TEMPLATE_INJECTABLE((SimpleTemplateAbstractClass<T>));


// TODO(bnmouli): Change this to get look for table entry.
TEST(GuicppMacrosTest, GetTypeIdWorksForAllClasses) {
  EXPECT_EQ(TypeIdProvider<SimpleConcreteClass>::GetTypeId(),
            TypeIdProvider<SimpleConcreteClass>::GetTypeId());

  EXPECT_EQ(TypeIdProvider<SimpleAbstractClass>::GetTypeId(),
            TypeIdProvider<SimpleAbstractClass>::GetTypeId());

  EXPECT_EQ(TypeIdProvider<SimpleTemplateClass<int> >::GetTypeId(),
            TypeIdProvider<SimpleTemplateClass<int> >::GetTypeId());

  EXPECT_EQ(TypeIdProvider<SimpleTemplateAbstractClass<int> >::GetTypeId(),
            TypeIdProvider<SimpleTemplateAbstractClass<int> >::GetTypeId());
}

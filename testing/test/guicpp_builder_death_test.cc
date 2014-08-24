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

// Death test for classes in builder.h

#include "guicpp/internal/guicpp_builder.h"

#include "include/gtest/gtest.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/internal/guicpp_port.h"
#include "include/guicpp_test_helper.h"

namespace guicpp_test {
using guicpp_test::TestBaseClass;

// This test is repeated in injector_death_test.cc.
TEST(GuicppBuilderDeathTest, Create_FailsForTypesWithNoDefaultBinding) {
  scoped_ptr<guicpp::Injector> injector(guicpp_test::GetEmptyInjector());

  // An attempt to Get an instance of a type with no-default binding
  // (i.e. made injectable using GUICPP_INJECTABLE macro).
  // Note: injector->Get() in turn calls Create() of *Builder class.
  EXPECT_DEATH(injector->Get<TestBaseClass*>(), "can not be instantiated");
}

}  // nameless namespace

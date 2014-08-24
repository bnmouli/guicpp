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


// To test if string and can be bound and injected

#include "guicpp/guicpp_strings.h"

#include <string>

#include "include/gtest/gtest.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_binder.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/guicpp_module.h"

namespace guicpp_test {

class StringsTestModule : public guicpp::Module {
 public:
  void Configure(guicpp::Binder* binder) const {
    binder->BindToValue< ::std::string>("Value of ::std::string");
    binder->BindToValue< ::std::wstring>(L"Value of ::std::wstring");
  }
};

TEST(GuicppStringsTest, StringsCanBeBoundAndInjected) {
  StringsTestModule module;
  scoped_ptr<guicpp::Injector> injector(guicpp::Injector::Create(&module));

  EXPECT_EQ(injector->Get< ::std::string>(), "Value of ::std::string");
  EXPECT_EQ(injector->Get< ::std::wstring>(), L"Value of ::std::wstring");
}

}  // namespace guicpp_test

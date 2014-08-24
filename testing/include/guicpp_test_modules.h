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


#ifndef GUICPP_TEST_MODULES_H_
#define GUICPP_TEST_MODULES_H_

#include "guicpp/guicpp_module.h"

namespace guicpp_test {

// Empty module does no binding.
class EmptyModule: public guicpp::Module {
  virtual void Configure(guicpp::Binder* binder) const;
};

// This module binds TestSimpleInjectableClass to TestInjectableSubClass.
class TestSimpleInjectableClassModule: public guicpp::Module {
  virtual void Configure(guicpp::Binder* binder) const;
};

// This module binds TestBaseClass to TestSimpleInjectableClass.
class TestBaseClassModule: public guicpp::Module {
  virtual void Configure(guicpp::Binder* binder) const;
};

// Module binds TestBaseClass with TestLabelOne to TestSimpleInjectableClass.
class TestBaseClassLabelOneModule: public guicpp::Module {
  void Configure(guicpp::Binder* binder) const;
};

// Module binds:
//  TestBaseClass to TestSimpleInjectableClass
//  TestBaseClass with TestLabelOne to TestSimpleInjectableClass
//  TestBaseClass with TestLabelTwo to TestInjectableSubClass
class TestBaseClassMultiBindModule: public guicpp::Module {
  void Configure(guicpp::Binder* binder) const;
};

// Used by tests that requires Factory.
class TestTopLevelSubClassBindModule: public guicpp::Module {
 public:
  void Configure(guicpp::Binder* binder) const;
};

}  // namespace guicpp_test
#endif  // GUICPP_TEST_MODULES_H_

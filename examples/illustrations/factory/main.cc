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


#include <iostream>

#include "guicpp/guicpp_binder.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/guicpp_module.h"
#include "guicpp/guicpp_tools.h"
#include "factoryuser.h"


class RefExampleModule: public guicpp::Module {
 public:
  void Configure(guicpp::Binder* binder) const {
  }
};


int main(int argc, char *argv[]) {
  // Get an instance of module, this can just be a local variable.
  RefExampleModule module;

  // Create an injector
  guicpp::Injector* injector = guicpp::CreateInjector(&module);

  PointerArgumentFactory *factory = injector->Get<PointerArgumentFactory*>();
  ExampleArgType *arg = new ExampleArgType("Locally Created");
  FactoryArgUser *arguser = factory->Get(arg);
  std::cout << "String in argument: " << arguser->arg() << std::endl;
  std::cout << "String in object: " << arguser->obj() << std::endl;
  delete arguser;
  delete factory;


  // Must delete injector at the end.
  delete injector;
  return 0;
}

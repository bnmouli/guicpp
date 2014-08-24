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
#include "refarg.h"


class RefExampleModule: public guicpp::Module {
 public:
  void Configure(guicpp::Binder* binder) const {
    ExampleArgType *a1 = new ExampleArgType("Object created in module");
    binder->BindRefToPointed<const ExampleArgType>(a1, guicpp::DeletePointer());
  }
};


int main(int argc, char *argv[]) {
  // Get an instance of module, this can just be a local variable.
  RefExampleModule module;

  // Create an injector
  guicpp::Injector* injector = guicpp::CreateInjector(&module);

  // Use inject to create all the objects you need.
  ReferenceExample* refex = injector->Get<ReferenceExample*>();
  std::cout << "String in refex: " << refex->s() << std::endl;
  delete refex;

  ReferenceArgUserFactory *factory = injector->Get<ReferenceArgUserFactory*>();
  ExampleArgType arg("Locally created");
  ReferenceArgUser *refuser = factory->Get(arg);
  std::cout << "String in refuser: " << refuser->s() << std::endl;
  delete refuser;
  delete factory;


  // Must delete injector at the end.
  delete injector;
  return 0;
}

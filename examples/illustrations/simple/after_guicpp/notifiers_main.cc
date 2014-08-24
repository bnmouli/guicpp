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
#include "message_sender.h"
#include "notifiers.h"


class SmsNotifierModule: public guicpp::Module {
 public:
  void Configure(guicpp::Binder* binder) const {
    // No need to specify binding for ContactList. If binding not specified for
    // a type, Guic++ creates instances of the same type. In this case, case
    // Gui++ create an instace ContactList and uses it to create SmsNotifier;
    // which is exactly what we want.

    // SmsSender is an interface and we intend to use RealSmsSender. This
    // binding must be specified.
    binder->Bind<SmsSender, RealSmsSender>();
  }
};


int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cerr << "Usage:  " << argv[0]
              << "path/to/conatct/table/contact-table.csv "
                 "<nickname> <message> " << std::endl;
    return 1;
  }


  // Get an instance of module, this can just be a local variable.
  SmsNotifierModule module;

  // Create an injector
  guicpp::Injector* injector = guicpp::CreateInjector(&module);

  // Use inject to create all the objects you need.
  SmsNotifier* notifier = injector->Get<SmsNotifier*>();
  notifier->Init(argv[1]);

  notifier->Notify(argv[2], argv[3]);

  delete notifier;

  // Must delete injector at the end.
  delete injector;

  return 0;
}

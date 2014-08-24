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


// The module that binds all necessary stuff required for notifier server

#ifndef NFE_MODULE_H_
#define NFE_MODULE_H_

#include "common/notifier_port.h"

#include "guicpp/guicpp_provider.h"
#include "guicpp/guicpp_module.h"

namespace notifier_example {

class EmailSender;

class NotifyServerModule: public guicpp::Module {
 public:
  NotifyServerModule(int port, const string contacts_csv)
      : port_(port), contacts_csv_(contacts_csv)  {}

  ~NotifyServerModule() {}

  void Configure(guicpp::Binder* binder) const;
 private:
  int port_;
  string contacts_csv_;
};


// EmailSender does not have a constructor which can be used to create it. Instead,
// it has a static method called GetInstance() and users of EmailSender class are
// expected to call GetInstance() to get its instance.
//
// The only way Guic++ can create objects is by using "new" operator on some
// constructor; this can not be used in this case. The following provider uses
// GetInstance() to get instance of EmailSender. We bind EmailSender to this
// provider in NotifyServerModule (see nfe_module.cc) this will make Guic++ to
// use this provider for getting instances of EmailSender.
//
// A provider is implemented by inheriting the from AbstrctProvider<prototype-of-get>
// class. The Get() method can take necessary arguments required to create EmailSender.
// All arguments of Get() are injected by Guic++. In this example we don't need any
// arguments.
//
class EmailSenderProvider: public guicpp::AbstractProvider<EmailSender* ()> {
 public:
  EmailSenderProvider() {}
  virtual ~EmailSenderProvider() {}

  // This creates instance of EmailSender calling EmailSender::GetInstance().
  EmailSender* Get();
};

}  // namespace notifier_example

#endif  // NFE_MODULE_H_

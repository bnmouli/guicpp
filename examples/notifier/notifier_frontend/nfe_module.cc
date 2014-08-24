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

#include "notifier_frontend/nfe_module.h"

#include "common/notifier_port.h"

#include "contact_list/contact_list.h"
#include "notifier_frontend/nfe_request_handler.h"
#include "notifier_frontend/nfe_server.h"
#include "messengers/message_sender.h"
#include "messengers/notifiers.h"
#include "guicpp/guicpp_binder.h"
#include "guicpp/guicpp_singleton.h"

namespace notifier_example {

void NotifyServerModule::Configure(guicpp::Binder* binder) const {
  // Bindings SmsSender to RealSmsSender. An instance of RealSmsSender
  // is provided to all classes that takes argument of type "SmsSender*".
  binder->Bind<SmsSender, RealSmsSender>();

  // Binding Notifier
  //   We use SmsNotifier for sending high priority notifications and
  //   EmailNotifier for sending low priority notifications.
  binder->Bind<guicpp::At<HighPriorityNotifierLabel, Notifier>, SmsNotifier>();
  binder->Bind<guicpp::At<LowPriorityNotifierLabel, Notifier>, EmailNotifier>();

  // Binding non-pointer type "int" annotated with PortNumberLabel.
  //
  // The value of "port_" is passed to any class that takes
  // argument of type int annotated with PortNumberLabel.
  binder->BindToValue<guicpp::At<PortNumberLabel, int> >(port_);

  // Load contact information from file
  ContactList* contact_list = ContactList::Load(contacts_csv_);

  // Bind the created instance to ContactList type.
  binder->BindToInstance<ContactList>(contact_list, guicpp::DeletePointer());

  // Binding RealSmsSender to lazy singleton scope. RealSmsSender is created
  // first time when it is required. Same instance is used for successive
  // injections.
  //
  // Note : The ownership of singleton object is retained by Guic++.
  //        The classes that receives such type should not delete the same.
  //        In this example, SmsNotifier does not take ownership of Notifier.
  binder->BindToScope<RealSmsSender, guicpp::LazySingleton>();

  // EmailSender is created using a provider, as it can not be created
  // just by doing "new" operator (see nfe_module.h).
  EmailSenderProvider* provider = new EmailSenderProvider();

  // This binding tells Guic++ to use this provider to create instances of
  // EmailSender. Note: Guic++ takes ownership of provider.
  binder->BindToProvider<EmailSender>(provider, guicpp::DeletePointer());
}

// Custom creation of EmailSender, uses EmailSender::GetInstance() to create
// the same.
EmailSender* EmailSenderProvider::Get() {
  return EmailSender::GetInstance();
}

}  // namespace notifier_example

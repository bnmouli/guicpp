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


// Defines Notifier and its implementations EmailNotifier and SmsNotifier.
// Notifier is something that can be used to send small notifications to
// the user identified by nickname. Both EmailNotifier and SmsNotifier uses
// ContactList to get contact information from nickname. EmailNotifier uses
// EmailSender to send email and SmsNotifier uses SmsSender to send SMS.

#ifndef NOTIFIERS_H_
#define NOTIFIERS_H_

#include <string>

#include "common/notifier_port.h"
#include "guicpp/guicpp.h"

namespace notifier_example {

class ContactList;
class EmailSender;
class SmsSender;

// Notifier is something that can be used to send small notifications
// to the user identified by nickname. How to get contact information
// using nickname and the medium used to send message are left to the
// implementation.
class Notifier {
 public:
  virtual ~Notifier() {}

  virtual bool Notify(const string& nickname, const string& message) = 0;

 protected:
  Notifier() {}

 private:
  SIMPLE_DISALLOW_COPY_AND_ASSIGN(Notifier);
};

GUICPP_INJECTABLE(Notifier);


// Notifier that sends email and gets ContactInfo using ContactList.
class EmailNotifier: public Notifier {
 public:
  EmailNotifier(ContactList* contact_list, EmailSender* email_sender);
  virtual ~EmailNotifier();

  virtual bool Notify(const string& nickname, const string& message);

 private:
  ContactList* contact_list_;
  EmailSender* email_sender_;

  SIMPLE_DISALLOW_COPY_AND_ASSIGN(EmailNotifier);
};

GUICPP_INJECT_CTOR(EmailNotifier, (
    ContactList* contact_list, EmailSender* email_sender));


// Notifier that sends SMS and gets ContactInfo using ContactList.
class SmsNotifier: public Notifier {
 public:
  SmsNotifier(ContactList* contact_list, const SmsSender* sms_sender);
  virtual ~SmsNotifier();

  virtual bool Notify(const string& nickname, const string& message);

 private:
  ContactList* contact_list_;
  const SmsSender* sms_sender_;

  SIMPLE_DISALLOW_COPY_AND_ASSIGN(SmsNotifier);
};

GUICPP_INJECT_CTOR(SmsNotifier, (
    ContactList* contact_list, const SmsSender* sms_sender));

}  // namespace notifier_example

#endif  // NOTIFIERS_H_

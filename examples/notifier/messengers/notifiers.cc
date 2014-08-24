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

#include "messengers/notifiers.h"

#include "contact_list/contact_list.h"
#include "messengers/message_sender.h"

namespace notifier_example {
GUICPP_DEFINE(EmailNotifier);
GUICPP_DEFINE(SmsNotifier);

EmailNotifier::EmailNotifier(ContactList* contact_list, EmailSender* email_sender)
  : contact_list_(contact_list), email_sender_(email_sender) {}

EmailNotifier::~EmailNotifier() {}

bool EmailNotifier::Notify(const string& nickname, const string& message) {
  const ContactInfo* contact_info = contact_list_->GetContact(nickname);
  if (contact_info == NULL) {
    return false;
  }

  email_sender_->SendEmail(contact_info->email_id, message);
  return true;
}


SmsNotifier::SmsNotifier(ContactList* contact_list, const SmsSender* sms_sender)
  : contact_list_(contact_list), sms_sender_(sms_sender) {}

SmsNotifier::~SmsNotifier() {}

bool SmsNotifier::Notify(const string& nickname, const string& message) {
  const ContactInfo* contact_info = contact_list_->GetContact(nickname);
  if (contact_info == NULL) {
    return false;
  }

  sms_sender_->SendSMS(contact_info->mobile_number, message);
  return true;
}

}  // namespace notifier_example

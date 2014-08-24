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

#include "notifiers.h"

#include <string>
#include <iostream>
#include "contact_list.h"
#include "message_sender.h"

GUICPP_DEFINE(SmsNotifier);

SmsNotifier::SmsNotifier(ContactList* contact_list, SmsSender* sms_sender)
  :contact_list_(contact_list), sms_sender_(sms_sender) {}

SmsNotifier::~SmsNotifier() {
  delete contact_list_;
  delete sms_sender_;
}

void SmsNotifier::Init(const std::string& csv_file) {
  contact_list_->Load(csv_file);
}

bool SmsNotifier::Notify(const std::string& nickname,
                         const std::string& message) {
  const ContactInfo* contact_info = contact_list_->GetContact(nickname);
  if (contact_info == NULL) {
    std::cout << "Sending SMS failed, No contact info for: "
              << nickname << std::endl;
    return false;
  }

  sms_sender_->SendSMS(contact_info->mobile_number, message);
  return true;
}

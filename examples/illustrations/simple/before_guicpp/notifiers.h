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


// Defines SmsNotifier.

#ifndef NOTIFIERS_H_
#define NOTIFIERS_H_

#include <string>

class ContactList;
class SmsSender;

// SmsNotifier used to send SMS using nickname.
class SmsNotifier {
 public:
  SmsNotifier();
  virtual ~SmsNotifier();

  virtual void Init(const std::string& csv_file);

  // Picks mobile number from contact info in contact list and
  // sends SMS using sms_sender.
  virtual bool Notify(const std::string& nickname, const std::string& message);

 private:
  ContactList* contact_list_;
  SmsSender* sms_sender_;

  // Top prohibit copy and assign
  explicit SmsNotifier(const SmsNotifier& from);
  void operator=(const SmsNotifier& from);
};

#endif  // NOTIFIERS_H_

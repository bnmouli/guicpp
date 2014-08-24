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


// Methods in real/mock implementations of SmsSender and EmailSender.

#include "messengers/message_sender.h"

#include <string>

#include "common/notifier_port.h"

namespace notifier_example {
GUICPP_DEFINE(RealSmsSender);
GUICPP_DEFINE(MockSmsSender);

void RealSmsSender::SendSMS(const string& mobile_num, const string& message) const {
  SIMPLE_LOG(INFO) << "Sending SMS to[" << mobile_num
                   << "] message[" << message << "]";

  // Code to send SMS goes here...
}

void MockSmsSender::SendSMS(const string& mobile_num, const string& message) const {
  SIMPLE_LOG(INFO) << "Mock implementation, NO message sent to[" << mobile_num
                   << "] message[" << message << "]";

  // Do something that helps testing..
}

EmailSender EmailSender::singlton_;

void EmailSender::SendEmail(const string& email_id, const string& message) {
  SIMPLE_LOG(INFO) << "Sending email to[" << email_id
                   << "] message[" << message << "]";

  // Code to send email goes here...
}

}  // namespace notifier_example

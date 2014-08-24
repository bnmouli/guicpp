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


// Methods in real/mock implementations of SmsSender

#include "message_sender.h"

#include <string>
#include <iostream>

void RealSmsSender::SendSMS(const std::string& mobile_num,
                            const std::string& message) {
  std::cout << "Sending SMS to[" << mobile_num
            << "] message[" << message << "]" << std::endl;

  // Code to send SMS goes here...
}

void MockSmsSender::SendSMS(const std::string& mobile_num,
                            const std::string& message) {
  std::cout << "Mock implementation, NO message sent to[" << mobile_num
            << "] message[" << message << "]" << std::endl;

  // Do something that helps testing..
}

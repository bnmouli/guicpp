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


// Defines SmsSender, its real/mock implementation.

#ifndef MESSAGE_SENDER_H_
#define MESSAGE_SENDER_H_

#include <string>

#include "guicpp/guicpp.h"

class SmsSender {
 public:
  virtual ~SmsSender() {}

  virtual void SendSMS(const std::string& mobile,
                       const std::string& message) = 0;
 protected:
  SmsSender() {}

 private:
  // Top prohibit copy and assign
  explicit SmsSender(const SmsSender& from);
  void operator=(const SmsSender& from);
};

GUICPP_INJECTABLE(SmsSender);

class RealSmsSender: public SmsSender {
 public:
  RealSmsSender() {}
  virtual ~RealSmsSender() {}

  virtual void SendSMS(const std::string& mobile,
                       const std::string& message);
 private:
  // Top prohibit copy and assign
  explicit RealSmsSender(const RealSmsSender& from);
  void operator=(const RealSmsSender& from);
};

GUICPP_INJECT_CTOR(RealSmsSender, ());


class MockSmsSender: public SmsSender {
 public:
  MockSmsSender() {}
  virtual ~MockSmsSender() {}

  virtual void SendSMS(const std::string& mobile,
                       const std::string& message);
 private:
  // Top prohibit copy and assign
  explicit MockSmsSender(const MockSmsSender& from);
  void operator=(const MockSmsSender& from);
};

GUICPP_INJECT_CTOR(MockSmsSender, ());

#endif  // MESSAGE_SENDER_H_

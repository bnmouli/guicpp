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


// Defines SmsSender, its real/mock implementation and EmailSender.
// EmailSender has method GetInstance() which must be used to get
// its instance.

#ifndef MESSAGE_SENDER_H_
#define MESSAGE_SENDER_H_

#include <string>

#include "common/notifier_port.h"
#include "guicpp/guicpp.h"

namespace notifier_example {

class SmsSender {
 public:
  virtual ~SmsSender() {}
  virtual void SendSMS(const string& mobile, const string& message) const = 0;

 protected:
  SmsSender() {}

 private:
  SIMPLE_DISALLOW_COPY_AND_ASSIGN(SmsSender);
};

GUICPP_INJECTABLE(SmsSender);

class RealSmsSender: public SmsSender {
 public:
  RealSmsSender() {}
  virtual ~RealSmsSender() {}

  virtual void SendSMS(const string& mobile, const string& message) const;
 private:
  SIMPLE_DISALLOW_COPY_AND_ASSIGN(RealSmsSender);
};

GUICPP_INJECT_CTOR(RealSmsSender, ());


class MockSmsSender: public SmsSender {
 public:
  MockSmsSender() {}
  virtual ~MockSmsSender() {}
  virtual void SendSMS(const string& mobile, const string& message) const;

 private:
  SIMPLE_DISALLOW_COPY_AND_ASSIGN(MockSmsSender);
};

GUICPP_INJECT_CTOR(MockSmsSender, ());

// The constructor of EmailSender is private and hence can't be used to
// instantiate it; instead, it provides a method called GetInstance()
// which must be used to get its instance.
//
// This class used to show how to use Guic++ for classes that can't be
// created by using "new" operator.
class EmailSender {
 public:
  ~EmailSender() {}
  void SendEmail(const string& email_id, const string& message);

  // This is a simple way of implementing singleton.
  static EmailSender* GetInstance() {
    return &singlton_;
  }

 private:
  EmailSender() {}
  static EmailSender singlton_;  // Singleton instance of EmailSender.

  SIMPLE_DISALLOW_COPY_AND_ASSIGN(EmailSender);
};

GUICPP_INJECTABLE(EmailSender);

}  // namespace notifier_example

#endif  // MESSAGE_SENDER_H_

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


#ifndef REFARG_H_
#define REFARG_H_

#include "guicpp/guicpp.h"
#include "guicpp/guicpp_factory.h"
#include <string>

class ExampleArgType {
 public:
  ExampleArgType(): s_("DefaultCtor") {}
  ExampleArgType(const std::string& str): s_(str) {}

  std::string s() const { return s_; }

 private:
  std::string s_;
};

GUICPP_INJECT_INLINE_CTOR(ExampleArgType, ());

class ReferenceExample {
 public:
  ReferenceExample(const ExampleArgType& arg): s_(arg.s()) {}

  std::string s() const { return s_; }

 private:
  std::string s_;
};

GUICPP_INJECT_INLINE_CTOR(ReferenceExample, (const ExampleArgType& arg));

class ReferenceArgUser {
 public:
  ReferenceArgUser(const ExampleArgType& arg): s_(arg.s()) {}

  std::string s() const { return s_; }

 private:
  std::string s_;
};

GUICPP_INJECT_INLINE_CTOR(ReferenceArgUser, (
    guicpp::At<guicpp::Assisted, const ExampleArgType&> arg));


class ReferenceArgUserFactory: public guicpp::Factory<
          ReferenceArgUser* (ExampleArgType& arg)> {};

#endif  // REFARG_H_

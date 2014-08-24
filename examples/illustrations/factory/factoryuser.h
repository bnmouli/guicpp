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

// This example is to show example of a class that takes one assisted argument
// and one injected argument.
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

// This example is to show example of a class that takes one assisted argument
// and one injected argument.

#ifndef FACTORYUSER_H_
#define FACTORYUSER_H_

#include "guicpp/guicpp.h"
#include "guicpp/guicpp_factory.h"
#include <string>

class SimpleClass {
 public:
  SimpleClass(): s_("DefaultCtor") {}
  SimpleClass(const std::string& str): s_(str) {}

  std::string s() const { return s_; }

 private:
  std::string s_;
};

GUICPP_INJECT_INLINE_CTOR(SimpleClass, ());

class ExampleArgType {
 public:
  ExampleArgType(): s_("DefaultCtor") {}
  ExampleArgType(const std::string& str): s_(str) {}

  std::string s() const { return s_; }

 private:
  std::string s_;
};

GUICPP_INJECT_INLINE_CTOR(ExampleArgType, ());

class FactoryArgUser {
 public:
  FactoryArgUser(SimpleClass* object, ExampleArgType* arg):
    object_(object), arg_(arg) {}

  ~FactoryArgUser() {
    delete arg_;
    delete object_;
  }

  std::string arg() const { return arg_->s(); }
  std::string obj() const { return object_->s(); }

 private:
  SimpleClass* object_;
  ExampleArgType* arg_;
};

GUICPP_INJECT_INLINE_CTOR(FactoryArgUser, (
    SimpleClass *object,
    guicpp::At<guicpp::Assisted, ExampleArgType*> arg));


// Note factory only takes arguments that needs to be injected as assisted
// Guic++ takes care of all other arguments.
class PointerArgumentFactory: public guicpp::Factory<
          FactoryArgUser* (ExampleArgType* arg)> {};

#endif  // FACTORYUSER_H_

#ifndef FACTORYUSER_H_
#define FACTORYUSER_H_

#include "guicpp/guicpp.h"
#include "guicpp/guicpp_factory.h"
#include <string>

class SimpleClass {
 public:
  SimpleClass(): s_("DefaultCtor") {}
  SimpleClass(const std::string& str): s_(str) {}

  std::string s() const { return s_; }

 private:
  std::string s_;
};

GUICPP_INJECT_INLINE_CTOR(SimpleClass, ());

class ExampleArgType {
 public:
  ExampleArgType(): s_("DefaultCtor") {}
  ExampleArgType(const std::string& str): s_(str) {}

  std::string s() const { return s_; }

 private:
  std::string s_;
};

GUICPP_INJECT_INLINE_CTOR(ExampleArgType, ());

class FactoryArgUser {
 public:
  FactoryArgUser(SimpleClass* object, ExampleArgType* arg):
    object_(object), arg_(arg) {}

  ~FactoryArgUser() {
    delete arg_;
    delete object_;
  }

  std::string arg() const { return arg_->s(); }
  std::string obj() const { return object_->s(); }

 private:
  SimpleClass* object_;
  ExampleArgType* arg_;
};

GUICPP_INJECT_INLINE_CTOR(FactoryArgUser, (
    SimpleClass *object,
    guicpp::At<guicpp::Assisted, ExampleArgType*> arg));


// Note factory only takes arguments that needs to be injected as assisted
// Guic++ takes care of all other arguments.
class PointerArgumentFactory: public guicpp::Factory<
          FactoryArgUser* (ExampleArgType* arg)> {};

#endif  // FACTORYUSER_H_

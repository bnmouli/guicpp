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


// TableEntry and helper functions used by GUICPP_INJECT_* macros.

#ifndef GUICPP_BUILDER_H_
#define GUICPP_BUILDER_H_

#include "guicpp/internal/guicpp_port.h"
#include "guicpp/internal/guicpp_table.h"
#include "guicpp/internal/guicpp_types.h"
#include "guicpp/internal/guicpp_util.h"

#include "guicpp/internal/guicpp_create_helpers.h"

namespace guicpp {
class Injector;

namespace internal {
class LocalContext;

// Used by GUICPP_INJECT_CTOR
// Creates an instance of type T using the function passed as template argument.
template <typename T, T* (*CreateFp)(const Injector*, const LocalContext*)>
class BindToFunction: public TableEntry<T*> {
 public:
  BindToFunction() {}
  virtual ~BindToFunction() {}

  virtual T* Get(const Injector* injector,
                 const LocalContext* local_context) const {
    return CreateFp(injector, local_context);
  }

  virtual typename TableEntryBase::BindType GetBindType() const {
    return TableEntryBase::BIND_TO_CTOR;
  }
};

class MacrosHelper {
 private:
  template <typename T>
  static T* CallExternFunction(const Injector* injector,
                               const LocalContext* local_context) {
    return GuicppCreateInstanceUsingCtor(TypeKey<T>(), injector, local_context);
  }

  template <typename T, typename CtorFp>
  static T* InlineCreateFunction(const Injector* injector,
                                 const LocalContext* local_context) {
    return CreateHelpers::Create(injector, local_context, TypeKey<CtorFp>());
  }

 public:
  // Used by GUICPP_INJECT_CTOR
  template <typename T>
  struct BindToExternFp {
    typedef BindToFunction<T, CallExternFunction<T> > Type;
  };

  // Unsed by GUICPP_TEMPLATE_INJECT_CTOR
  template <typename T, typename CtorFp>
  struct BindToInlineFp {
    typedef BindToFunction<T, InlineCreateFunction<T, CtorFp> > Type;
  };

  // Used by GUICPP_DEFINE
  template <typename T, typename CtorFp>
  static T* ExternCreateHelper(TypeKey<CtorFp>,
                               const Injector* injector,
                               const LocalContext* local_context) {
    return InlineCreateFunction<T, CtorFp>(injector, local_context);
  }

 private:
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(MacrosHelper);
};

}  // namespace internal
}  // namespace guicpp

#endif  // GUICPP_BUILDER_H_

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


// This file defines Injector class. Injector maintains bindings in memory
// and provides APIs to instantiate objects.

#ifndef GUICPP_INJECTOR_H_
#define GUICPP_INJECTOR_H_

#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_annotations.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/internal/guicpp_inject_util.h"
#include "guicpp/internal/guicpp_factory_types.h"
#include "guicpp/internal/guicpp_local_context.h"
#include "guicpp/internal/guicpp_types.h"
#include "guicpp/internal/guicpp_util.h"

namespace guicpp {
class Module;

// Injector class holds all bind information in memory (in bind_table)
// and provides APIs to Get/Create instances.
//
// Implementation:
//   Most of the logic is implemented in InjectorUtil to keep this class small.
class Injector: public internal::InternalType {
 public:
  ~Injector();  // No class should inherit from Injector.

  // Get<T>() returns an instance of a type that is implicitly convertible to
  // the type T (such as pointer to a sub-class), according to bindings done
  // in the module which is used for creating the Injector. The type T can be
  // annotated with labels. In that case, labels are used for bind table
  // lookup and return type of Get() is the actual-type.
  template <typename T>
  typename internal::AtUtil::GetTypes<T>::ActualType Get() const;

  // WARNING: DO NOT USE THIS DIRECTLY.
  // Use guicpp::CreateInjector() declared in tools.h.
  //
  // This is used to create an Injector having all the bindings specified
  // in the module. This will call module->Configure().
  static Injector* Create(const Module* module);

 private:
  // Injector assumes ownership of bind_table.
  explicit Injector(internal::BindTable* bind_table);

  friend class internal::InjectorUtil;
  friend class GuicppBinderTest;
  scoped_ptr<internal::BindTable> bind_table_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(Injector);
};


// -- Implementation --

template <typename T>
typename internal::AtUtil::GetTypes<T>::ActualType Injector::Get() const {
  internal::LocalContext local_context;
  internal::InjectorUtil inject_util(this);
  return inject_util.GetWithContext<T>(&local_context);
}

}  // namespace guicpp

#endif  // GUICPP_INJECTOR_H_

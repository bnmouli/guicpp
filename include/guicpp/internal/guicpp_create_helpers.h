// This file was GENERATED by command:
//     pump.py guicpp_create_helpers.h.pump
// DO NOT EDIT BY HAND!!!

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


// Helper functions used for creating an instance of user defined class.

#ifndef GUICPP_CREATE_HELPERS_H_
#define GUICPP_CREATE_HELPERS_H_

#include "guicpp/internal/guicpp_port.h"
#include "guicpp/internal/guicpp_inject_util.h"
#include "guicpp/internal/guicpp_types.h"

namespace guicpp {
namespace internal {
class LocalContext;

// These helper functions invoke the appropriate constructor after
// instantiating all of the target object's dependencies.
class CreateHelpers {
 public:
  template <typename T>
  static T* Create(const Injector* injector,
                   const LocalContext* local_context,
                   TypeKey<T* (*)()> fp) {
    return new T();
  }

  template <typename T, typename A1>
  static T* Create(const Injector* injector,
                   const LocalContext* local_context,
                   TypeKey<T* (*)(A1)> fp) {
    InjectorUtil inject_util(injector);
    return new T(
      inject_util.GetWithContext<A1>(local_context));
  }

  template <typename T, typename A1, typename A2>
  static T* Create(const Injector* injector,
                   const LocalContext* local_context,
                   TypeKey<T* (*)(A1, A2)> fp) {
    InjectorUtil inject_util(injector);
    return new T(
      inject_util.GetWithContext<A1>(local_context),
      inject_util.GetWithContext<A2>(local_context));
  }

  template <typename T, typename A1, typename A2, typename A3>
  static T* Create(const Injector* injector,
                   const LocalContext* local_context,
                   TypeKey<T* (*)(A1, A2, A3)> fp) {
    InjectorUtil inject_util(injector);
    return new T(
      inject_util.GetWithContext<A1>(local_context),
      inject_util.GetWithContext<A2>(local_context),
      inject_util.GetWithContext<A3>(local_context));
  }

  template <typename T, typename A1, typename A2, typename A3, typename A4>
  static T* Create(const Injector* injector,
                   const LocalContext* local_context,
                   TypeKey<T* (*)(A1, A2, A3, A4)> fp) {
    InjectorUtil inject_util(injector);
    return new T(
      inject_util.GetWithContext<A1>(local_context),
      inject_util.GetWithContext<A2>(local_context),
      inject_util.GetWithContext<A3>(local_context),
      inject_util.GetWithContext<A4>(local_context));
  }

  template <typename T, typename A1, typename A2, typename A3, typename A4,
      typename A5>
  static T* Create(const Injector* injector,
                   const LocalContext* local_context,
                   TypeKey<T* (*)(A1, A2, A3, A4, A5)> fp) {
    InjectorUtil inject_util(injector);
    return new T(
      inject_util.GetWithContext<A1>(local_context),
      inject_util.GetWithContext<A2>(local_context),
      inject_util.GetWithContext<A3>(local_context),
      inject_util.GetWithContext<A4>(local_context),
      inject_util.GetWithContext<A5>(local_context));
  }

  template <typename T, typename A1, typename A2, typename A3, typename A4,
      typename A5, typename A6>
  static T* Create(const Injector* injector,
                   const LocalContext* local_context,
                   TypeKey<T* (*)(A1, A2, A3, A4, A5, A6)> fp) {
    InjectorUtil inject_util(injector);
    return new T(
      inject_util.GetWithContext<A1>(local_context),
      inject_util.GetWithContext<A2>(local_context),
      inject_util.GetWithContext<A3>(local_context),
      inject_util.GetWithContext<A4>(local_context),
      inject_util.GetWithContext<A5>(local_context),
      inject_util.GetWithContext<A6>(local_context));
  }

  template <typename T, typename A1, typename A2, typename A3, typename A4,
      typename A5, typename A6, typename A7>
  static T* Create(const Injector* injector,
                   const LocalContext* local_context,
                   TypeKey<T* (*)(A1, A2, A3, A4, A5, A6, A7)> fp) {
    InjectorUtil inject_util(injector);
    return new T(
      inject_util.GetWithContext<A1>(local_context),
      inject_util.GetWithContext<A2>(local_context),
      inject_util.GetWithContext<A3>(local_context),
      inject_util.GetWithContext<A4>(local_context),
      inject_util.GetWithContext<A5>(local_context),
      inject_util.GetWithContext<A6>(local_context),
      inject_util.GetWithContext<A7>(local_context));
  }

  template <typename T, typename A1, typename A2, typename A3, typename A4,
      typename A5, typename A6, typename A7, typename A8>
  static T* Create(const Injector* injector,
                   const LocalContext* local_context,
                   TypeKey<T* (*)(A1, A2, A3, A4, A5, A6, A7, A8)> fp) {
    InjectorUtil inject_util(injector);
    return new T(
      inject_util.GetWithContext<A1>(local_context),
      inject_util.GetWithContext<A2>(local_context),
      inject_util.GetWithContext<A3>(local_context),
      inject_util.GetWithContext<A4>(local_context),
      inject_util.GetWithContext<A5>(local_context),
      inject_util.GetWithContext<A6>(local_context),
      inject_util.GetWithContext<A7>(local_context),
      inject_util.GetWithContext<A8>(local_context));
  }

  template <typename T, typename A1, typename A2, typename A3, typename A4,
      typename A5, typename A6, typename A7, typename A8, typename A9>
  static T* Create(const Injector* injector,
                   const LocalContext* local_context,
                   TypeKey<T* (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9)> fp) {
    InjectorUtil inject_util(injector);
    return new T(
      inject_util.GetWithContext<A1>(local_context),
      inject_util.GetWithContext<A2>(local_context),
      inject_util.GetWithContext<A3>(local_context),
      inject_util.GetWithContext<A4>(local_context),
      inject_util.GetWithContext<A5>(local_context),
      inject_util.GetWithContext<A6>(local_context),
      inject_util.GetWithContext<A7>(local_context),
      inject_util.GetWithContext<A8>(local_context),
      inject_util.GetWithContext<A9>(local_context));
  }

  template <typename T, typename A1, typename A2, typename A3, typename A4,
      typename A5, typename A6, typename A7, typename A8, typename A9,
      typename A10>
  static T* Create(const Injector* injector,
                   const LocalContext* local_context,
                   TypeKey<T* (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9,
                       A10)> fp) {
    InjectorUtil inject_util(injector);
    return new T(
      inject_util.GetWithContext<A1>(local_context),
      inject_util.GetWithContext<A2>(local_context),
      inject_util.GetWithContext<A3>(local_context),
      inject_util.GetWithContext<A4>(local_context),
      inject_util.GetWithContext<A5>(local_context),
      inject_util.GetWithContext<A6>(local_context),
      inject_util.GetWithContext<A7>(local_context),
      inject_util.GetWithContext<A8>(local_context),
      inject_util.GetWithContext<A9>(local_context),
      inject_util.GetWithContext<A10>(local_context));
  }

 private:
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(CreateHelpers);
};

}  // namespace internal
}  // namespace guicpp

#endif  // GUICPP_CREATE_HELPERS_H_
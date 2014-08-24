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


// This file contains macros used to define providers.
//
// Use Case:
//  Providers are used for custom creation of objects. The only way Guic++ can
//  create the object is by using "new" operator on specified constructor and
//  injecting its arguments. This may not be possible for some types.
//
//  For example, consider EmailSender class, this class does not have a
//  constructor that can be used to create it; Instead, it provides a static
//  method called GetInstance() which should be used to get its instance.
//
//  In such cases, user can write their provider that creates the object in the
//  expected way (using EmailSender::GetInstance()) and ask Guic++ to use it.
//
// Usage:
//  1. Implement Provider by inheriting from AbstractProvider<prototype-of-get>
//     (i.e.  AbstractProvider<return-type-of-get-method (arguments-of-get)>)
//
//     return-type-of-get:
//        The return type of the Get method. Often it is the pointer to object
//        created by provider but return type can also be value types.
//
//     arguments-of-get-with-annotations:
//        is list of 0 or more arguments required to create the intended object.
//        These arguments may be annotated.
//
//     arguments-of-get-without-annotations:
//        is list of 0 or more arguments required to create the intended object.
//        The arguments are not annotated in this case.
//
//     The AbstractProvider<prototype-of-get> is equivalent to:
//      class EquivalentAbstractProvider {
//       public:
//        virtual ~EquivalentAbstractProvider() {}
//        virtual return-type-of-get Get(
//            arguments-of-get-without-annotations) = 0;
//
//       protected:
//        EquivalentAbstractProvider();
//
//       private:
//        ... some Guic++ internal methods ...
//      };
//
//
//     A typical implementation of provider would look like:
//      class ExampleProvider: public guicpp::AbstractProvider<
//            return-type-of-get (arguments-of-get-with-annotations)> {
//       public:
//        // Implement this Get method.
//        return-type-of-get Get(arguments-of-get-without-annotations);
//      };
//
//     Example:
//      class EmailSenderProvider:
//            public guicpp::AbstractProvider<EmailSender* ()> {
//       public:
//        EmailSenderProvider() {}
//        ~EmailSenderProvider() {}
//
//        EmailSender* Get() {
//          return EmailSender::GetInstance();
//        }
//      };
//
//
//  2. Bind the type to provider using Binder::BindToProvider()
//
//     Example:
//      EmailSender needs to be created using this provider hence we
//      bind EmailSender to EmailSenderProvider.
//
//        binder->BindToProvider<EmailSender>(new EmailSenderProvider());
//
//     Guic++ will call Provider's Get() method by injecting its arguments.
//
// Note:
//   Caveats: All argument types of ProviderType::Get() must be injectable at
//   the point where Provider's constructor is instantiated.
//
// Implementation Detail:
//   GetInvoker iin ProviderGet requires all arguments of Get method to be
//   injectable, that means at the point where it is instantiated user must
//   look at all arguments of Get() and include all those headers which make
//   them injectable.
//
//   get_invoker_fp_ is a pointer to GetInvoker which is initialized in
//   constructor. That makes GetInvoker to be instantiated only where the
//   constructor is instantiated. This minimizes the number places where
//   Get's arguments needs to be injectable.

#ifndef GUICPP_PROVIDER_H_
#define GUICPP_PROVIDER_H_

#include "guicpp/guicpp_annotations.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/internal/guicpp_provider_types.h"
#include "guicpp/internal/guicpp_types.h"

#include "guicpp/internal/guicpp_provider_helpers.h"

namespace guicpp {
namespace internal {
template <typename R, typename ProviderType, typename CleanupAction>
class BindToProviderEntry;
}

// Abstract Get method is declared in ProviderGet class, AbstractProvider does
// not implement the method hence remains an abstract class.
template <typename T>
class AbstractProvider : public internal::ProviderGet<T> {
 protected:
  AbstractProvider(): get_invoker_fp_(&AbstractProvider::GetInvoker) {}

 private:
  typename AbstractProvider::ReturnType InvokeGet(
      const Injector* injector, const internal::LocalContext* local_context) {
    get_invoker_fp_(injector, local_context, this);
  }

  // The following classes uses InvokeGet() method.
  template <typename R, typename ProviderType, typename CleanupAction>
  friend class internal::BindToProviderEntry;
  friend class GuicppProviderTest;

  typename AbstractProvider::ReturnType(* const get_invoker_fp_) (
      const Injector* injector,
      const internal::LocalContext* local_context,
      internal::ProviderGet<T>* provider);
};

}  // namespace guicpp

#endif  // GUICPP_PROVIDER_H_

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


// This file defines:
//  * Label : Base class for user defined labels.
//  * Assisted : Used to annotate an argument whose value should be
//               taken from a factory parameter.

#ifndef GUICPP_ANNOTATIONS_H_
#define GUICPP_ANNOTATIONS_H_

#include "guicpp/internal/guicpp_port.h"
#include "guicpp/internal/guicpp_types.h"

namespace guicpp {
namespace internal {
class NormalInject;
class InjectorUtil;
class NotLabelled;

// Annotation base class used to determine the kind of injection to be done.
// Possible values are:
//   * guicpp::internal::NormalInject : Lookup done in injector bind-table;
//                       if not bound, uses default binding.
//   * guicpp::Assisted : Lookup done on factory parameter list.
//
class InjectType {  // Type of injection.
 public:
  typedef InjectType AnnotationType;
  typedef NormalInject AnnotationDefaultValue;

 private:
  GUICPP_DISALLOW_COPY_AND_ASSIGN_(InjectType);
};

}  // namespace internal

// Labels are used to have more than one binding to the same type depending
// on the context/purpose. This is referred to as multibinding.
// Usage:
//   * The user can define a label by defining an empty class and inheriting
//     it from Label.
//
//   * Once a label is created it can be used as an annotation.
//
// Example:
//   * Defining label
//     class HighPriorityNotifierLabel : public guicpp::Label {};
//
//   * Using it to annotate argument
//     GUICPP_INJECT_CTOR(NotifyRequestHandler, (
//         At<HighPriorityNotifierLabel, Notifier*> high_priority_notifier,
//         At<LowPriorityNotifierLabel, Notifier*> low_priority_notifier));
//
//   * Specifying binding
//     binder->Bind<At<HighPriorityNotifierLabel, Notifier>, SmsNotifier>();
//     binder->Bind<At<LowPriorityNotifierLabel, Notifier>, EmailNotifier>();
//
//   high_priority_notifier : Gets an instance of SmsNotifier
//   low_priority_notifier : Gets an instance of EmailNotifier
class Label {
 public:
  typedef Label AnnotationType;
  typedef internal::NotLabelled AnnotationDefaultValue;

 protected:
  // We put the constructor here without the definition in order to prevent
  // users from accidentally instantiating label classes derived from Label.
  Label();

 private:
  GUICPP_DISALLOW_COPY_AND_ASSIGN_(Label);
};

// Assisted is used to annotate an argument whose value comes from a parameter
// of a factory. The arguments that are annotated with "Assisted" are called
// "assisted arguments". See comments in factory.h for more information.
// Assisted arguments may also have labels, in such cases the order of label
// and Assisted annotation does not matter.
//
// Example:
//   GUICPP_INJECT_CTOR(NotifyRequestHandler, (
//       guicpp::At<guicpp::Assisted, HttpRequest*> request,
//       guicpp::At<guicpp::Assisted, ForWritingHttpResponse,
//                  OutputStream*> response));
//
//  Changing the order of Assisted annotation and label does not matter; hence,
//  the following statement is same as the one above.
//
//   GUICPP_INJECT_CTOR(NotifyRequestHandler, (
//       guicpp::At<guicpp::Assisted, HttpRequest*> request,
//       guicpp::At<ForWritingHttpResponse, guicpp::Assisted,
//                  OutputStream*> response));
class Assisted: public internal::InjectType {
 private:
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(Assisted);
};


namespace internal {
// The default value for Label.
class NotLabelled: public Label {};

class NormalInject: public InjectType {
 private:
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(NormalInject);
};

}  // namespace internal
}  // namespace guicpp

#endif  // GUICPP_ANNOTATIONS_H_

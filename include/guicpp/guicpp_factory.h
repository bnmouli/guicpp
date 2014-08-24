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


// This file defines macros and classes that are used to define factories.
//
// Use Case:
//  The factories are used for creating objects at runtime. These objects
//  can (optionally) take arguments whose values are known only at runtime.
//
//  Example: consider this piece of code that does not use Guic++:
//    NotifyRequestHandler* handler = new NotifyRequestHandler(
//        http_request, http_response, sms_sender);
//
//  This creates one instance of NotifyRequestHandler to handle each http
//  request. Many instances of NotifyRequestHandler are created during the
//  life time of the process. The value of http_request is known only at
//  runtime.
//
//  The Guic++ factories are designed to create such objects. The objects
//  created at runtime can take:
//     * Arguments whose values are known only at runtime.
//       Such as http_request : The request object received from net.
//               The values of this argument is known only at run time.
//
//     * Arguments which can be bound at initialization.
//       Such as sms_sender : An interface used by NotifyRequestHandler to
//               send SMSs. SmsSender type can be bound to an implementation
//               type in advance
//
//  In the example, NotifyRequestHandler take both kind of arguments.
//
// Usage:
//  guicpp::Factory lets you define a factory interface that takes the
//  necessary runtime parameters.
//
//  The following steps describe the use of factory.
//
//  1. Annotate arguments expected to be injected from factory parameters
//     (a.k.a runtime arguments) with "guicpp::Assisted".
//
//     Example:
//     NotifyRequestHandler expects "request" and "response" to come from
//     factory parameters.
//
//      GUICPP_INJECT_CTOR(NotifyRequestHandler, (
//          guicpp::At<guicpp::Assisted, HttpRequest*> request,
//          guicpp::At<guicpp::Assisted,
//                     ClientResponseLabel, OutputStream*> response,
//          SmsSender* sms_sender));
//
//  2. Define a factory interface class.
//     A factory interface class is an empty class inherited from
//     guicpp::Factory<prototype-of-its-Get-method> which is same as:
//     guicpp::Factory<return-type-of-get-method (arguments-of-get-method)>
//
//     The arguments of function prototype can be annotated. return-type must
//     not be annotated.
//
//     Example:
//      class NotifyRequestHandlerFactory: public guicpp::Factory<
//            NotifyRequestHandler* (
//                HttpRequest* param_req,
//                guicpp::At<ClientResponseLabel, OutputStream*> param_resp)> {
//        // This is an empty class. Do not define any methods here.
//      };
//
//     The name of the interface class can be anything, but we follow the
//     convention "<return-type>Factory".
//
//     The abstract method "Get()" is declared in guicpp::Factory making
//     the above class is equivalent to:
//
//      class NotifyRequestHandlerFactoryEquivalent {
//       public:
//        virtual ~NotifyRequestHandlerFactoryEquivalent() {}
//
//        // Note annotations on arguments are removed.
//        virtual NotifyRequestHandler* Get(
//            HttpRequest* param_req, OutputStream* param_resp) const;
//
//       protected:
//        NotifyRequestHandlerFactoryEquivalent() {}
//      };
//
//  3. In the class that needs to create objects at runtime, take the factory
//     as a constructor argument and store it as a class attribute.
//     Guic++ will automatically creates an implementation of the factory.
//
//     Example:
//      class NotifyRequestDispatcher: public Dispatcher {
//       public:
//        NotifyRequestDispatcher(NotifyRequestHandlerFactory* factory)
//          : req_handler_factory_(factory) {
//        }
//
//        void Dispatch(HttpRequest* request, OutputStream* response);
//
//       private:
//        scoped_ptr<HttpRequestHandlerfactory> req_handler_factory_;
//      };
//
//     GUICPP_INJECT_CTOR(NotifyRequestDispatcher, (
//         NotifyRequestHandlerFactory* factory));
//
//     Note: Factory ownership is transferred to class through the constructor
//     (in the above example, NotifyRequestDispatcher will own the factory).
//
//  4. Use it to create objects at runtime
//     Example:
//      void RequestDispatcher::Dispatch(
//          HttpRequest* request, OutputStream* response);
//        ...
//        HttpRequestHandler* handler =
//            req_handler_factory_->Get(request, response);
//        ...
//      }
//
// The object creation:
//  The factory combines its parameters with injector and uses it to create
//  the object bound to its return-type and also all its direct and indirect
//  dependencies.
//
//  Example:
//  Lets consider following three arguments of NotifyRequestHandler:
//   GUICPP_INJECT_CTOR(NotifyRequestHandler, (
//       guicpp::At<guicpp::Assisted, HttpRequest*> request,
//       guicpp::At<guicpp::Assisted,
//                  ClientResponseLabel, OutputStream*> response,
//       OutputStream* logger));
//
//  * Guic++ factory looks at binding for NotifyRequestHandler
//    - in this case, there is no explicit binding for NotifyRequestHandler
//      and since classes are implicitly bound to self, it goes ahead and
//      starts creating an instance of type NotifyRequestHandler.
//
//  * "request" and "response" arguments of NotifyRequestHandler are annotated
//     with "Assisted" and hence they are injected from factory parameter.
//     - "param_req" parameter of Get() and "request" argument of
//       NotifyRequestHandler's constructor are of same type and both have
//       no labels, hence value of "param_req" is passed as "request".
//     - "param_resp" parameter of Get() and "response" argument of
//       NotifyRequestHandler's constructor are of same type and both have
//       no labels, hence value of "param_resp" is passed as "response".
//
//  * "logger" is also of type OutputStream, but it is not annotated with
//    "Assisted"; Hence, it is injected by injector.
//
//    In the example OutputStream is an abstract class, it is bound to
//    LogOutputStream in a module as follows:
//      binder->Bind<OutputStream, LogOutputStream>();
//
//    Hence, an instance of LogOutputStream is created and passed as "logger".
//
//  LogOutputStream is defined as follows:
//
//   class LogOutputStream: public OutputStream {
//    public:
//     // "request" used only for logging.
//     LogOutputStream(HttpRequest* request) {}
//     virtual ~LogOutputStream() {}
//     ...
//    private:
//     ...
//   };
//
//  GUICPP_INJECT_CTOR(LogOutputStream, (
//      guicpp::At<guicpp::Assisted, HttpRequest*> request));
//
//  As mentioned earlier, all direct and indirect dependencies are created by
//  combining factory-parameters and injector. Hence, it is perfectly valid
//  for LogOutputStream to take "Assisted" arguments. The value of "param_req"
//  is used as "request" to create LogOutputStream.

#ifndef GUICPP_FACTORY_H_
#define GUICPP_FACTORY_H_

#include "guicpp/guicpp_annotations.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/internal/guicpp_factory_types.h"
#include "guicpp/internal/guicpp_types.h"

#include "guicpp/internal/guicpp_factory_helpers.h"

namespace guicpp {
// This is the factory interface used declare factories.
//
// Usage:
//   A factory interface is simply an empty class inherited from
// guicpp::Factory<prototype-of-get>
//
// This can be thought as an interface equivalent to:
//  class FactoryEquivalent {
//   public:
//    virtual return-type-of-get Get(
//        arguments-of-get-method-without-annotations) const = 0;
//  };
//
//
// Example:
//   class NotifyRequestHandlerFactory: public guicpp::Factory<
//         NotifyRequestHandler* (
//             HttpRequest* param_req,
//             guicpp::At<ClientResponseLabel, OutputStream*> param_resp)> {
//     // This is an empty class. Do not define any methods here.
//   };
//
// Implementation Detail:
//   The specialized FactoryInterface has an abstract method called Get with
//   the same prototype as GetSignature without annotations. Factory class
//   inherits FactoryInterface and does not implement Get() which leaves it
//   as a pure abstract class (interface).
//
//   The specialized version of FactoryInterface is not included here (they
//   are in factory_helpers.h) as it leads to cyclic inclusion. The name
//   FactoryInterface is resolved at the time of instantiation and hence
//   will choose the appropriate specialized version of FactoryInterface.
template <typename GetSignature>
class Factory: public internal::FactoryInterface<GetSignature> {
 public:
  typedef GetSignature GuicppGetSignature;

  virtual ~Factory() {}

 protected:
  Factory() {}

 private:
  GUICPP_DISALLOW_COPY_AND_ASSIGN_(Factory);
};

}  // namespace guicpp

#endif  // GUICPP_FACTORY_H_

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


// Defines NotifyRequestHandler, the request handler of Notifier server.
// NotifyRequestHandler takes "request" as constructor argument and on
// call to its "Handle()" method, it processes the "request" and writes
// the output using AppendOutput() method in "request".
//
// This also defines NotifyRequestHandlerFactory, which can be used to
// create instances of NotifyRequestHandler at run time.

#ifndef NFE_REQUEST_HANDLER_H_
#define NFE_REQUEST_HANDLER_H_

#include "common/notifier_port.h"

#include "guicpp/guicpp_factory.h"
#include "guicpp/guicpp.h"

namespace notifier_example {
class HttpRequest;
class Notifier;
class Logger;

// Used to annotate Notifier used to send high priority messages
class HighPriorityNotifierLabel: public guicpp::Label {};

// Used to annotate Notifier used to send low priority messages
class LowPriorityNotifierLabel: public guicpp::Label {};

// This class is instantiated using NotifyRequestHandlerFactory.
// The arguments of the factory are available to this class.
class NotifyRequestHandler {
 public:
  // @param request : The request object received from net.
  //    This is an assisted argument and expected to be injected from factory
  //    parameter of same type.
  //
  // @param high_priority_notifier: used to send high priority messages to
  //    the user. This type is bound to SmsNotifier in NotifyServerModule.
  //
  // @param low_priority_notifier: used to send low priority messages to
  //    the user. This type is bound to EmailNotifier in NotifyServerModule.
  //
  // @param logger: Used for writing logs.
  NotifyRequestHandler(HttpRequest* request,
                       Notifier* high_priority_notifier,
                       Notifier* low_priority_notifier,
                       Logger* logger);
  ~NotifyRequestHandler();

  // Processes the query
  void Handle();

 private:
  HttpRequest* request_;
  scoped_ptr<Notifier> high_priority_notifier_;
  scoped_ptr<Notifier> low_priority_notifier_;
  scoped_ptr<Logger> logger_;
};

GUICPP_INJECT_CTOR(NotifyRequestHandler, (
    guicpp::At<guicpp::Assisted, HttpRequest*> request,
    guicpp::At<HighPriorityNotifierLabel, Notifier*> high_priority_notifier,
    guicpp::At<LowPriorityNotifierLabel, Notifier*> low_priority_notifier,
    Logger* logger));


// The factory interface that is used to instantiate NotifyRequestHandler.
// The interface class defined by this macro is equivalent to:
//
//  class NotifyRequestHandlerFactory {
//   public:
//    virtual ~NotifyRequestHandlerFactory() {}
//
//    virtual NotifyRequestHandler* Get(HttpRequest* param_req) const = 0;
//   protected:
//    NotifyRequestHandlerFactory() {}
//  };
//
// In this example, NotifyRequestHandler does not have any explicit binding.
// Since NotifyRequestHandler is a concrete class, it is implicitly bound to
// itself; Hence, NotifyRequestHandlerFactory::Get() creates an instance of
// NotifyRequestHandler and the value passed to this factory is used for
// assisted argument of NotifyRequestHandler (i.e. argument named "request")
// and all its direct and indirect dependencies, in this case logger.
//
// The argument "logger" of NotifyRequestHandler is of type "Logger" which also
// takes "HttpRequest" as a constructor argument. The value of factory parameter
// is used in this case as well. In other words, parameters of the factory are
// available not only to top-level object created; but also to all its direct
// and indirect dependencies.
class NotifyRequestHandlerFactory:
  public guicpp::Factory<NotifyRequestHandler* (HttpRequest* param_req)> {};

}  // namespace notifier_example

#endif  // NFE_REQUEST_HANDLER_H_

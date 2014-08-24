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


// Dispatcher that dispatches the request to HttpRequestHandler.
//
// NotifyServer implements notification server; a HTTP web server
// that can be used to send notification to person identified by
// a nickname.

#ifndef NFE_SERVER_H_
#define NFE_SERVER_H_

#include <string>

#include "common/notifier_port.h"
#include "http_server/public/server.h"
#include "guicpp/guicpp.h"

namespace notifier_example {
class NotifyRequestHandlerFactory;
class NotifyRequestDispatcher;

// Label used to annotate NotifyServer's port number.
class PortNumberLabel: public guicpp::Label {};

// NotifyServer initialized SimpleHttpServer server with port number
// and NotifyRequestDispatcher and calls SimpleHttpServer::StartServer()
//
// SimpleHttpServer then starts listening on the port and calls
// NotifyRequestDispatcher::Dispatch with received HTTP request.
//
// See ../http_server/public/server.h for details on SimpleHttpServer.
class NotifyServer {
 public:
  NotifyServer(SimpleHttpServer* server,
               NotifyRequestDispatcher* dispatcher,
               int port);
  ~NotifyServer();

  void StartServer();

 private:
  scoped_ptr<SimpleHttpServer> server_;
};

GUICPP_INJECT_CTOR(NotifyServer, (
    SimpleHttpServer* server,
    NotifyRequestDispatcher* dispatcher,
    guicpp::At<PortNumberLabel, int> port));


// NotifyRequestDispatcher dispatches HTTP request to NotifyRequestHandler.
class NotifyRequestDispatcher: public Dispatcher {
 public:
  explicit NotifyRequestDispatcher(NotifyRequestHandlerFactory* factory);
  ~NotifyRequestDispatcher();

  // Creates an instance of NotifyRequestHandler using
  // NotifyRequestHandlerFactory and calls its Handle() method.
  //
  // The value of "request" is passed to factory's Get() method and all created
  // objects can take it as an assisted argument.
  //
  // In this example, NotifyRequestHandler takes "request" and "Logger" also
  // takes "request" as constructor arguments; They are created by using the
  // value passed here.
  void Dispatch(HttpRequest* request) const;

 private:
  scoped_ptr<NotifyRequestHandlerFactory> req_handler_factory_;

  SIMPLE_DISALLOW_COPY_AND_ASSIGN(NotifyRequestDispatcher);
};


GUICPP_INJECT_CTOR(NotifyRequestDispatcher, (
    NotifyRequestHandlerFactory* factory));

// The logger class. Just logs what ever passed to Log()
// and appends "request" to each log which gives necessary
// context to anlyze the logs.
class Logger {
 public:
  // request is used only for logging.
  explicit Logger(HttpRequest* request): request_(request) {}
  ~Logger() {}

  void Log(const string& s);

 private:
  const HttpRequest* request_;
};

GUICPP_INJECT_CTOR(Logger, (
    guicpp::At<guicpp::Assisted, HttpRequest*> request));

}  // namespace notifier_example

#endif  // NFE_SERVER_H_

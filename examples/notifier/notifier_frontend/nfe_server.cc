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


// NotifyServer and NotifyRequestDispatcher classes.

#include "notifier_frontend/nfe_server.h"

#include <string>

#include "common/notifier_port.h"
#include "http_server/public/server.h"
#include "notifier_frontend/nfe_guicpp.h"
#include "notifier_frontend/nfe_request_handler.h"

namespace notifier_example {
GUICPP_DEFINE(NotifyServer);
GUICPP_DEFINE(NotifyRequestDispatcher);
GUICPP_DEFINE(Logger);

NotifyServer::NotifyServer(SimpleHttpServer* server,
                           NotifyRequestDispatcher* dispatcher,
                           int port): server_(server) {
  server_->Init(dispatcher, port);
}

NotifyServer::~NotifyServer() {}

void NotifyServer::StartServer() {
  server_->StartServer();
}

NotifyRequestDispatcher::NotifyRequestDispatcher(
    NotifyRequestHandlerFactory* factory): req_handler_factory_(factory) {
}

NotifyRequestDispatcher::~NotifyRequestDispatcher() {}

void NotifyRequestDispatcher::Dispatch(HttpRequest* request) const {
  // Get an instance of NotifyRequestHandler using NotifyRequestHandlerFactory.
  // The value of "request" is passed to the factory. Any object crated in
  // by the factory can take them as assisted argument.
  //
  // In this example, NotifyRequestHandler takes "request" and "Logger" also
  // takes "request" as constructor arguments; They are created by using the
  // value passed here.
  scoped_ptr<NotifyRequestHandler> handler(req_handler_factory_->Get(request));

  handler->Handle();
}

void Logger::Log(const string& s) {
  SIMPLE_LOG(INFO) << s << " [Request: " << request_->ToString() << "]";
}

}  // namespace notifier_example

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


// The main code for notifier server.

#include <cstdlib>

#include "common/notifier_port.h"

#include "guicpp/guicpp_injector.h"
#include "guicpp/guicpp_tools.h"
#include "notifier_frontend/nfe_module.h"
#include "notifier_frontend/nfe_server.h"

using notifier_example::NotifyServerModule;
using notifier_example::NotifyServer;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    SIMPLE_LOG(FATAL) << "Usage: nfe port configfilen\n";
  }

  int port = std::atoi(argv[1]);
  NotifyServerModule module(port, argv[2]);

  // Create an injector using Compile method.
  guicpp::Injector* injector = guicpp::CreateInjector(&module);

  // injector->Get<NotifyServer*>() creates an instance of NotifyServer.
  // Guic++ automatically creates all its direct and indirect dependencies.
  NotifyServer* server(injector->Get<NotifyServer*>());


  // NotifyServer::StartServer starts the server. This method returns
  // only when server is brought down by visiting "/quitquitquit".
  server->StartServer();

  // We own the objects created by Guic++
  delete server;

  // Delete the injector at the end of program.
  //
  // Note: The better option is to use scoped_ptr for Injector.
  // In this code however,
  //    guicpp::Injector* injector =  ...
  //
  // Better illustrates how injector is created.
  delete injector;
}

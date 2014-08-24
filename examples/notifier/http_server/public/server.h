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


// SimpleHttpServer used to implement simple HTTP servers. This library
// used to avoid "uninteresting" code from Guic++ examples. This code
// does not use Guic++, and treated as third party code. Hence you need
// not look at this code if you are are only interested in Guic++.
//
// Usage:
//   * Get an instance of SimpleHttpServer.
//   * Initialize it using Init() method with an implementation of Dispatcher
//     and port number.
//   * Call StartServer() method to start the server. This method returns
//     only when (and if) browser is pointed to "/quitquitquit".

#ifndef SERVER_H_
#define SERVER_H_

#include "common/notifier_port.h"

#include <string>

namespace notifier_example {
using std::string;
using std::map;

// This is a pared out version of HTTP request.
// To keep the code small, this has only CGI arguments
// and a method to write HTTP response.
class HttpRequest {
 public:
  ~HttpRequest() { }

  // Method to read CGI arguments.
  //
  // @param arg : name of the cgi argument.
  // @param value : value to be written.
  // Returns true if argument is present, value assigned to "value"
  // If requested argument is not present returns false and value is
  // unchanged.
  bool GetCgiArg(const string& arg, string* value) const;

  // Appends "str" to response string.
  void AppendOutput(const string& str);

  // Returns a human redable string that can be used for logging.
  string ToString() const;

 private:
  // SimpleHttpServer uses constructor to create
  // Populates cgi_args_map_
  // Sends output_ to client (browser).
  friend class SimpleHttpServer;
  HttpRequest() {}

  string output_;
  map<string, string> cgi_args_map_;

  SIMPLE_DISALLOW_COPY_AND_ASSIGN(HttpRequest);
};


// Interface used by Simple Http Server library to dispatch HTTP request.
class Dispatcher {
 public:
  virtual ~Dispatcher() {}

  // Dispatches http request to handler.
  virtual void Dispatch(HttpRequest* request) const = 0;

 protected:
  Dispatcher() {}
};


// Simple HTTP Server. This server:
//   * Listens to HTTP request on a port
//   * For each request it calls the dispatcher->Dispatch().
class SimpleHttpServer {
 public:
  SimpleHttpServer();
  ~SimpleHttpServer();

  // Call Init() before StartServer().
  void Init(Dispatcher* dispatcher, int port);

  // Starts the server, this function will never return.
  void StartServer();

 private:
  static bool ReadHttpRequest(char* buff, HttpRequest *request);
  static void WriteOutput(int outfd, const string& output);

  guicpp::scoped_ptr<Dispatcher> dispatcher_;
  int port_;
  char* buffer_;  // Don't want to depend on scoped_array
  bool stop_;

  SIMPLE_DISALLOW_COPY_AND_ASSIGN(SimpleHttpServer);
};

}  // namespace notifier_example

#endif  // SERVER_H_

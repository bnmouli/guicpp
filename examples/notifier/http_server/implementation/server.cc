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
// does not use Guic++, hence you need not look at this code if you are
// only interested in Guic++.

#include "http_server/public/server.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


namespace notifier_example {
static const int kBufferSize = 10000;

SimpleHttpServer::SimpleHttpServer(): port_(0), stop_(false) {
  buffer_ = new char[kBufferSize]();
  memset(buffer_, '\0', kBufferSize);
}

SimpleHttpServer::~SimpleHttpServer() {
  delete[] buffer_;
}

void SimpleHttpServer::Init(Dispatcher* dispatcher, int port) {
  dispatcher_.reset(dispatcher);
  port_ = port;
}

void SimpleHttpServer::StartServer() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  SIMPLE_CHECK(sockfd > 0);

  int reuse = 1;
  // This helps quickly re-start the server, though rarely may lead to
  // unexpected behavior.
  int err = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
  SIMPLE_CHECK(err == 0);

  struct sockaddr_in serv_addr;
  memset(&serv_addr, '\0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port_);
  SIMPLE_CHECK(
      bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0);

  listen(sockfd, 5);

  while (!stop_) {
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    SIMPLE_CHECK(newsockfd > 0);

    int rsize = read(newsockfd, buffer_, kBufferSize-1);
    if (rsize <= 0) {
      GUICPP_LOG_(INFO) << "Ignoring empty requests";
      close(newsockfd);
      continue;
    }

    GUICPP_LOG_(INFO) << "Here is the message: " << buffer_;

    HttpRequest request;
    if (ReadHttpRequest(buffer_, &request)) {
      dispatcher_->Dispatch(&request);
      WriteOutput(newsockfd, request.output_);
    } else {
      WriteOutput(newsockfd, "<h1>Parse Error!</h1>");
    }

    memset(buffer_, '\0', rsize);
    close(newsockfd);
  }

  close(sockfd);
}

static const char kHttpHeader[] =
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: keep-alive\r\n"
    "\r\n";

void SimpleHttpServer::WriteOutput(int outfd, const string& output) {
  GUICPP_LOG_(INFO) << "Output: " << output;

  int wsize = write(outfd, kHttpHeader, sizeof(kHttpHeader)-1);
  SIMPLE_CHECK(wsize == sizeof(kHttpHeader) -1);
  if (output.length() > 0) {
    wsize = write(outfd, output.c_str(), output.length());
    SIMPLE_CHECK(wsize == output.length());
  }
}

static const char kEndOfHeaders[] = "\r\n\r\n";

bool SimpleHttpServer::ReadHttpRequest(char* buff, HttpRequest *request) {
  // Get method
  char *end = strpbrk(buff, " ");
  if (!end) {
    GUICPP_LOG_(ERROR) << "Failed to get method";
    return false;
  }
  string method(buff, end - buff);

  char *cursor = NULL;

  if (method == "GET") {
    end = strchr(buff, '\r');
    if (end) *end = '\0';
    end = strchr(buff, '\n');
    if (end) *end = '\0';

    cursor = strchr(buff, '?');
    if (!cursor) return true;

    cursor += 1;
  } else if (method == "POST") {
    end = strstr(buff, kEndOfHeaders);
    if (!end) {
      GUICPP_LOG_(ERROR) << "Failed to find end of HTTP headers";
      return false;
    }

    cursor = end + sizeof(kEndOfHeaders) -1;
    if (*cursor == '\0') return true;
  } else {
    GUICPP_LOG_(ERROR) << "Method not supported: " << method;
    return false;
  }

  // Read arguments.
  end = strpbrk(cursor, " \r\n");
  if (end) *end = '\0';

  string key, value;
  while (cursor) {
    end = strchr(cursor, '&');
    if (end) {
      *end = '\0';
      end += 1;
    }

    char *sep = strchr(cursor, '=');
    if (sep) {
      key.assign(cursor, sep - cursor);
      value.assign(sep+1);
    } else {
      key.assign(cursor);
      value.assign("1");
    }

    GUICPP_LOG_(INFO) << "Key: " << key << " Value: " << value;
    request->cgi_args_map_[key] = value;

    cursor = end;
  }

  return true;
}

bool HttpRequest::GetCgiArg(const string& arg, string* value) const {
  std::map<string, string>::const_iterator iter = cgi_args_map_.find(arg);
  if (iter == cgi_args_map_.end()) {
    return false;
  }
  value->assign(iter->second);
  return true;
}

void HttpRequest::AppendOutput(const string& str) {
  output_.append(str);
}

string HttpRequest::ToString() const {
  string tostring;
  for (map<string, string>::const_iterator iter = cgi_args_map_.begin();
       iter != cgi_args_map_.end();
       ++iter) {
    tostring.append(iter->first);
    tostring.append(": ");
    tostring.append(iter->second);
    tostring.append("\n");
  }
  return tostring;
}

}  // namespace notifier_example

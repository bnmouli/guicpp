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

#include "notifier_frontend/nfe_request_handler.h"

#include <string>
#include "http_server/public/server.h"
#include "notifier_frontend/nfe_guicpp.h"
#include "notifier_frontend/nfe_server.h"
#include "messengers/notifiers.h"

namespace notifier_example {
GUICPP_DEFINE(NotifyRequestHandler);

static const char kInputNickName[] =
    "Nick name: <input type='text' name='nickname' /><br />";

static const char kInputMessage[] =
    "Message: <input type='text' name='message' /><br />";

static const char kInputPriority[] =
    "<select name='priority'>"
    "<option value='low'>Low</option>"
    "<option value='high'>High</option>"
    "</select> <br />";

static const char kSubmit[] =
    "<input type='submit' value='Submit' /> </form>";

// NotifyRequestHandler requires "request".
NotifyRequestHandler::NotifyRequestHandler(HttpRequest* request,
                                           Notifier* high_priority_notifier,
                                           Notifier* low_priority_notifier,
                                           Logger* logger)
  : request_(request),
    high_priority_notifier_(high_priority_notifier),
    low_priority_notifier_(low_priority_notifier),
    logger_(logger) {}

NotifyRequestHandler::~NotifyRequestHandler() {}

static string GetCgiArg(const HttpRequest* request, const string& arg) {
  string value;
  if (!request->GetCgiArg(arg, &value)) {
    // We do not handle errors yet.  In this code,
    // if argument not present we assume value to be ""
  }

  return value;
}

// Writes an http form as HTTP response.
// This method called both for rendering the initial page and
// when form is submitted. If nickname is set (which happens
// when form is submitted), it sends notification to "nickname"
// and renders status and form again.
void NotifyRequestHandler::Handle() {
  logger_->Log("NotifyRequestHandler::Handle() start");
  request_->AppendOutput(
      "<html><head><title>Notification Sender</title></head><body>");

  // If nickname is specified, send message to that person using notifier
  // and render the status.
  string nickname = GetCgiArg(request_, "nickname");
  if (nickname != "") {
    logger_->Log("Sending Notification");

    bool status = false;
    if (GetCgiArg(request_, "priority") == "high") {
      status = high_priority_notifier_->Notify(
          nickname, GetCgiArg(request_, "message"));
    } else {
      status = low_priority_notifier_->Notify(
          nickname, GetCgiArg(request_, "message"));
    }

    if (status) {
      logger_->Log("Notification Sent successfully");
      request_->AppendOutput(
          "<span style='color:blue'>Message Sent to " + nickname + "</span>");
    } else {
      logger_->Log("Sending Notification failed");
      request_->AppendOutput(
          "<span style='color:red'>Contact information not found for " +
          nickname + "</span>");
    }
  }

  // Write the html form.
  request_->AppendOutput("<br /><form action='/'>");
  request_->AppendOutput(kInputNickName);
  request_->AppendOutput(kInputMessage);
  request_->AppendOutput(kInputPriority);
  request_->AppendOutput(kSubmit);
  request_->AppendOutput("</form></body></html>");

  logger_->Log("NotifyRequestHandler::Handle() end");
}

}  // namespace notifier_example

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


// This header is used to make third party code injectable.
// In this case, classes of ../http_server.
//
// We always include this header along with third party headers.

#ifndef NFE_GUICPP_H_
#define NFE_GUICPP_H_

#include "guicpp/guicpp.h"

namespace notifier_example {
class HttpRequest;
class Dispatcher;
class SimpleHttpServer;
}  // namespace notifier_example

// These macros can also be invoked in notifier_example namespace.
// But as a convention, we always call these in global namespace
// for making "third party" code injectable.

GUICPP_INJECTABLE(notifier_example::HttpRequest);
GUICPP_INJECTABLE(notifier_example::Dispatcher);
GUICPP_INJECT_CTOR(notifier_example::SimpleHttpServer, ());

#endif  // NFE_GUICPP_H_

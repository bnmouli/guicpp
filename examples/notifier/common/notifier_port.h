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


#ifndef NOTIFIER_PORT_H_
#define NOTIFIER_PORT_H_

// Instead of writing another port file. We reuse things from guicpp
// guicpp/include/guicpp_port.h

#include <string>

#include "guicpp/internal/guicpp_port.h"

#define SIMPLE_DISALLOW_COPY_AND_ASSIGN(C) \
    C(const C&); \
    void operator=(const C&)

#define SIMPLE_LOG GUICPP_LOG_
#define SIMPLE_CHECK GUICPP_CHECK_

namespace notifier_example {
  using guicpp::scoped_ptr;
  using std::string;
}

#endif  // NOTIFIER_PORT_H_

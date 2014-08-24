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


// Note: Prefer including "guicpp.h" as in many cases
// you would other headers included in that file.
//
// Makes string and its variant injectable.

#ifndef GUICPP_STRINGS_H_
#define GUICPP_STRINGS_H_

#include <string>
#include "guicpp/guicpp_macros.h"

GUICPP_INJECTABLE(::std::string);
GUICPP_INJECTABLE(::std::wstring);

#endif  // GUICPP_STRINGS_H_

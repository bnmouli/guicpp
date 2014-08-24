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


// This file defines LocalContext which is used to pass some
// local context to all functions in a call tree.

#include "guicpp/internal/guicpp_local_context.h"

namespace guicpp {
namespace internal {

const TableEntryBase* LocalContext::FindEntry(TypeId tid) const {
  for (int i = 0; i < num_args_; ++i) {
    if (args_[i].type_id == tid) {
      return args_[i].entry;
    }
  }

  return NULL;
}

}  // namespace internal
}  // namespace guicpp

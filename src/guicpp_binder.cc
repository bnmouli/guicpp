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


// Definitions of methods declared Binder class.

#include "guicpp/guicpp_binder.h"

#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_module.h"
#include "guicpp/internal/guicpp_table.h"

namespace guicpp  {
Binder::Binder(internal::BindTable* bind_table)
    : bind_table_(bind_table), num_errors_(0) {
}

Binder::~Binder() {
}

// Adds entry to bind_table_ for tid.
void Binder::AddBindEntry(internal::TypeId tid,
                          const internal::TableEntryBase* entry) {
  // bind_table_ takes the ownership of the entry, it is deleted even if
  // AddEntry fails.
  if (!bind_table_->AddEntry(tid, entry)) {
    // TODO(bnmouli): ADDNAME Need to add GetName() method to TableEntryBase
    // and then change this to print meaning full error.
    GUICPP_LOG_(ERROR) << "Duplicate Binding: Type is already bound.";

    ++num_errors_;
  }
}

// Include bindings specified in module.
//
// Implementation detail:
//   We simply call other module's Configure method with this binder.
//
//   You may think of directly calling module->Configure(binder) instead of
//   binder->Install(module). DON'T DO IT. This implementation may change
//   in future.
void Binder::Install(const Module* module) {
  module->Configure(this);
}

}  // namespace guicpp

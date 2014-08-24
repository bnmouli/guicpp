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


// defines methods in Injector class.

#include "guicpp/guicpp_injector.h"

#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_binder.h"
#include "guicpp/internal/guicpp_local_context.h"
#include "guicpp/guicpp_module.h"
#include "guicpp/internal/guicpp_table.h"

namespace guicpp {
Injector::Injector(internal::BindTable* bind_table)
    : bind_table_(bind_table) {}

Injector::~Injector() {}

// This is used to create Injector having all the bindings specified
// in module. This will call module->Configure().
// static
Injector* Injector::Create(const Module* module) {
  // injector owns the bind_table.
  Injector* injector = new Injector(new internal::BindTable());

  // Binder does not own the bind_table.
  Binder binder(injector->bind_table_.get());

  module->Configure(&binder);  // This will populate the bind_table.

  if (binder.num_errors() != 0) {
    GUICPP_LOG_(FATAL) << "Creation of Injector failed: "
                       << "Module had " << binder.num_errors() << " errors. ";
  }

  return injector;
}

}  // namespace guicpp

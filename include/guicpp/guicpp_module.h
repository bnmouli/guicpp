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


// Declares the Module interface. Implement this interface and
// create all bindings in Configure() method. The module can be
// used to create injector.

#ifndef GUICPP_MODULE_H_
#define GUICPP_MODULE_H_

#include "guicpp/internal/guicpp_port.h"

namespace guicpp {
class Binder;

// A Module is a logical entity that groups a set of bindings.
// Clients write modules by inheriting this class and implementing the
// Configure method. Each logical set of bindings may be grouped in a
// separate module. A module can include bindings done in other_module
// by calling binder->Install(other_module).
//
// Module is passed to guicpp::CreateInjector() to create an Injector which
// holds all bind information and provides APIs to get instances of objects.
class Module {
 public:
  virtual ~Module() {}

  // User shall specify bindings in this method. Users should specify bindings
  // by calling methods of a Binder passed here. See Binder class in binder.h
  // for details.
  virtual void Configure(Binder* binder) const = 0;

 protected:
  Module() {}

 private:
  GUICPP_DISALLOW_COPY_AND_ASSIGN_(Module);
};

}  // namespace guicpp
#endif  // GUICPP_MODULE_H_

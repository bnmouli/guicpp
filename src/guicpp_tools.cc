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


#include "guicpp/guicpp_tools.h"

#include "guicpp/guicpp_annotations.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/guicpp_binder.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/guicpp_module.h"
#include "guicpp/guicpp_singleton.h"

namespace guicpp {
namespace internal {

class InvokeCleanup {
 public:
  explicit InvokeCleanup(ScopeSetupContext* context): context_(context) {}
  ~InvokeCleanup() {}

  void operator()() {
    context_->Cleanup();
  }

 private:
  ScopeSetupContext* context_;
};

// Injector::Create() takes only one module as argument; Hence in order to do
// required additional bindings, we need this WrapperModule. This module binds
// ScopeSetupContext used by singleton scope, installs user_module and does
// binding that is required to call context->Cleanup().
class WrapperModule: public Module {
 public:
  WrapperModule(const Module* user_module, ScopeSetupContext* context)
      : user_module_(user_module), context_(context) {}
  ~WrapperModule() {}

  // Note: The order in which these bindings are done is important.
  void Configure(Binder* binder) const {
    // ScopeSetupContext is bound first as it is required by Singleton
    // scope which can be used in user_module.
    binder->BindToInstance<ScopeSetupContext>(context_, DeletePointer());

    // Install user module, this can use singleton scope.
    binder->Install(user_module_);
    binder->AddCleanupAction(InvokeCleanup(context_));
  }

 private:
  const Module* user_module_;
  ScopeSetupContext* context_;
};

}  // namespace internal

// The functionality of this function is similar to Injector::CreateInjector()
// but adds some additional bindings necessary for some Guic++ features such as
// LazySingleton scopes to work.
Injector* CreateInjector(const Module* module) {
  internal::ScopeSetupContext* context = new internal::ScopeSetupContext();

  internal::WrapperModule wrapper(module, context);

  Injector* injector = Injector::Create(&wrapper);
  context->Init(injector);

  return injector;
}

}  // namespace guicpp

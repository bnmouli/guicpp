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


// This file contains implementation of the Lazy Singleton scope.
// A type can be bound to LazySingleton using Binder::BindToScope
// as follows:
//   binder->BindToScope<Type, LazySingleton>();
//
// Implementation:
//  This is completely independent of rest of Guic++ code and can
//  be made a separate build target.

#ifndef GUICPP_SINGLETON_H_
#define GUICPP_SINGLETON_H_

#include <vector>

#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_annotations.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/guicpp_binder.h"
#include "guicpp/guicpp_injector.h"
#include "guicpp/guicpp_macros.h"
#include "guicpp/guicpp_provider.h"

namespace guicpp {
// The types that are bound to LazySingleton are instantiated on the first
// request. Successive requests will return the same instance.
//
// Implementation:
//  Binder::BindToScope expects scope types to have a template static
//  method called ConfigureScope() which must do the necessary binding
//  for the type T. In LazySingleton::ConfigureScope() method we bind T
//  annotated with L to LazySingletonProvider<T>
class LazySingleton {
 public:
  template<typename L, typename T>
  static void ConfigureScope(Binder* binder);

 private:
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(LazySingleton);
};


// Implementation

namespace internal {
class UnScoped: public Label {};

class SetupInterface {
 public:
  virtual ~SetupInterface() {}

  virtual void Init(const Injector* injector) = 0;
  virtual void Cleanup() = 0;

 protected:
  SetupInterface() {}
};


// This context implements Init() and Cleanup() methods that are
// called from guicpp::CreateInjector().
class ScopeSetupContext {
 public:
  ScopeSetupContext(): injector_(NULL), cleanup_list_size_(0) {}
  ~ScopeSetupContext() {}

  // Calls Init() methods of all providers in init_list_ in order.
  void Init(const Injector* injector);

  // Calls Cleanup() methods of all providers in cleanup_list_
  // in reverse order.
  void Cleanup();

  // AddToInitList() is called only while binding and hence it is not
  // protected by locks.
  //
  // Init() are called in order of their addition to init_list_.
  void AddToInitList(SetupInterface* init);

  // Not protected with locks. The caller holds lock when necessary.
  void InvokeInitNow(SetupInterface* init) {
    GUICPP_DCHECK_(injector_) << "Injector cant be null at this time";
    init->Init(injector_);
  }

  // AddToCleanupList() is called when objects are instantiated which can
  // happen from different threads. Hence, it is protected with lock.
  //
  // It is an error to add a provider to cleanup list if the same provider is
  // not there in init_list_. This is because we assume size of cleanup_list_
  // can not exceed size of init_list_. This lets us implement cleanup_list_
  // using an array.
  //
  // Cleanup() are called in reverse order of their addition to list.
  void AddToCleanupList(SetupInterface* cleanup);

 private:
  const Injector* injector_;

  // init_list is implemented using vector because it is simple and
  // populated only during initialization.
  vector<SetupInterface*> init_list_;

  // Cleanup list is implemented using an array. We can do so because
  // the max size of list is known at initialization time. This gives
  // performance benefit. The memory for the array is allocated in
  // Init() method.
  //
  // TODO(bnmouli): if we change cleanup_list_size_ to atomic_int
  // we don't need mutex.
  Mutex mu_;
  scoped_array<SetupInterface*> cleanup_list_;
  int cleanup_list_size_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(ScopeSetupContext);
};

GUICPP_INJECTABLE(ScopeSetupContext);

// This class implements lazy singleton scope. It creates a new instance of
// type T on first call to Get(), and successive calls Get() will return the
// same instance.
//
// For thread safety, this uses GoogleOnceDynamic to instantiate exactly once.
template <typename T>
class LazySingletonProvider: public guicpp::AbstractProvider<T* ()>,
                             public SetupInterface {
 public:
  explicit LazySingletonProvider(ScopeSetupContext* context)
      : context_(context), injector_(NULL), object_(NULL) {
    context->AddToInitList(this);
  }

  ~LazySingletonProvider() {
    // Cleanup must be called before deleting LazySingletonProvider.
    GUICPP_DCHECK_(object_ == NULL);
  }

  T* Get() {
    once_.Init(&Create, this);  // Creates object the first time it's called.
    return object_;
  }

  void Init(const Injector* injector) {
    // It is safe to invoke Init() as long as injector remains the same.
    GUICPP_DCHECK_(injector_ == NULL || injector_ == injector);
    injector_ = injector;
  }

  void Cleanup() {
    delete object_;
    object_ = NULL;
  }

 private:
  // This is supposed to be called only once.
  static void Create(LazySingletonProvider* provider) {
    // Ensure provider is initialized before proceeding with object creation.
    provider->context_->InvokeInitNow(provider);

    // If a type "T" is bound to LazySingleton, a call to injector->Get<T*>()
    // in this provider, would lead to infinite recursion. To avoid it, the
    // provider calls injector->Get<At<UnScoped, T*> >();
    provider->object_ = provider->injector_->template Get<At<UnScoped, T*> >();

    // Immediately after object is instantiated add self to cleanup list
    // in ScopeSetupContext which ensures that the singleton objects are
    // deleted in reverse order of creation.
    provider->context_->AddToCleanupList(provider);
  }

  ScopeSetupContext* const context_;

  GoogleOnceDynamic once_;
  const Injector* injector_;
  T* object_;
};

}  // namespace internal


template<typename L, typename T>
inline void LazySingleton::ConfigureScope(Binder* binder) {
  internal::ScopeSetupContext* context =
      binder->GetBoundInstance<internal::ScopeSetupContext>();

  if (context == NULL) {
    GUICPP_LOG_(FATAL) << "Looks like you are using Injector::Create() to "
        "create the injector. You must use use guicpp::CreateInjector() "
        "for singleton scopes to work";
    return;  // Unreachable
  }

  binder->BindToProvider<guicpp::At<L, T> >(
      new internal::LazySingletonProvider<T>(context), DeletePointer());
}

}  // namespace guicpp

#endif  // GUICPP_SINGLETON_H_

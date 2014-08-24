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


// This file declares various types of bind table entries.

#ifndef GUICPP_ENTRIES_H_
#define GUICPP_ENTRIES_H_

#include "guicpp/internal/guicpp_port.h"
#include "guicpp/internal/guicpp_inject_util.h"
#include "guicpp/internal/guicpp_table.h"
#include "guicpp/internal/guicpp_util.h"

namespace guicpp {
namespace internal {
class LocalContext;

// Supports Binder::Bind() and Binder::BindValueType().
// This gets an instance of DestinationType annotated with
// DestinationAnnotations and returns it as SourceType.
template <typename SourceType,
          typename DestinationAnnotations,
          typename DestinationType>
class BindToTypeEntry: public TableEntry<SourceType> {
 public:
  BindToTypeEntry() {}
  virtual ~BindToTypeEntry() {}

  virtual SourceType Get(const Injector* injector,
                         const LocalContext* local_context) const {
    InjectorUtil inject_util(injector);
    return inject_util.GetActualType<
        DestinationAnnotations, DestinationType>(local_context);
  }

  virtual typename TableEntryBase::BindType GetBindType() const {
    return TableEntryBase::BIND_TO_TYPE;
  }

 private:
  GUICPP_DISALLOW_COPY_AND_ASSIGN_(BindToTypeEntry);
};

// Supports Binder::BindToInstance. This binds type T to pointer to an
// instance. CleanupAction determines the action to be taken on "ptr_"
// when this table entry is deleted.
//
// PointerTableEntry accepts a pointer in its constructor and returns
// the same pointer on every call to Get().
template <typename T, typename CleanupAction>
class PointerTableEntry: public TableEntry<T> {
 public:
  PointerTableEntry(T ptr, CleanupAction cleanup_action)
      : ptr_(ptr), cleanup_action_(cleanup_action) {}

  virtual ~PointerTableEntry() {
    cleanup_action_(ptr_);
  }

  // Just returns pointer taken in constructor.
  virtual T Get(const Injector* injector,
                 const LocalContext* local_context) const {
    return ptr_;
  }

  virtual typename TableEntryBase::BindType GetBindType() const {
    return TableEntryBase::BIND_TO_INSTANCE;
  }

 private:
  T ptr_;
  CleanupAction cleanup_action_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(PointerTableEntry);
};

// Supports Binder::BindToValue(). Returns the value provided to the
// constructor on every call to Get().
template <typename T>
class ValueTableEntry: public TableEntry<T> {
 public:
  explicit ValueTableEntry(T value): value_(value) {}
  virtual ~ValueTableEntry() {}

  // Just returns the value taken in the constructor.
  virtual T Get(const Injector* injector,
                const LocalContext* local_context) const {
    return value_;
  }

  virtual typename TableEntryBase::BindType GetBindType() const {
    return TableEntryBase::BIND_TO_VALUE;
  }

 private:
  T value_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(ValueTableEntry);
};

// Supports Binder::BindRefToPointed. This binds type T to reference to
// the instance. CleanupAction determines the action to be taken on "ptr_"
// when this table entry is deleted.
//
// ReferenceTableEntry accepts a pointer in its constructor and returns
// reference to the pointed instance on every call to Get().
template <typename T, typename CleanupAction>
class ReferenceTableEntry: public TableEntry<T> {
 public:
  ReferenceTableEntry(typename TypeInfo<T>::ReferredType* ptr,
                    CleanupAction cleanup_action)
      : ptr_(ptr), cleanup_action_(cleanup_action) {}

  virtual ~ReferenceTableEntry() {
    cleanup_action_(ptr_);
  }

  // Just returns pointer taken in constructor.
  virtual T Get(const Injector* injector,
                 const LocalContext* local_context) const {
    return *ptr_;
  }

  virtual typename TableEntryBase::BindType GetBindType() const {
    return TableEntryBase::BIND_TO_POINTED;
  }

 private:
  typename TypeInfo<T>::ReferredType* ptr_;
  CleanupAction cleanup_action_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(ReferenceTableEntry);
};

// Supports Binder::BindToProvider(). This binds the type T to the provider.
// This calls Get() method of the provider provided by user.
template <typename T, typename ProviderType, typename CleanupAction>
class BindToProviderEntry: public TableEntry<T> {
 public:
  // BindToProviderEntry assumes ownership of provider.
  BindToProviderEntry(ProviderType* provider, CleanupAction cleanup_action)
      : provider_(provider), cleanup_action_(cleanup_action) {}
  virtual ~BindToProviderEntry() {
    cleanup_action_(provider_);
  }

  // Calls provider's Get() method by injecting argument it needs.
  virtual T Get(const Injector* injector,
                const LocalContext* local_context) const {
    return provider_->InvokeGet(injector, local_context);
  }

  virtual typename TableEntryBase::BindType GetBindType() const {
    return TableEntryBase::BIND_TO_PROVIDER;
  }

 private:
  ProviderType* provider_;
  CleanupAction cleanup_action_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(BindToProviderEntry);
};

// Entry used only for cleanup action. This is never added to BindTable
template <typename CleanupAction>
class CleanupEntry: public InvalidEntry {
 public:
  explicit CleanupEntry(CleanupAction cleanup_action)
      : cleanup_action_(cleanup_action) {}
  ~CleanupEntry() {
    cleanup_action_();
  }

 private:
  CleanupAction cleanup_action_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(CleanupEntry);
};

}  // namespace internal
}  // namespace guicpp

#endif  // GUICPP_ENTRIES_H_

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


// This file is to declare Binder class. Binder provides methods
// that are used to specify bindings.
//
// Include this header in files where SomeModule::Configure()
// methods are defined.

#ifndef GUICPP_BINDER_H_
#define GUICPP_BINDER_H_

#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_annotations.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/internal/guicpp_entries.h"
#include "guicpp/internal/guicpp_table.h"
#include "guicpp/internal/guicpp_types.h"
#include "guicpp/internal/guicpp_util.h"

namespace guicpp {
class LazySingleton;
class Module;

namespace internal {
class GuicppTestScope;  // This scope is used only for testing.
}  // namespace internal

// Binder class provides the APIs to populate bind_table.
// Bind table will then be used by Injector.
class Binder {
 public:
  // User should not create Binder directly. Binder is passed to
  // Module::Configure() method from Injector::Create() method.
  ~Binder();  // No class should inherit from Binder.

  // Binds "Interface" to "Implementation". Both "Interface" and
  // "Implementation" may be annotated.  This makes Guic++ treat any
  // request for pointer to "Interface" as the request for pointer to
  // "Implementation". The pointer to "Implementation" must be implicitly
  // convertible to pointer to "Interface" (i.e. "Implementation*" must be
  // assignable to ""Interface*").
  //
  // Usage:
  //   binder->Bind<Interface, Implementation>();
  template<typename Interface, typename Implementation>
  void Bind();

  // Similar to Bind() but binds non-pointer types. This binds a value-type "T"
  // to another value-type "D". Both T and D may be annotated. The D should
  // be assignable to T.
  //
  // This tells Guic++ that any request for "T" (which is of non-pointer type
  // and can be annotated) should be treated as if it were a request for "D"
  // (which is a non-pointer and may be annotated).
  //
  // Usage:
  //   binder->Bind<T, D>();
  template<typename T, typename D>
  void BindValueType();

  // Binds T to the instance pointed by "ptr". "ptr" has to be implicitly
  // convertible to T*. T may be annotated.
  //
  // Usage:
  //   binder->BindToInstance<T>(ptr, cleanup_action)
  //
  // @param ptr is a pointer to an instance of type T
  // @param cleanup_action is the action to be taken on the pointer at the time
  //        of cleanup. The action is a function pointer or a functor that is
  //        called with "ptr" as its argument at the time of cleanup.
  //        Guic++ defines following functors:
  //          * guicpp::DoNothing() : Does nothing. Caller must own the "ptr".
  //          * guicpp::DeletePointer() : Deletes the pointer
  //        These functors are defined later in this file (see below).
  //        Cleanup actions are called in reverse order of bnding.
  template <typename T, typename CleanupAction>
  void BindToInstance(typename internal::AtUtil::GetTypes<T>::ArgType* ptr,
                      CleanupAction cleanup_action);

  // Binds T to value.
  // Usage:
  //   binder->BindToValue<Type>(value);
  //
  // Note: Type may be annotated.
  //
  // @param value is a value of type T.
  template <typename T>
  void BindToValue(typename internal::AtUtil::GetTypes<T>::ActualType value);

  // Binds Type& to *pointer.
  // Usage:
  //   binder->BindToRefToPointed<Type>(value);
  //
  // Note: Type may be annotated.
  //
  // @param ptr is a pointer to an instance of type T
  // @param cleanup_action is the action to be taken on the pointer at the time
  //        of cleanup.
  template <typename T, typename CleanupAction>
  void BindRefToPointed(typename internal::AtUtil::GetTypes<T>::ArgType* ptr,
                        CleanupAction cleanup_action);

  // Binds "T" to the provider implemented by the user. When an instance of T is
  // requested, the Get() method of the provider is called by injecting all its
  // arguments.
  //
  // Usage:
  //   binder->BindToProvider<T>(pointer-to-provider);
  //
  // @param provider: is a pointer to the provider implemented by user by
  //        inheriting from guicpp::AbstractProvider<prototype-of-get-method>
  //
  // @param cleanup_action is a function pointer or a functor that is called
  //        with "provider" as its argument at the time of cleanup.
  template <typename T, typename ProviderType, typename CleanupAction>
  void BindToProvider(ProviderType* provider, CleanupAction cleanup_action);

  // Similar to BindToProvider, used when return type of the provider is a
  // value-type When requested for T, the Get() method of the provider is
  // called by injecting all its arguments.
  //
  // Usage:
  //   binder->BindValueToProvider<T>(pointer-to-provider);
  //
  // @param provider: same as in BindToProvider, but the return type of the
  //        provider's Get() is non-pointer. The binder assumes the ownership
  //        of the provider.
  //
  // @param cleanup_action same as BindToProvider.
  template <typename T, typename ProviderType, typename CleanupAction>
  void BindValueToProvider(ProviderType* provider,
                           CleanupAction cleanup_action);

  // Binds a scope to type T.
  // Usage:
  //   binder->BindToScope<T, ScopeName>();
  //
  // Currently only LazySingleton is supported and hence the usage is:
  //   binder->BindToScope<T, guicpp::LazySingleton>();
  //
  // T may be annotated with labels.
  //
  // If a type is bound to LazySingleton, those objects are instantiated exactly
  // once by Guic++ when it is requested for first time. These objects are owned
  // by injector.
  template <typename T, typename Scope>
  void BindToScope();

  // Registers a function/functor to be called at the time of cleanup.
  // function/functor should take no arguments.
  // Cleanup actions are called in reverse order of binding.
  template <typename CleanupAction>
  void AddCleanupAction(CleanupAction cleanup_action);

  // Install another module. In other words, include bindings specified in other
  // module.
  //
  // Usage:
  //   binder->Install(pointer_to_anothermodule);
  //
  // The ownership of the other module lies with the caller.
  void Install(const Module* module);

 private:
  // Binder is never created directly.
  // Binder does not own bind_table.
  explicit Binder(internal::BindTable* bind_table);

  // Returns the instance bound to "T" using BindToInstance(). If "T" is not
  // bound, it returns NULL. It is an error if "T" is bound but not using
  // BindToInstance(), in that case this method fails fatally.
  //
  // NOTE: We may make this method OR a variant of this method available to all
  // users (a public method). But this is not in its final version hence keeping
  // it private to avoid abuses.
  //
  // LazySingleton singleton needs to use it and hence making it a friend.
  template <typename T>
  typename internal::AtUtil::GetTypes<T>::ArgType* GetBoundInstance() const;
  friend class LazySingleton;  // Uses GetBoundInstance()

  // Number of errors encountered so far. Used only in Injector::Create method.
  int num_errors() const { return num_errors_; }

  // Binder is instantiated only in Injector::Create(). Injector::Create() will
  // fatally fail if num_errors() returns non-zero. Injector does not access any
  // other private members of Binder other than constructor and num_errors().
  friend class Injector;

  // Following private members of Binder are not accessed by any friend classes.

  // Adds entry to bind_table_ for tid. If there is an entry already for tid,
  // this will GUICPP_LOG_ an error, will increment  num_errors_ and entry
  // pointer will be deleted.
  void AddBindEntry(internal::TypeId tid,
                    const internal::TableEntryBase* entry);

  internal::BindTable* bind_table_;  // pointer not owned by Binder

  // Number of errors encountered so far. num_errors_ will be 0 to start with,
  // this is incremented each time an error encountered.
  int num_errors_;

  friend class GuicppBinderTest;
  friend class GuicppBinderLogTest;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(Binder);
};

// Functors that can be used as "cleanup_action" with Binder::BindToInstance().

// Deletes the pointer.
struct DeletePointer {
  template <typename T>
  void operator()(T* p) { delete p; }
};

// Does nothing. This can be used when you don't want Guic++ to take ownership
// of the pointer.
struct DoNothing {
  template <typename T>
  void operator()(T* p) {}  // Does nothing.
};

// -- Implementations --

// Binds "Interface" to "Implementation".
template<typename Interface, typename Implementation>
inline void Binder::Bind() {
  using internal::AtUtil;
  using internal::TableEntryBase;
  using internal::BindToTypeEntry;
  using internal::InjectorUtil;
  using internal::TypeId;

  // const/volatile qualifiers can't be ignored for pointer arguments.
  typedef typename AtUtil::GetTypes<Interface>::ArgType* LhsType;
  typedef typename AtUtil::GetTypes<Implementation>::ArgType* RhsType;
  typedef typename AtUtil::GetTypes<Interface>::Annotations LhsAnnotations;
  typedef typename AtUtil::GetTypes<Implementation>::Annotations RhsAnnotations;

  const TableEntryBase* entry =
      new BindToTypeEntry<LhsType, RhsAnnotations, RhsType>();

  TypeId tid = InjectorUtil::GetBindId<LhsAnnotations, LhsType>();
  AddBindEntry(tid, entry);
}

// Similar to Bind() but binds non-pointer types.
template<typename T, typename D>
inline void Binder::BindValueType() {
  using internal::AtUtil;
  using internal::TableEntryBase;
  using internal::BindToTypeEntry;
  using internal::InjectorUtil;
  using internal::TypeId;

  // const/volatile qualifiers can be ignored for value type arguments.
  typedef typename AtUtil::GetTypes<T>::ActualType LhsType;
  typedef typename AtUtil::GetTypes<D>::ActualType RhsType;
  typedef typename AtUtil::GetTypes<T>::Annotations LhsAnnotations;
  typedef typename AtUtil::GetTypes<D>::Annotations RhsAnnotations;

  const TableEntryBase* entry =
      new BindToTypeEntry<LhsType, RhsAnnotations, RhsType>();

  TypeId tid = InjectorUtil::GetBindId<LhsAnnotations, LhsType>();
  AddBindEntry(tid, entry);
}

// Binds T to the instance pointed by "ptr".
template <typename T, typename CleanupAction>
inline void Binder::BindToInstance(
    typename internal::AtUtil::GetTypes<T>::ArgType* ptr,
    CleanupAction cleanup_action) {
  using internal::AtUtil;
  using internal::TableEntryBase;
  using internal::PointerTableEntry;
  using internal::InjectorUtil;
  using internal::TypeId;

  typedef typename AtUtil::GetTypes<T>::Annotations LhsAnnotations;
  typedef typename AtUtil::GetTypes<T>::ArgType* LhsType;

  const TableEntryBase* entry =
      new PointerTableEntry<LhsType, CleanupAction>(ptr, cleanup_action);

  TypeId tid = InjectorUtil::GetBindId<LhsAnnotations, LhsType>();
  AddBindEntry(tid, entry);
}

// Binds T to the value.
template <typename T>
inline void Binder::BindToValue(
    typename internal::AtUtil::GetTypes<T>::ActualType value) {
  using internal::AtUtil;
  using internal::TableEntryBase;
  using internal::ValueTableEntry;
  using internal::InjectorUtil;
  using internal::TypeId;

  typedef typename AtUtil::GetTypes<T>::Annotations LhsAnnotations;
  typedef typename AtUtil::GetTypes<T>::ActualType LhsType;

  const TableEntryBase* entry = new ValueTableEntry<LhsType>(value);

  TypeId tid = InjectorUtil::GetBindId<LhsAnnotations, LhsType>();
  AddBindEntry(tid, entry);
}

// Binds T& to the instance pointed by "ptr".
template <typename T, typename CleanupAction>
inline void Binder::BindRefToPointed(
    typename internal::AtUtil::GetTypes<T>::ArgType* ptr,
    CleanupAction cleanup_action) {
  using internal::AtUtil;
  using internal::TableEntryBase;
  using internal::ReferenceTableEntry;
  using internal::InjectorUtil;
  using internal::TypeId;

  typedef typename AtUtil::GetTypes<T>::Annotations LhsAnnotations;
  typedef typename AtUtil::GetTypes<T>::ArgType& LhsType;

  const TableEntryBase* entry =
      new ReferenceTableEntry<LhsType, CleanupAction>(ptr, cleanup_action);

  TypeId tid = InjectorUtil::GetBindId<LhsAnnotations, LhsType>();
  AddBindEntry(tid, entry);
}

// Binds "T" to the provider implemented by the user.
template <typename T, typename ProviderType, typename CleanupAction>
void Binder::BindToProvider(ProviderType* provider,
                            CleanupAction cleanup_action) {
  using internal::AtUtil;
  using internal::TableEntryBase;
  using internal::BindToProviderEntry;
  using internal::InjectorUtil;
  using internal::TypeId;

  typedef typename AtUtil::GetTypes<T>::Annotations LhsAnnotations;
  typedef typename AtUtil::GetTypes<T>::ArgType* LhsType;

  // BindToProviderEntry assumes the ownership of provider.
  const TableEntryBase* entry = new BindToProviderEntry<
      LhsType, ProviderType, CleanupAction>(provider, cleanup_action);

  TypeId tid = InjectorUtil::GetBindId<LhsAnnotations, LhsType>();
  AddBindEntry(tid, entry);
}

// Similar to BindToProvider, used when return type of the provider
// is a value-type
template <typename T, typename ProviderType, typename CleanupAction>
void Binder::BindValueToProvider(ProviderType* provider,
                                 CleanupAction cleanup_action) {
  using internal::AtUtil;
  using internal::TableEntryBase;
  using internal::BindToProviderEntry;
  using internal::InjectorUtil;
  using internal::TypeId;

  typedef typename AtUtil::GetTypes<T>::Annotations LhsAnnotations;
  typedef typename AtUtil::GetTypes<T>::ActualType LhsType;

  // BindToProviderEntry assumes the ownership of provider.
  const TableEntryBase* entry = new BindToProviderEntry<
      LhsType, ProviderType, CleanupAction>(provider, cleanup_action);

  TypeId tid = InjectorUtil::GetBindId<LhsAnnotations, LhsType>();
  AddBindEntry(tid, entry);
}

// Binds a scope to type T.
//
// Implementation Note:
//   This method expects a static template method called "ConfigureScope" to be
//   defined in the "Scope" class. The "ConfigureScope" method is expected to
//   bind "T" to a provider which creates "T" as required. For example,
//   LazySingleton::ConfigureScope() binds T to LazySingletonProvider.
//   This provider creates an instance of T on first call to its Get() and
//   returns the same instance on successive calls.
//
//   This implementation is subject to change. Currently this option is chosen
//   because it requires very little built-in support from Guic++.
template <typename T, typename Scope>
void Binder::BindToScope() {
  // Scope implementation is subject to change in future releases. This
  // GUICPP_COMPILE_ASSERT_ stops users from using "custom scope". We don't
  // expect users to write "custom scopes" till the time we finalize the
  // implementation of Scope.
  //
  // GuicppTestScope is used only for Guic++ testing.
  GUICPP_COMPILE_ASSERT_(
      (guicpp::internal::is_same<LazySingleton, Scope>::value) ||
      (guicpp::internal::is_same<internal::GuicppTestScope, Scope>::value),
      this_version_does_not_allow_anything_otherthan_LazySingleton);

  typedef typename internal::AtUtil::GetTypes<T>::ArgType LhsType;
  typedef typename internal::AtUtil::GetTypes<T>::Annotations LhsAnnotations;

  Scope::template ConfigureScope<
      typename internal::AtUtil::GetAt<LhsAnnotations, Label>::Type,
      LhsType>(this);
}

// Registers a function/functor to be called at the time of cleanup.
template <typename CleanupAction>
void Binder::AddCleanupAction(CleanupAction cleanup_action) {
  bind_table_->AddToCleanupList(
      new internal::CleanupEntry<CleanupAction>(cleanup_action));
}

// Returns the instance bound to "T" using BindToInstance().
template <typename T>
typename internal::AtUtil::GetTypes<T>::ArgType* Binder::GetBoundInstance()
  const {
  using internal::AtUtil;
  using internal::TableEntryBase;
  using internal::TableEntryReader;
  using internal::InjectorUtil;
  using internal::TypeId;

  typedef typename AtUtil::GetTypes<T>::Annotations Annotations;
  typedef typename AtUtil::GetTypes<T>::ArgType* ArgType;

  TypeId tid = InjectorUtil::GetBindId<Annotations, ArgType>();
  const TableEntryBase* base_entry = bind_table_->FindEntry(tid);

  if (base_entry == NULL) {
    return NULL;
  }

  if (base_entry->GetBindType() != TableEntryBase::BIND_TO_INSTANCE) {
    // TODO(bnmouli): ADDNAME Change to meaning full error once GetTypeName is
    // added to TableEntryBase
    GUICPP_LOG_(FATAL) << "T is not bound using BindToInstance(), "
                          "but called GetBoundInstance()";
  }

  // You don't need injector/local context to do Get() in case of
  // BIND_TO_INSTANCE.
  return TableEntryReader<ArgType>::Get(base_entry, NULL, NULL);
}

}  // namespace guicpp
#endif  // GUICPP_BINDER_H_

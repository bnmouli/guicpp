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


// This file contains implementations of InjectHandlers.

#ifndef GUICPP_INJECT_UTIL_H_
#define GUICPP_INJECT_UTIL_H_

#include "guicpp/internal/guicpp_factory_types.h"
#include "guicpp/internal/guicpp_local_context.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/internal/guicpp_table.h"

// An empty global class passed as an argument to GuicppGetDefaultEntry.
// Thanks to Koenig lookup having this argument adds global namespace to
// compiler lookup list while resolving GuicppGetDefaultEntry identifier.
class GuicppEmptyGlobalClass {};

namespace guicpp {
class Injector;

namespace internal {
class LocalContext;

// Utility used with injector
class InjectorUtil {
 public:
  explicit InjectorUtil(const Injector* injector): injector_(injector) {}
  ~InjectorUtil() {}

  template <typename T>
  typename AtUtil::GetTypes<T>::ActualType GetWithContext(
      const LocalContext* local_context) const;

  template <typename Annotations, typename ActualType>
  ActualType GetActualType(const LocalContext* local_context) const;

  // Gets the bind Id uses to put/lookup in bind_table_
  template <typename Annotations, typename ActualType>
  static TypeId GetBindId();

  // Gets the bind Id uses to put/lookup in factory argument list
  template <typename T>
  static TypeId GetFactoryArgsBindId();

  const TableEntryBase* FindEntry(TypeId bindId) const;

 private:
  const Injector* injector_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(InjectorUtil);
};


// Helper class used to combine multiple types. This class is used to get a
// unique TypeId for ActualType with the label "L" and with InjectType "I".
template <typename I, typename L, typename ActualType>
class LabelHelper {};

template <typename Annotations, typename ActualType>
class NormalInjectHandler {
 private:
  typedef typename TypeInfo<ActualType>::TypeSpecifier TypeSpecifier;

 public:
  template <typename Table>
  TypeId GetBindId(TypeKey<Table> t) const {
    return Error<ActualType>::Invalid_binding_or_lookup(t);
  }

  TypeId GetBindId(TypeKey<NormalInject> t) const {
    typedef typename AtUtil::GetAt<Annotations, Label>::Type LabelAt;
    return TypeIdProvider<
        LabelHelper<NormalInject, LabelAt, TypeSpecifier> >::GetTypeId();
  }

  ActualType Get(const Injector* injector,
                 const LocalContext* local_context) const;

 private:
  ActualType GetHelper(const TableEntryBase& base_entry,
                       const Injector* injector,
                       const LocalContext* local_context) const {
    return TableEntryReader<ActualType>::Get(
        &base_entry, injector, local_context);
  }
};

template <typename ActualType>
struct NormalInjectSelector {
  typedef typename TypeInfo<ActualType>::TypeSpecifier TypeSpecifier;
  typedef typename integral_constant<bool,
          is_class_or_union<TypeSpecifier>::value &&
          !is_convertible<TypeSpecifier*, InternalType*>::value>::type Type;
};

template <typename Annotations, typename ActualType>
NormalInjectHandler<Annotations, ActualType> GuicppGetInjectHandler(
    TypeKey3<NormalInject, Annotations, ActualType>,
    typename NormalInjectSelector<ActualType>::Type) {
  return NormalInjectHandler<Annotations, ActualType>();
}


// NormalInjectHandler refers to default binding provided GUICPP_INJECT* macros.
// This handler is used for types for which default binding can be missing such
// as primitive types (any non class/union type). This expects the type to be
// explicitly bound in some module.
template <typename Annotations, typename ActualType>
class MissingDefaultTypeHandler {
 public:
  template <typename Table>
  TypeId GetBindId(TypeKey<Table> t) const {
    return NormalInjectHandler<Annotations, ActualType>().GetBindId(t);
  }

  ActualType Get(const Injector* injector,
                 const LocalContext* local_context) const {
    TypeId tid = GetBindId(TypeKey<NormalInject>());
    InjectorUtil inject_util(injector);

    const TableEntryBase* base_entry = inject_util.FindEntry(tid);
    if (base_entry == NULL) {
      GUICPP_LOG_(FATAL) << "This type can not be instantiated, "
                            "missing binding";
    }

    return TableEntryReader<ActualType>::Get(
        base_entry, injector, local_context);
  }
};

template <typename ActualType>
struct MissingDefaultTypeSelector {
  typedef typename TypeInfo<ActualType>::TypeSpecifier TypeSpecifier;
  typedef typename integral_constant<bool,
          !is_class_or_union<TypeSpecifier>::value>::type Type;
};

template <typename Annotations, typename ActualType>
MissingDefaultTypeHandler<Annotations, ActualType> GuicppGetInjectHandler(
    TypeKey3<NormalInject, Annotations, ActualType>,
    typename MissingDefaultTypeSelector<ActualType>::Type) {
  return MissingDefaultTypeHandler<Annotations, ActualType>();
}


template <typename Annotations, typename ActualType>
class AssistedInjectHandler {
 private:
  typedef typename TypeInfo<ActualType>::TypeSpecifier TypeSpecifier;

 public:
  template <typename Table>
  TypeId GetBindId(TypeKey<Table> t) const {
    return Error<ActualType>::Invalid_use_of_Assisted_annotation;
  }

  TypeId GetBindId(TypeKey<Assisted> t) const {
    typedef typename AtUtil::GetAt<Annotations, Label>::Type LabelAt;
    return TypeIdProvider<
        LabelHelper<Assisted, LabelAt, TypeSpecifier> >::GetTypeId();
  }

  ActualType Get(const Injector* injector,
                 const LocalContext* local_context) const {
    TypeId tid = GetBindId(TypeKey<Assisted>());
    const TableEntryBase* base_entry = local_context->FindEntry(tid);
    if (base_entry == NULL) {
      GUICPP_LOG_(FATAL) << "Expected assisted argument (a.k.a "
          "factory argument), but is not present in factory parameter list.";
    }
    return TableEntryReader<ActualType>::Get(
        base_entry, injector, local_context);
  }
};

template <typename ActualType>
struct AssistedInjectSelector {
  typedef typename TypeInfo<ActualType>::TypeSpecifier TypeSpecifier;
  typedef typename integral_constant<bool,
          !is_convertible<TypeSpecifier*, InternalType*>::value>::type Type;
};

template <typename Annotations, typename ActualType>
AssistedInjectHandler<Annotations, ActualType> GuicppGetInjectHandler(
    TypeKey3<Assisted, Annotations, ActualType>,
    typename AssistedInjectSelector<ActualType>::Type) {
  return AssistedInjectHandler<Annotations, ActualType>();
}


template <typename Annotations, typename ActualType>
class InternalTypeInjectHandler {
 private:
  typedef typename TypeInfo<ActualType>::TypeSpecifier TypeSpecifier;

 public:
  template <typename Table>
  TypeId GetBindId(TypeKey<Table> t) const {
    return Error<ActualType>::Invalid_binding_of_internal_type(t);
  }

  ActualType Get(const Injector* injector,
                 const LocalContext* local_context) const {
    return GetHelper(static_cast<TypeSpecifier*>(NULL),
                     injector, local_context);
  }

 private:
  ActualType GetHelper(Injector*, const Injector* injector,
                       const LocalContext* local_context) const {
    return const_cast<Injector*>(injector);
  }

  ActualType GetHelper(FactoryBase*, const Injector* injector,
                       const LocalContext* local_context) const {
    return new RealFactory<
        Annotations, TypeSpecifier,
        typename TypeSpecifier::GuicppGetSignature>(injector);
  }
};

template <typename ActualType>
struct InternalTypeInjectSelector {
  typedef typename TypeInfo<ActualType>::TypeSpecifier TypeSpecifier;
  typedef typename integral_constant<bool,
          is_convertible<TypeSpecifier*, InternalType*>::value>::type Type;
};

template <typename Annotations, typename ActualType>
InternalTypeInjectHandler<Annotations, ActualType> GuicppGetInjectHandler(
    TypeKey3<NormalInject, Annotations, ActualType>,
    typename InternalTypeInjectSelector<ActualType>::Type) {
  return InternalTypeInjectHandler<Annotations, ActualType>();
}


// -- Implementation --

// The default implementation of GuicppCreateInstanceUsingCtor.
// This implementation is chosen by compiler when a class specific
// implementation is not found [Section 14.6].
// It induces a compiler error that is more readable then what the compiler
// emits in the absence of this default implementation.
template <typename T>
inline InvalidEntry GuicppGetDefaultEntry(TypeKey<T> t, ...) {
  return guicpp::internal::Error<T>::Type_is_not_injectable(t);
}

template <typename Annotations, typename ActualType>
ActualType NormalInjectHandler<Annotations, ActualType>::Get(
    const Injector* injector, const LocalContext* local_context) const {
  TypeId tid = GetBindId(TypeKey<NormalInject>());
  InjectorUtil inject_util(injector);

  const TableEntryBase* base_entry = inject_util.FindEntry(tid);
  if (base_entry != NULL) {
    return TableEntryReader<ActualType>::Get(
        base_entry, injector, local_context);
  }

  // The second argument is of type "T*" and is a dummy argument. This argument
  // is added just to add T's namespace to lookup list. Thanks to Koenig lookup
  // (AKA Argument Dependent Lookup), the compiler will try to look up the
  // GuicppGetDefaultEntry function in T's namespace [Section 3.4.2]; this
  // allows us to invoke an inject macro in T's namespace and for the same
  // reason the third argument of type GuicppEmptyGlobalClass is added and
  // this adds global namespace to the lookup list. This lets us to invoke
  // an inject macro in the global namespace when invoking it from class's
  // namespace is not feasible.
  return GetHelper(GuicppGetDefaultEntry(TypeKey<TypeSpecifier>(),
                                         static_cast<TypeSpecifier*>(0),
                                         GuicppEmptyGlobalClass()),
                   injector, local_context);
}


template <typename T>
typename AtUtil::GetTypes<T>::ActualType InjectorUtil::GetWithContext(
    const LocalContext* local_context) const {
  typedef typename AtUtil::GetTypes<T>::ActualType ActualType;
  typedef typename AtUtil::GetTypes<T>::Annotations Annotations;

  return GetActualType<Annotations, ActualType>(local_context);
}

template <typename Annotations, typename ActualType>
ActualType InjectorUtil::GetActualType(
    const LocalContext* local_context) const {
  typedef typename AtUtil::GetAt<Annotations, InjectType>::Type InjectTypeAt;
  return GuicppGetInjectHandler(
      TypeKey3<InjectTypeAt, Annotations, ActualType>(), true_type())
      .Get(injector_, local_context);
}

// Gets the bind Id uses to put/lookup in bind_table_
template <typename Annotations, typename ActualType>
TypeId InjectorUtil::GetBindId() {
  typedef typename AtUtil::GetAt<Annotations, InjectType>::Type InjectTypeAt;
  return GuicppGetInjectHandler(
      TypeKey3<InjectTypeAt, Annotations, ActualType>(), true_type())
      .GetBindId(TypeKey<NormalInject>());
}

// Gets the bind Id uses to put/lookup in factory argument list
template <typename T>
TypeId InjectorUtil::GetFactoryArgsBindId() {
  typedef typename AtUtil::GetTypes<T>::ActualType ActualType;
  typedef typename AtUtil::GetTypes<T>::Annotations Annotations;
  typedef typename AtUtil::GetAt<Annotations, InjectType, Assisted>::Type
      InjectTypeAt;

  return GuicppGetInjectHandler(
      TypeKey3<InjectTypeAt, Annotations, ActualType>(), true_type())
      .GetBindId(TypeKey<Assisted>());
}

}  // namespace internal
}  // namespace guicpp

#endif  // GUICPP_INJECT_UTIL_H_

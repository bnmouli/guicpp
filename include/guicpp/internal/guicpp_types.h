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


// This file defines types that are used extensively in Guic++ implementation.

#ifndef GUICPP_TYPES_H_
#define GUICPP_TYPES_H_

#include "guicpp/internal/guicpp_port.h"

namespace guicpp {
class Injector;

namespace internal {
class LocalContext;

// Type of unique id associated with each type.
typedef const void* TypeId;

// Exposed Guic++ internal classes are inherited from this class,
// it would make it simpler to check if a given type is Guic++
// internal type.
class InternalType {};

// Removes parentheses from its arguments.
// INTERNAL USE ONLY - do not invoke from the user code.
#define GUICPP_RM_PARENTHESES_(...) __VA_ARGS__

// TypeKey<T> is an empty classes used only for overloading functions/methods.
// All functions/methods that take TypeKey<SomeClass> as an argument, do so
// just to overload the function/method name. These arguments serve no other
// purpose and they are ignored.
//
// Example: Consider these methods
//  helper function to get non-pointers
//    template <typename T>
//    T GetHelper(TypeKey<T> t);
//
//  helper function to get pointers
//    template <typename T>
//    T* GetHelper(TypeKey<T*> t);
//
//  GetHelper(TypeKey<MyClass>())  // instantiates non-pointer version
//  GetHelper(TypeKey<MyClass*>()) // instantiates pointer version
//
//  Note: template specialization won't work for methods.
//  The following code is invalid.
//  class SomeClass {
//    template <typename T>
//    T GetHelper();  // non-pointer version
//
//    template <typename T>
//    T* GetHelper(); //  pointer version
//  };
//
//  instance_of_someclass->GetHelper<MyClass>();  // ambiguous and fail
template<typename T>
struct TypeKey {
  typedef T Type;
};

// Same as above, combines 2 types
template<typename T1, typename T2>
struct TypeKey2 {
  typedef T1 Type1;
  typedef T2 Type2;
};

// Same as above, combines 3 types
template<typename T1, typename T2, typename T3>
struct TypeKey3 {
  typedef T1 Type1;
  typedef T2 Type2;
  typedef T3 Type3;
};


// Categories of a type
class TypesCategory {
 public:
  enum Enum {
    IS_POINTER,
    IS_REFERENCE,
    IS_VALUE
  };

  // These types help to overload inline functions and their by
  // instantiate appropriate functions.
  typedef guicpp::internal::integral_constant<Enum, IS_POINTER> IsPointer;
  typedef guicpp::internal::integral_constant<Enum, IS_REFERENCE> IsReference;
  typedef guicpp::internal::integral_constant<Enum, IS_VALUE> IsValue;

 private:
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(TypesCategory);
};

// Util used to get category of the type T
template <typename T>
class TypeInfo {
 public:
  typedef T Type;

  // Category : category of the type T evaluates to
  //            IsPointer if it is a pointer type
  //            IsReference if it is a reference type
  //            IsValue if it is a value type
  typedef typename guicpp::internal::if_<guicpp::internal::is_pointer<T>::value,
      TypesCategory::IsPointer,
  // else if
      typename guicpp::internal::if_<guicpp::internal::is_reference<T>::value,
          TypesCategory::IsReference,
      // else
          TypesCategory::IsValue>::type>::type Category;

  // ReferredType : Evaluates to
  //      pointed type if T is a pointer type
  //      referred type if T is a reference type
  //      T itself if T is a value type
  typedef typename guicpp::internal::if_<guicpp::internal::is_pointer<T>::value,
      typename guicpp::internal::remove_pointer<T>,
  // else if
      typename guicpp::internal::if_<guicpp::internal::is_reference<T>::value,
          typename guicpp::internal::remove_reference<T>,
      // else
          guicpp::internal::identity_<T> >::type>::type::type ReferredType;

  // IsConst : true if the referred object is constant.
  //     That is IsConst evaluates to true_type if:
  //       T is pointer and pointed type is constant.
  //       T is a constant value or reference.
  typedef guicpp::internal::is_same<ReferredType*, const ReferredType*> IsConst;

  // TypeSpecifier : type specifier of type T
  typedef typename guicpp::internal::remove_cv<ReferredType>::type
      TypeSpecifier;

 private:
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(TypeInfo);
};

// Base class for annotation classes (At). Having such a common
// base class helps us to check if a given type T is annotated
// or not. See AtUtil defined in util.h
class AtBaseClass {
 private:
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(AtBaseClass);
};

}  // namespace internal
}  // namespace guicpp

#endif  // GUICPP_TYPES_H_

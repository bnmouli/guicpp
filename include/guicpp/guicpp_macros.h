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


// Note: Prefer including "guicpp.h" instead of this as in many cases you
//       would also need to include other headers included by guicpp.h
//
// This file contains macros that are used to make classes/types injectable
// and a supportive macro GUICPP_DEFINE.
//
// A class/type is said to be injectable if Guic++ "knows" if that class/type
// is instantiable and if so how. This is referred as "default binding", it is
// used if the type is not explicitly bound. For example,
//
//      GUICPP_INJECT_CTOR(MyClass, ());
//
// Creates a default binding for MyClass to its default constructor. If MyClass
// is not bound using binder in any module, then MyClass is instantiated using
// its default constructor.
//
// The macros that specify this default binding are referred to as
// "inject macros". All macros defined in this file are "inject macros"
// except for GUICPP_DEFINE. GUICPP_DEFINE is a support macro that defines
// functions for Guic++'s internal use.
//
// A class/type is said to be "injectable" at a point if the class/type is made
// injectable using any of the macro defined in this file before that point.
//
// NOTE: Only one of the inject macros should be invoked for any class and
//       must be done in exactly one (header) file. Invoking more than one
//       and/or in multiple files can lead to ODR violation.
//
// Example:
//   Here are the lines from example.h
//      class A;
//      class C;
//      // Checkpoint 1
//      class A {
//       public:
//        A() {}
//      };
//      class B {
//       public:
//        B() {}
//      };
//      // Checkpoint 2
//      GUICPP_INJECT_CTOR(A);
//      // Checkpoint 3
//      GUICPP_INJECTABLE(C, ());
//      // GUICPP_DEFINE(C) invoked from a source file not shown here.
//      // Checkpoint 4
//      class C {
//       public:
//        C() {}
//      };
//      // Checkpoint 5
//
//   The following statements are true about example.h
//      * Classes A and C are injectable.
//      * Class B is not injectable.
//      * At Checkpoint 1 class A and C are incomplete and non-injectable
//      * At Checkpoint 2 class A and B are complete types but none are
//        injectable
//         - Because, inject macro for A and C appears later
//      * At Checkpoint 3 A is complete and injectable.
//         - Because inject macro for A is invoked before Checkpoint 3
//      * At Checkpoint 4 A and C are injectable but C is an incomplete type.
//      * At Checkpoint 5 A, B and C are complete types and A and C are
//        injectable.
//
// Implementation notes:
//   * The typedef at the end of some of these macros are just to make it legal
//     to put a semicolon after macro usage.
//
//   * The only purpose of GuicppCtorSignature is to make the signature of the
//     constructor available to the GUICPP_DEFINE macro. GUICPP_DEFINE calls
//     ExternCreateHelper with GuicppCtorSignature(TypeKey<T>()); this makes
//     signature of constructor available to InlineFunctionHelper.
//
//   * The aim of these macros is to implement the GuicppGetDefaultEntry
//     function for each class (e.g. SomeClass).
//     The prototype of GuicppGetDefaultEntry is:
//       implementation-of-table-entry GuicppGetDefaultEntry(
//           TypeKey<SomeClass> typeKey,
//           SomeClass* dummy1,
//           GuicppEmptyGlobalClass dummy2);
//
//   * GuicppGetDefaultEntry takes dummy1, which is pointer to SomeClass.
//     This adds SomeClass's namespace to compiler lookup list while resolving
//     GuicppGetDefaultEntry name. This lets user to invoke inject macros from
//     the class's namespace.
//
//   * It also takes GuicppEmptyGlobalClass which is an empty class defined in
//     global namespace. This adds global namespace to lookup list. This lets
//     user to invoke inject macros from global namespace in cases where
//     invoking from class's namespace is not feasible.
//
//   * GuicppGetDefaultEntry takes typeKey, which makes the signature of the
//     function unique for each class. That is, GuicppGetDefaultEntry for
//     SomeClass has different signature than for OtherClass. This function
//     overloading lets us to call appropriate function depending on type.
//     Note: though dummy1 also does function overloading, it does not always
//     work as intended, because pointer to subclass can matched to pointer to
//     a base class.
//
//
//     For example, GuicppGetDefaultEntry for SomeClass is:
//      SomeTableEntry GuicppGetDefaultEntry(
//          TypeKey<SomeClass> typeKey,
//          const SomeClass* dummy1,
//          GuicppEmptyGlobalClass dummy2) {
//        ... implementation for SomeClass...
//      }
//
//     and GuicppGetDefaultEntry for OtherClass is:
//      SomeTableEntry GuicppGetDefaultEntry(
//          TypeKey<OtherClass> typeKey,
//           const OtherClass* dummy1,
//           GuicppEmptyGlobalClass dummy2) {
//        ... implementation for OtherClass...
//      }
//
//     These are two different functions with the same name overloaded by
//     argument types. Note GuicppGetDefaultEntry is NOT a template function.
//
//     Example:
//          GuicppGetDefaultEntry(TypeKey<T>(), NULL, GuicppEmptyGlobalClass());
//
//     If T = SomeClass, the call will resolve to
//      SomeTableEntry GuicppGetDefaultEntry(
//          TypeKey<SomeClass> typeKey,
//          const SomeClass* dummy1,
//          GuicppEmptyGlobalClass dummy2);
//
//     and if T = OtherClass, the call will resolve to
//      SomeTableEntry GuicppGetDefaultEntry(
//          TypeKey<OtherClass> typeKey,
//          const OtherClass* dummy1,
//          GuicppEmptyGlobalClass dummy2);

#ifndef GUICPP_MACROS_H_
#define GUICPP_MACROS_H_

#include "guicpp/guicpp_annotations.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/internal/guicpp_builder.h"
#include "guicpp/internal/guicpp_types.h"
#include "guicpp/internal/guicpp_util.h"

// Marks a concrete class as injectable and creates a default binding between
// the type and its constructor. That is, if the type is not explicitly bound
// it is instantiated using the specified constructor.
//
// Note: GUICPP_DEFINE() macro must be used along with this macro.
//
// Usage:
//   GUICPP_INJECT_CTOR(ClassName,
//      (Type1 arg1, Type2 arg2, ...)  //  prototype of the constructor
//      );
//
// This macro should be invoked in the same namespace where the argument class
// is defined. OR it can be invoked in the global namespace (This is recommended
// for third party types where you can't edit header files). It is an error to
// invoke this macro in any other namespace.
//
// Note: The constructor arguments can be annotated.
//
// At the point where this macro is invoked:
//   * The "ClassName" can be an incomplete type.
//   * The constructor argument types can be non-injectable
//
// However, ClassName must be a complete type and argument types must be
// injectable at the point where GUICPP_DEFINE is invoked.
//
// Examples:
//   class EmptyClass {};
//   GUICPP_INJECT_CTOR(EmptyClass, ());
//
//   class EmptyClassUser {
//    public:
//     EmptyClassUser(EmptyClass* empty_class);
//     ~EmptyClass();
//   };
//
//   GUICPP_INJECT_CTOR(EmptyClassUser, (EmptyClass* empty_class));
//
// Implementation detail:
//   This defines inline function GuicppCtorSignature. This is used only by
//   ExternCreateHelper function called by GUICPP_DEFINE macro.
#define GUICPP_INJECT_CTOR(T, Args) \
    extern T* GuicppCreateInstanceUsingCtor( \
        guicpp::internal::TypeKey<T>, \
        const guicpp::Injector* injector, \
        const guicpp::internal::LocalContext* local_context); \
    inline guicpp::internal::MacrosHelper::BindToExternFp<T>::Type \
          GuicppGetDefaultEntry(guicpp::internal::TypeKey<T> typeKey, \
                                const T* ns1, GuicppEmptyGlobalClass ns2) { \
      return guicpp::internal::MacrosHelper::BindToExternFp<T>::Type(); \
    } \
    inline guicpp::internal::TypeKey<T* (*) Args> GuicppCtorSignature( \
        guicpp::internal::TypeKey<T>) { \
      return guicpp::internal::TypeKey<T* (*) Args>(); \
    } typedef int GuicppDummyTypedefForMacros

// Defines internal plumbing code required by Guic++ to inject the class
// using specified constructor. This macro should be invoked from source
// file if the class is made injectable using GUICPP_INJECT_CTOR macro.
//
// Usage:
//   GUICPP_DEFINE(ClassName);
//
// This macro should be invoked in some source file and should be done
// from same namespace as corresponding GUICPP_INJECT_CTOR.
//
// At the point where this macro is invoked:
//   * The "ClassName" must be a complete type
//   * The constructor argument types must be injectable
//
// Note: This is not an "inject macro", this is just a supportive macro
// that must be used with GUICPP_INJECT_CTOR which is an inject macro.
//
// Implementation detail:
//  All the required logic is implemented in ExternCreateHelper This function
//  just calls it with proper arguments.
#define GUICPP_DEFINE(T) \
    extern T* GuicppCreateInstanceUsingCtor( \
        guicpp::internal::TypeKey<T> t, \
        const guicpp::Injector* injector, \
        const guicpp::internal::LocalContext* local_context) { \
      return guicpp::internal::MacrosHelper::ExternCreateHelper<T>( \
          GuicppCtorSignature(t), injector, local_context); \
    } typedef int GuicppDummyTypedefForMacros


// Similar to GUICPP_INJECT_CTOR except you won't need to call GUICPP_DEFINE.
// All plumbing code are instantiated inline.
//
// Usage:
//   GUICPP_INJECT_INLINE_CTOR(ClassName, (ArgType1 a1, ArgType2 a2));
//
// At the point where this macro is invoked:
//   * The "ClassName" must be a complete type
//   * The constructor argument types must be injectable
//
// Implementation details:
//   We re-use the macro meant for template types. We just need to put
//   extra () arround T.
#define GUICPP_INJECT_INLINE_CTOR(T, Args) \
    GUICPP_TEMPLATE_INJECT_CTOR((T), Args)

// Marks a type/class as injectable but does not create any default binding.
// Types made injectable using this macro must be explicitly bound before
// Guic++ can instantiate them.
//
// These types must be bound
// explicitly to be able to get an instance.
//
// Usage:
//   GUICPP_INJECTABLE(ClassName);
//
// This macro should be invoked in the same namespace where the argument class
// is defined. OR it can be invoked in the global namespace (This is recommended
// for third party types where you can't edit header files). It is an error to
// invoke this macro in any other namespace.
//
// Guic++ will not attempt to create instances of this type/class. A request
// for an instance of this type will fail, unless some binding is specified.
// For instance, an abstract class can be bound to a concrete sub-class,
// in which case Guic++ will create an instance of the concrete sub-class
// when the abstract class is requested.
//
// This macro can be used with abstract classes or any types which are not
// intended to be directly instantiated by Guic++
//
// At the point where this macro is invoked:
//   * The "ClassName" can be an incomplete type.
//
// Example:
//   class AbstractClass {
//    public:
//     virtual ~AbstractClass();
//     virtual void SomeFunction() = 0;
//   };
//
//   GUICPP_INJECTABLE(AbstractClass);
//
//   class IncompleteAbstractClass;
//   GUICPP_INJECTABLE(IncompleteAbstractClass);
//
// AbstractClass can be bound to a ConcreteSubClass in some module as follows:
//    binder->Bind<AbstractClass, ConcreteSubClass>();
//
// This will make the Guic++ to instantiate ConcreteSubClass when requested for
// AbstractClass.
#define GUICPP_INJECTABLE(T) \
    inline guicpp::internal::InvalidEntry GuicppGetDefaultEntry( \
        guicpp::internal::TypeKey<T> typeKey, const T* ns1, \
        GuicppEmptyGlobalClass ns2) { \
      return guicpp::internal::InvalidEntry(); \
    } typedef int GuicppDummyTypedefForMacros


// Marks a concrete template class as injectable and creates a default binding
// between the type and its constructor. That is, if the type is not explicitly
// bound it is instantiated using the specified constructor.
//
// Usage:
//   template <typename T1, typename T2, ...>
//   GUICPP_TEMPLATE_INJECT_CTOR(
//     (ClassName<T1, T2, ...>),  // Note extra ()s around type
//     (Type1 arg1, Type2 arg2, ...)  //  prototype of the constructor
//   );
//
// This macro should be invoked in the same namespace where the argument class
// is defined. OR it can be invoked in the global namespace (This is recommended
// for third party types where you can't edit header files). It is an error to
// invoke this macro in any other namespace.
//
// At the point where this macro is invoked:
//   * The "ClassName" must be a complete type
//   * All the constructor argument types must be injectable.
//
// Note: The constructor arguments can be annotated.
//
// Example:
//   template <typename T>
//   class SimpleTemplateClass {
//    public:
//     SimpleTemplateClass(T* tvalue);
//   };
//
//   template <typename T>
//   GUICPP_TEMPLATE_INJECT_CTOR((SimpleTemplateClass<T>), (T* tvalue));
//
#define GUICPP_TEMPLATE_INJECT_CTOR(T, Args) \
    inline typename guicpp::internal::MacrosHelper::BindToInlineFp< \
           GUICPP_RM_PARENTHESES_ T, GUICPP_RM_PARENTHESES_ T* (*) Args>::Type \
        GuicppGetDefaultEntry( \
            guicpp::internal::TypeKey<GUICPP_RM_PARENTHESES_ T> typeKey, \
            const GUICPP_RM_PARENTHESES_ T* ns1, GuicppEmptyGlobalClass ns2) { \
    return typename guicpp::internal::MacrosHelper::BindToInlineFp< \
          GUICPP_RM_PARENTHESES_ T, \
          GUICPP_RM_PARENTHESES_ T* (*) Args>::Type(); \
    } typedef int GuicppDummyTypedefForMacros

// Marks a template class as injectable but does create any default binding
// (actually creates a "invalid" default binding). These types must be bound
// explicitly to be able to get an instance.
//
// Usage:
//   template <typename T1, typename T2, ...>
//   GUICPP_TEMPLATE_INJECTABLE(
//     (ClassName<T1, T2, ...>),  // Note extra ()s around type
//   );
//
// This macro should be invoked in the same namespace where the argument class
// is defined. OR it can be invoked in the global namespace (This is recommended
// for third party types where you can't edit header files). It is an error to
// invoke this macro in any other namespace.
//
// At the point where this macro is invoked:
//   * The "ClassName" can be an incomplete type
//
// Example:
//   template <typename T>
//   class SimpleTemplateAbstractClass {
//    public:
//     virtual void SomeAbstractFunction() = 0;
//   };
//
//   template <typename T>
//   GUICPP_TEMPLATE_INJECTABLE((SimpleTemplateAbstractClass<T>));
//
#define GUICPP_TEMPLATE_INJECTABLE(T) \
    inline guicpp::internal::InvalidEntry GuicppGetDefaultEntry( \
        guicpp::internal::TypeKey<GUICPP_RM_PARENTHESES_ T> typeKey, \
        const GUICPP_RM_PARENTHESES_ T* ns1, GuicppEmptyGlobalClass ns2) { \
      return guicpp::internal::InvalidEntry(); \
    } typedef int GuicppDummyTypedefForMacros

#endif  // GUICPP_MACROS_H_

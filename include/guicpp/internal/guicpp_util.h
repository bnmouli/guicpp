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


#ifndef GUICPP_UTIL_H_
#define GUICPP_UTIL_H_

#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_annotations.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/internal/guicpp_types.h"

namespace guicpp {
class Injector;

namespace internal {
class LocalContext;

// The template class used to get unique if (a.k.a TypeId) for each type.
template <typename T>
class TypeIdProvider {
 public:
  static TypeId GetTypeId() {
    return &type_id_;
  }
 private:
  static uint32 type_id_;

  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(TypeIdProvider);
};

// C++ compiler as per standard, will ensure that there is exactly one instance
// of type_id_ for each type T. [Sections 3.2, 14.6.3 and 14.6.2]
//
// This feature is used here to get one unique id for each type.
template<typename T>
uint32 TypeIdProvider<T>::type_id_ = 0;


// The Error class is used to give more readable compiler errors. On SomeError
// with SomeType we call
//   Error<SomeType>::SomeError();
// And compiler gives error that reads
//   'SomeError' is not a member of 'Error<SomeType>'
// Which user can read as 'Error: SomeError in SomeType'
//
// Note: This must be used in functions which are not instantiated in normal
// circumstances.
template <typename T>
class Error {
 private:
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(Error);
};

// Invalid<T>() returns an invalid value of type T. This is useful when a value
// of type T is needed for compilation, but the statement will not really be
// executed (or we don't care if the statement crashes).
template <typename T>
inline T Invalid() {
  GUICPP_DCHECK_(false) << "This Invalid<T>() should be unreachable code";
  return static_cast<T(*)()>(NULL)();
}

typedef Annotations<> EmptyAnnotations;

// This class is used by AtUtil when an attribute is not annotated.
// This class is similar to "At" class with zero annotations. We do not allow
// user to use At with zero annotations for readability.
template <typename T>
class NotAnnotated {
 public:
  typedef T ArgType;
  typedef EmptyAnnotations Annotations;

 private:
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(NotAnnotated);
};

// This utility class lets us deal with arguments which may be annotated.
class AtUtil {
 public:
  template <typename T>
  class GetTypes {
   private:
    // We add "remove_reference" just to avoid compiler error in case T is a
    // reference. Explicit check for reference will make overall condition
    // evaluate to false. We could also use is_convertible<T&, AtBaseClass&>
    // but that will need "T" to be a complete type.
    typedef typename guicpp::internal::if_<
        (!guicpp::internal::is_reference<T>::value) &&
        guicpp::internal::is_convertible<
            typename guicpp::internal::remove_reference<T>::type*,
            AtBaseClass*>::value,
         // true
            T,
         // else
            NotAnnotated<T> >::type AnnotatedType;

   public:
    typedef typename AnnotatedType::ArgType ArgType;
    typedef typename guicpp::internal::remove_cv<ArgType>::type ActualType;
    typedef typename AnnotatedType::Annotations Annotations;

   private:
    GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(GetTypes);
  };

  // Gets value of annotation of Lookup type.
  // Getting it is simple, it is just Entries<Lookup>::type but we need to check
  // if annotation of Lookup type is specified before to referring to "type"
  template <typename Annotations,
            typename Lookup,
            typename DefaultValue = typename Lookup::AnnotationDefaultValue>
  class GetAt {
   public:
    typedef typename guicpp::internal::if_<
      guicpp::internal::is_convertible<
          typename Annotations::template Entries<NoArg>*,
          AtEntryBase<Lookup>*>::value,
       // true
          typename Annotations::template Entries<Lookup>,
       // false
          identity_<DefaultValue> >::type::type Type;

   private:
    GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(GetAt);
  };

 private:
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(AtUtil);
};

}  // namespace internal
}  // namespace guicpp

#endif  // GUICPP_UTIL_H_

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



#ifndef GUICPP_PORT_H_
#define GUICPP_PORT_H_

#include <stdlib.h>   // For abort

#include <map>
#include <iostream>
#include <vector>

namespace guicpp {
namespace internal {

using std::map;
using std::make_pair;

// The GUICPP_COMPILE_ASSERT_ macro can be used to verify that a compile time
// expression is true. For example, you could use it to verify the size of a
// static array:
//
//   GUICPP_COMPILE_ASSERT_(ARRAYSIZE(content_type_names) == CONTENT_NUM_TYPES,
//                  content_type_names_incorrect_size);
//
// or to make sure a struct is smaller than a certain size:
//
//   GUICPP_COMPILE_ASSERT_(sizeof(foo) < 128, foo_too_large);
//
// The second argument to the macro is the name of the variable. If
// the expression is false, most compilers will issue a warning/error
// containing the name of the variable.

template <bool>
struct CompileAssert {};

#define GUICPP_COMPILE_ASSERT_(expr, msg) \
  typedef guicpp::internal::CompileAssert<(bool(expr))> msg[bool(expr) ? 1 : -1]

// Implementation details of GUICPP_COMPILE_ASSERT_:
//
// - GUICPP_COMPILE_ASSERT_ works by defining an array type that has -1
//   elements (and thus is invalid) when the expression is false.
//
// - The simpler definition
//
//     #define GUICPP_COMPILE_ASSERT_(expr, msg) \
//                      typedef char msg[(expr) ? 1 : -1]
//
//   does not work, as gcc supports variable-length arrays whose sizes
//   are determined at run-time (this is gcc's extension and not part
//   of the C++ standard).  As a result, gcc fails to reject the
//   following code with the simple definition:
//
//     int foo;
//     GUICPP_COMPILE_ASSERT_(foo, msg); // not supposed to compile as foo is
//                               // not a compile-time constant.
//
// - By using the type CompileAssert<(bool(expr))>, we ensures that
//   expr is a compile-time constant.  (Template arguments must be
//   determined at compile-time.)
//
// - The outer parentheses in CompileAssert<(bool(expr))> are necessary
//   to work around a bug in gcc 3.4.4 and 4.0.1.  If we had written
//
//     CompileAssert<bool(expr)>
//
//   instead, these compilers will refuse to compile
//
//     GUICPP_COMPILE_ASSERT_(5 > 0, some_message);
//
//   (They seem to think the ">" in "5 > 0" marks the end of the
//   template argument list.)
//
// - The array size is (bool(expr) ? 1 : -1), instead of simply
//
//     ((expr) ? 1 : -1).
//
//   This is to avoid running into a bug in MS VC 7.1, which
//   causes ((0.0) ? 1 : -1) to incorrectly evaluate to 1.


// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
//
// For disallowing only assign or copy, write the code directly, but declare
// the intend in a comment, for example:
// void operator=(const TypeName&);  // DISALLOW_ASSIGN
// Note, that most uses of DISALLOW_ASSIGN and DISALLOW_COPY are broken
// semantically, one should either use disallow both or neither. Try to
// avoid these in new code.
#define GUICPP_DISALLOW_COPY_AND_ASSIGN_(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

// A macro to disallow all the implicit constructors, namely the
// default constructor, copy constructor and operator= functions.
//
// This should be used in the private: declarations for a class
// that wants to prevent anyone from instantiating it. This is
// especially useful for classes containing only static methods.
#define GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(TypeName) \
  TypeName();                                    \
  GUICPP_DISALLOW_COPY_AND_ASSIGN_(TypeName)

// The arraysize(arr) macro returns the # of elements in an array arr.
// The expression is a compile-time constant, and therefore can be
// used in defining new arrays, for example.  If you use arraysize on
// a pointer by mistake, you will get a compile-time error.

// This template function declaration is used in defining arraysize.
// Note that the function doesn't need an implementation, as we only
// use its type.
template <typename T, long N>
char (&ArraySizeHelper(T (&array)[N]))[N];

// That gcc wants both of these prototypes seems mysterious. VC, for
// its part, can't decide which to use (another mystery). Matching of
// template overloads: the final frontier.
#ifndef COMPILER_MSVC
template <typename T, long N>
char (&ArraySizeHelper(const T (&array)[N]))[N];
#endif

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

using std::vector;

// Define a small subset of tr1 type traits. The traits we define are:
//   is_integral
//   is_pointer
//   is_reference
//   remove_const
//   remove_volatile
//   remove_cv
//   remove_reference
//   add_reference
//   remove_pointer
//   is_same
//   is_convertible
// We can add more type traits as required.

// Types small_ and big_ are guaranteed such that sizeof(small_) <
// sizeof(big_)
typedef char small_;

struct big_ {
  small_ dummy[2];
};

// Identity metafunction.
template <class T>
struct identity_ {
  typedef T type;
};

template<class T, T v>
struct integral_constant {
  static const T value = v;
  typedef T value_type;
  typedef integral_constant<T, v> type;
};

template <class T, T v> const T integral_constant<T, v>::value;

// Abbreviations: true_type and false_type are structs that represent boolean
// true and false values.
typedef integral_constant<bool, true>  true_type;
typedef integral_constant<bool, false> false_type;

// if_ is a templatized conditional statement.
// if_<cond, A, B> is a compile time evaluation of cond.
// if_<>::type contains A if cond is true, B otherwise.
template<bool cond, typename A, typename B>
struct if_ {
  typedef A type;
};

template<typename A, typename B>
struct if_<false, A, B> {
  typedef B type;
};


// is_pointer is false except for pointer types. A cv-qualified type (e.g.
// "int* const", as opposed to "int const*") is cv-qualified if and only if
// the underlying type is.
template <class T> struct is_pointer : false_type { };
template <class T> struct is_pointer<T*> : true_type { };
template <class T> struct is_pointer<const T> : is_pointer<T> { };
template <class T> struct is_pointer<volatile T> : is_pointer<T> { };
template <class T> struct is_pointer<const volatile T> : is_pointer<T> { };

// Specified by TR1 [4.7.4] Pointer modifications.
template<typename T> struct remove_pointer { typedef T type; };
template<typename T> struct remove_pointer<T*> { typedef T type; };
template<typename T> struct remove_pointer<T* const> { typedef T type; };
template<typename T> struct remove_pointer<T* volatile> { typedef T type; };
template<typename T> struct remove_pointer<T* const volatile> {
  typedef T type; };

// is_reference evaluates to true_type if T is a reference,
// to false_type otherwise
template<typename T> struct is_reference : false_type {};
template<typename T> struct is_reference<T&> : true_type {};

// Specified by TR1 [4.7.2] Reference modifications.
template<typename T> struct remove_reference { typedef T type; };
template<typename T> struct remove_reference<T&> { typedef T type; };

template <typename T> struct add_reference { typedef T& type; };
template <typename T> struct add_reference<T&> { typedef T& type; };


// Specified by TR1 [4.7.1]
template<typename T> struct remove_const { typedef T type; };
template<typename T> struct remove_const<T const> { typedef T type; };
template<typename T> struct remove_volatile { typedef T type; };
template<typename T> struct remove_volatile<T volatile> { typedef T type; };
template<typename T> struct remove_cv {
  typedef typename remove_const<typename remove_volatile<T>::type>::type type;
};

// Specified by TR1 [4.6] Relationships between types
template<typename T, typename U> struct is_same : public false_type { };
template<typename T> struct is_same<T, T> : public true_type { };

template <typename From, typename To>
struct ConvertHelper {
  static small_ Test(To);
  static big_ Test(...);
  static From Create();
};

// Inherits from true_type if From is convertible to To, false_type otherwise.
template <typename From, typename To>
struct is_convertible : integral_constant<bool,
  sizeof(ConvertHelper<From, To>::Test(ConvertHelper<From, To>::Create()))
                        == sizeof(small_)> {};


template <class T> struct is_class_or_union {
  template <class U> static small_ tester(void (U::*)());
  template <class U> static big_ tester(...);
  static const bool value = sizeof(tester<T>(0)) == sizeof(small_);
};


// Used to supress warning on constant expression.
// TODO(bnmouli): Do I need to make it non-inline?
inline bool IsTrue(bool b) { return b; }

// LOGGING
enum GuicppLogSeverity {
  GUICPP_INFO,
  GUICPP_WARNING,
  GUICPP_ERROR,
  GUICPP_FATAL
};

// Formats log entry severity, provides a stream object for streaming the
// log message, and terminates the message with a newline when going out of
// scope.
class GuicppLog {
 public:
  GuicppLog(GuicppLogSeverity severity, const char* file, int line)
      : severity_(severity), file_(file), line_(line) {
    GetStream() << "Location [" << file << "@" << line << "]\n";
  }

  // Flushes the buffers and, if severity is GUICPP_FATAL, aborts the program.
  ~GuicppLog() {
    GetStream() << std::endl;
    if (severity_ == GUICPP_FATAL) {
      GetStream().flush();
      abort();
    }
  }

  ::std::ostream& GetStream() { return ::std::cerr; }

 private:
  const GuicppLogSeverity severity_;
  const char *file_;
  int line_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(GuicppLog);
};

#define GUICPP_LOG_(severity) \
    ::guicpp::internal::GuicppLog(::guicpp::internal::GUICPP_##severity, \
                                  __FILE__, __LINE__).GetStream()

// The GNU compiler emits a warning if nested "if" statements are followed by
// an "else" statement and braces are not used to explicitly disambiguate the
// "else" binding.  This leads to problems with code like:
//
//   if (gate)
//     ASSERT_*(condition) << "Some message";
//
// The "switch (0) case 0:" idiom is used to suppress this.
#ifdef __INTEL_COMPILER
#define GUICPP_AMBIGUOUS_ELSE_BLOCKER_
#else
#define GUICPP_AMBIGUOUS_ELSE_BLOCKER_ switch (0) case 0: default:
#endif

#define GUICPP_CHECK_(condition) \
    GUICPP_AMBIGUOUS_ELSE_BLOCKER_ \
    if (::guicpp::internal::IsTrue(condition)) \
      ; \
    else \
      GUICPP_LOG_(FATAL) << "Condition " #condition " failed. "

#define GUICPP_CHECK_EQ_(lhs, rhs) \
    GUICPP_CHECK_((lhs) == (rhs))

#define GUICPP_DCHECK_ GUICPP_CHECK_
#define GUICPP_DCHECK_EQ_ GUICPP_CHECK_EQ_

// This MUST be ported
class Mutex {
 public:
  Mutex(): locked_(false) {}
  ~Mutex() {}

  void Lock() {
    GUICPP_CHECK_(!locked_);
    locked_ = true;
  }

  void Unlock() {
    GUICPP_CHECK_(locked_);
    locked_ = false;
  }

 private:
  bool locked_;
};

class MutexLock {
 public:
  explicit MutexLock(Mutex* mu): mu_(mu) {
    mu_->Lock();
  }

  ~MutexLock() {
    mu_->Unlock();
  }

 private:
  Mutex* mu_;
  GUICPP_DISALLOW_COPY_AND_ASSIGN_(MutexLock);
};

typedef MutexLock WriterMutexLock;
typedef MutexLock ReaderMutexLock;

}  // namespace internal

// INTERNAL IMPLEMENTATION - DO NOT USE IN USER CODE.

// This implementation of scoped_ptr is PARTIAL - it only contains
// enough stuff to satisfy Guic++'s need.
template <typename T>
class scoped_ptr {
 public:
  typedef T element_type;

  explicit scoped_ptr(T* p = NULL) : ptr_(p) {}
  ~scoped_ptr() { reset(); }

  T& operator*() const { return *ptr_; }
  T* operator->() const { return ptr_; }
  T* get() const { return ptr_; }

  void reset(T* p = NULL) {
    if (p != ptr_) {
      // Makes sure T is a complete type.
      if (::guicpp::internal::IsTrue(sizeof(T) > 0)) {
        delete ptr_;
      }
      ptr_ = p;
    }
  }

 private:
  T* ptr_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(scoped_ptr);
};


template <typename T>
class scoped_array {
 public:
  typedef T element_type;

  explicit scoped_array(T* p = NULL) : ptr_(p) {}
  ~scoped_array() { reset(); }

  T& operator[](int i) const { return ptr_[i]; }
  T* get() const { return ptr_; }

  void reset(T* p = NULL) {
    if (p != ptr_) {
      // Makes sure T is a complete type.
      if (::guicpp::internal::IsTrue(sizeof(T) > 0)) {
        delete[] ptr_;
      }
      ptr_ = p;
    }
  }

 private:
  T* ptr_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(scoped_array);
};


// Use implicit_cast as a safe version of static_cast for upcasting in
// the type hierarchy (e.g. casting a Foo* to a SuperclassOfFoo* or a
// const Foo*).  When you use implicit_cast, the compiler checks that
// the cast is safe.  Such explicit implicit_casts are necessary in
// surprisingly many situations where C++ demands an exact type match
// instead of an argument type convertable to a target type.
//
// The syntax for using implicit_cast is the same as for static_cast:
//
//   implicit_cast<ToType>(expr)
//
// implicit_cast would have been part of the C++ standard library,
// but the proposal was submitted too late.  It will probably make
// its way into the language in the future.
template<typename To>
inline To implicit_cast(To x) { return x; }

// When you upcast (that is, cast a pointer from type Foo to type
// SuperclassOfFoo), it's fine to use implicit_cast<>, since upcasts
// always succeed.  When you downcast (that is, cast a pointer from
// type Foo to type SubclassOfFoo), static_cast<> isn't safe, because
// how do you know the pointer is really of type SubclassOfFoo?  It
// could be a bare Foo, or of type DifferentSubclassOfFoo.  Thus,
// when you downcast, you should use this macro.  In debug mode, we
// use dynamic_cast<> to double-check the downcast is legal (we die
// if it's not).  In normal mode, we do the efficient static_cast<>
// instead.  Thus, it's important to test in debug mode to make sure
// the cast is legal!
//    This is the only place in the code we should use dynamic_cast<>.
// In particular, you SHOULDN'T be using dynamic_cast<> in order to
// do RTTI (eg code like this:
//    if (dynamic_cast<Subclass1>(foo)) HandleASubclass1Object(foo);
//    if (dynamic_cast<Subclass2>(foo)) HandleASubclass2Object(foo);
// You should design the code some other way not to need this.
template<typename To, typename From>  // use like this: down_cast<T*>(foo);
inline To down_cast(From* f) {  // so we only accept pointers
  // Ensures that To is a sub-type of From *.  This test is here only
  // for compile-time type checking, and has no overhead in an
  // optimized build at run-time, as it will be optimized away
  // completely.
  if (false) {
    const To to = NULL;
    ::guicpp::implicit_cast<From*>(to);
  }

#if GUICPP_HAS_RTTI
  // RTTI: debug mode only!
  GUICPP_CHECK_(f == NULL || dynamic_cast<To>(f) != NULL);
#endif
  return static_cast<To>(f);
}

typedef long int32;
typedef unsigned long uint32;

// TODO(bnmouli): PORT This
class GoogleOnceDynamic {
 public:
  GoogleOnceDynamic(): called_(false) {}
  ~GoogleOnceDynamic() {}

  template <typename T>
  void Init(void (*fp)(T*), T *arg) {
    if (!called_) {
      fp(arg);
      called_ = true;
    }
  }

 private:
  bool called_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(GoogleOnceDynamic);
};

}  // namespace guicpp

namespace guicpp_test {
using guicpp::scoped_ptr;
using guicpp::scoped_array;
}

#endif  // GUICPP_PORT_H_

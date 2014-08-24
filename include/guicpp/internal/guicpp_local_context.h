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


// This file defines LocalContext which is used to pass some
// local context to all functions in a call tree.
//
// An instance of LocalContext is created (local variable on stack)
// when user calls Injector::Get() or Factory::Get() method and passed
// to all functions in the call tree.

#ifndef GUICPP_LOCAL_CONTEXT_H_
#define GUICPP_LOCAL_CONTEXT_H_

#include "guicpp/internal/guicpp_port.h"
#include "guicpp/internal/guicpp_table.h"
#include "guicpp/internal/guicpp_types.h"

namespace guicpp {
class Injector;

namespace internal {
class LocalContext;

// Used to map factory argument-type to a argument-value. This mapping is
// stored in LocalContext and is never part of injector (BindTable).
template <typename T>
class FactoryArgumentEntry: public TableEntry<T> {
 public:
  explicit FactoryArgumentEntry(const T& object): object_(object) {}
  virtual ~FactoryArgumentEntry() {}

  virtual T Get(const Injector* /* injector */,
                const LocalContext* /* local_context */) const {
    return object_;
  }

  virtual typename TableEntryBase::BindType GetBindType() const {
    return TableEntryBase::BIND_FACTORY_ARGUMENT;
  }

 private:
  // We store reference to object instead of copying the object itself.
  // Since factory arguments are guaranteed to exist till it returns,
  // storing reference should not be a problem.
  const T& object_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(FactoryArgumentEntry);
};


struct TypeIdArgumentPair {
  TypeId type_id;
  const TableEntryBase* entry;
};

// LocalContext holds an array of arguments passed to factory (if any).
// And also is a place holder for any context that may be required in
// future.
class LocalContext {
 public:
  // This constructor is used by RealFactory class
  //
  // @param args an array of TypeIdArgumentPair.
  // @param num_args number entries in that array.
  LocalContext(const TypeIdArgumentPair* args, int num_args)
      : args_(args), num_args_(num_args) {
    // Check for duplications?
    // Each element in array must have distinct type id.
  }

  LocalContext(): args_(NULL), num_args_(0) {}

  const TableEntryBase* FindEntry(TypeId tid) const;

 private:
  const TypeIdArgumentPair* args_;
  int num_args_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(LocalContext);
};

}  // namespace internal
}  // namespace guicpp

#endif  // GUICPP_LOCAL_CONTEXT_H_

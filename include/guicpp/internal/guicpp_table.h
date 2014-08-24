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


// This file declares the classes that hold all binding information.

#ifndef GUICPP_TABLE_H_
#define GUICPP_TABLE_H_

#include "guicpp/internal/guicpp_port.h"
#include "guicpp/internal/guicpp_types.h"
#include "guicpp/internal/guicpp_util.h"

namespace guicpp {
class Injector;

namespace internal {
class LocalContext;

// Bind table keeps information about various sorts of bindings specified
// by the user. It is used by injector to get object instances. The table
// keeps pointers to TableEntryBase. TableEntryBase is an interface
// independent of the actual type of the bound instance. Injector uses TypeId
// to look up an entry, then casts that entry to the appropriate TableEntry<T>
// (injector knows about type T, corresponding to TypeId) and calls
// TableEntry<T>::Get() to obtain an instance of T.
class TableEntryBase {
 public:
  // These are the types of binding supported by Guic++
  enum BindType {
    // Binds a type to its constructor.
    // This binding is not stored in bind table; Instead, it is the return type
    // of function implemented by GUICPP_INJECT_CTOR macro.
    BIND_TO_CTOR,

    // Binds a source type to a destination type.
    // This makes Guic++ treat any request to "source type" as the request to
    // "destination type".
    BIND_TO_TYPE,

    // Binds a pointer type to an instance.
    BIND_TO_INSTANCE,

    // Binds a value type to the value.
    // A value type is any non-pointer "assignable" type
    // (E.g. basic types such as int and double).
    BIND_TO_VALUE,

    // Binds a reference to pointed instance.
    BIND_TO_POINTED,

    // This binds a type T to a custom factory implemented by user.
    // When requested for an instance of T, Get() method of the factory is
    // called by injecting all the argument required by factory.
    BIND_TO_PROVIDER,

    // Used for factory arguments.
    // This binds type of argument to the value passed to the factory. The
    // values are picked from local_context filled by factory's Get() method.
    // Note: these bindings are created by factory implementation and in a
    // separate table in LocalContext.
    BIND_FACTORY_ARGUMENT,

    INVALID_BIND
  };

  virtual ~TableEntryBase() {}

  // Get unique id associated with the type.
  virtual TypeId GetTypeId() const = 0;

  // Returns category of the bound type.
  virtual TypesCategory::Enum GetCategory() const = 0;

  // Returns true if bound type is a constant pointer or reference.
  virtual bool IsConst() const = 0;

  // Returns type of binding.
  virtual BindType GetBindType() const = 0;

 protected:
  TableEntryBase() {}
};

// This is base class for a table entry having type specific operations.
// These APIs are independent of how they are bound. The methods declared in
// this class are sufficient to create any object (and their dependencies).
template <typename T>
class TableEntry: public TableEntryBase {
 public:
  virtual ~TableEntry() {}

  virtual T Get(const Injector* injector,
                const LocalContext* local_context) const = 0;

  // Since the type is known, these APIs declared in base class are
  // implemented here.
  virtual TypeId GetTypeId() const {
    typedef typename TypeInfo<T>::TypeSpecifier TypeSpecifier;
    return TypeIdProvider<TypeSpecifier>::GetTypeId();
  }

  virtual TypesCategory::Enum GetCategory() const {
    return TypeInfo<T>::Category::value;
  }

  virtual bool IsConst() const {
    return TypeInfo<T>::IsConst::value;
  }

 protected:
  TableEntry() {}
};


// Used by GUICPP_INJECTABLE for abstract types.
class InvalidEntry: public TableEntryBase {
 public:
  // BindToCtor assumes ownership of provider.
  InvalidEntry() {}
  virtual ~InvalidEntry() {}

  virtual TableEntryBase::BindType GetBindType() const {
    return TableEntryBase::INVALID_BIND;
  }

  // Following are unreached code.
  virtual TypeId GetTypeId() const {
    return Invalid<TypeId>();
  }

  virtual TypesCategory::Enum GetCategory() const {
    return Invalid<TypesCategory::Enum>();
  }

  virtual bool IsConst() const {
    return Invalid<bool>();
  }
};

// The Bind Table, this maps type IDs to TableEntryBase objects.
// This table is created during creation of injector and referred
// many times while creating objects.
class BindTable {
 public:
  BindTable();
  ~BindTable();  // No class should inherit from BindTable

  // Finds and returns entry associated with bindId.
  // This return null if no entry found.
  const TableEntryBase* FindEntry(TypeId bindId) const;

  // Adds entry for bindId.
  // If the table already has an entry for bindId, the entry is not added to the
  // bind table. The ownership of the entry is assumed even in error cases, that
  // is entry is added to cleanup list even in error cases.
  bool AddEntry(TypeId bindId, const TableEntryBase* entry);

  // Adds an entry cleanup list.
  // AddEntry() internally calls AddToCleanupList(). This is called only for
  // entries that are not added to bind_map_ but needs to deleted at cleanup
  // time. All entries are deleted in reverse order of addition.
  void AddToCleanupList(const TableEntryBase* entry);

 private:
  map<TypeId, const TableEntryBase*> bind_map_;

  // This vector maintains entries in the order they are added.
  vector<const TableEntryBase*> cleanup_list_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(BindTable);
};

template <typename T>
class TableEntryReader {
 public:
  // Returns an instance of T from bind table entry.
  // The bound type must be convertible to T.
  static T Get(const TableEntryBase* entry_base,
               const Injector* injector,
               const LocalContext* local_context);

 private:
  typedef typename TypeInfo<T>::TypeSpecifier TypeSpecifier;

  // An instance of this class used internally by Get() method.
  TableEntryReader(const TableEntryBase* entry_base,
                   const Injector* injector,
                   const LocalContext* local_context)
      : entry_base_(entry_base), injector_(injector),
        local_context_(local_context) {}

  // Helper functions used by Get() method.

  // If requested type is same as BoundType, no conversion required.
  T GetAndCast(TypeKey2<T, T> /* type cast */) const {
    return GetBoundType<T>();
  }

  // Following specialization are to allow these conversions
  //  1. non-const pointer to const pointer
  //  2. non-const reference to const reference
  //  3. reference to value
  //  4. const-reference to value
  //
  //  Note: It is not possible (easy) to support value to reference as it may

  //  non-const pointer to const pointer
  const TypeSpecifier* GetAndCast(
      TypeKey2<const TypeSpecifier*, TypeSpecifier*> /* type cast */) const {
    return GetBoundType<TypeSpecifier*>();
  }

  // non-const reference to const reference
  const TypeSpecifier& GetAndCast(
      TypeKey2<const TypeSpecifier&, TypeSpecifier&> /* type cast */) const {
    return GetBoundType<TypeSpecifier&>();
  }

  // reference to value
  TypeSpecifier GetAndCast(
      TypeKey2<TypeSpecifier, TypeSpecifier&> /* type cast */) const {
    return GetBoundType<TypeSpecifier&>();
  }

  // const reference to value
  TypeSpecifier GetAndCast(
      TypeKey2<TypeSpecifier, const TypeSpecifier&> /* type cast */) const {
    return GetBoundType<TypeSpecifier&>();
  }

  // Value to const reference is not supported see
  // go/guicpp-value-to-reference for details.

  // Error handler. If the bound type is not convertible to requested type,
  // logs error and fails fatally.
  template <typename BoundType>
  T GetAndCast(TypeKey2<T, BoundType> /* type cast */) const {
    GUICPP_LOG_(FATAL) << "Can not convert BoundType["
                       << CategoryString<BoundType>()
                       << "T] to RequestedType[" << CategoryString<T>() << "T]";
    return Invalid<T>();  // Unreachable code.
  }

  // Gets the value from table entry.
  template <typename BoundType>
  BoundType GetBoundType() const {
    const TableEntry<BoundType>* entry =
        down_cast<const TableEntry<BoundType>*>(entry_base_);
    return entry->Get(injector_, local_context_);
  }

  // Returns a string that describes categories of P. For example if P is a
  // const pointer this returns "const T*". This is used for logging.
  template <typename P>
  static const char* CategoryString();

  const TableEntryBase* entry_base_;
  const Injector* injector_;
  const LocalContext* local_context_;

  GUICPP_DISALLOW_COPY_AND_ASSIGN_(TableEntryReader);
};


// -- Implementation --

// static
template <typename T>
inline T TableEntryReader<T>::Get(const TableEntryBase* entry_base,
                                  const Injector* injector,
                                  const LocalContext* local_context) {
  if (entry_base->GetBindType() == TableEntryBase::INVALID_BIND) {
    GUICPP_LOG_(FATAL) << "This type can not be instantiated, missing binding";
  }

  // TypeId of T and the bound type must be same.
  GUICPP_DCHECK_EQ_(TypeIdProvider<TypeSpecifier>::GetTypeId(),
                    entry_base->GetTypeId());

  TableEntryReader<T> reader(entry_base, injector, local_context);

  switch (entry_base->GetCategory()) {
    case TypesCategory::IS_VALUE:
      return reader.GetAndCast(TypeKey2<T, TypeSpecifier>());

    case TypesCategory::IS_POINTER:
      if (entry_base->IsConst()) {
        return reader.GetAndCast(TypeKey2<T, const TypeSpecifier*>());
      }

      return reader.GetAndCast(TypeKey2<T, TypeSpecifier*>());

    case TypesCategory::IS_REFERENCE:
      if (entry_base->IsConst()) {
        return reader.GetAndCast(TypeKey2<T, const TypeSpecifier&>());
      }

      return reader.GetAndCast(TypeKey2<T, TypeSpecifier&>());

    default:
      GUICPP_CHECK_(false) << "Unsupported type";
  }

  return Invalid<T>();
}

// TODO(bnmouli): ADDNAME Logging type names is not really nice in this version
// of Guic++. Need to do a total revamp.
const char* GetCategoryString(TypesCategory::Enum category, bool is_const);

// Returns a string that describes categories of P.
//
// TODO(bnmouli): ADDNAME Once "GetName()" is added, this function should be
// replaced by the one which can print the type as string.
template <typename T>
template <typename P>
const char* TableEntryReader<T>::CategoryString() {
  return GetCategoryString(TypeInfo<P>::Category::value,
                           TypeInfo<P>::IsConst::value);
}

}  // namespace internal
}  // namespace guicpp

#endif  // GUICPP_TABLE_H_

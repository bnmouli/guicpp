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


// This file defines the methods in classes declared in table.h.

#include "guicpp/internal/guicpp_table.h"

#include <map>
#include <utility>

#include "guicpp/internal/guicpp_inject_util.h"

namespace guicpp {
namespace internal {
BindTable::BindTable() {
}

BindTable::~BindTable() {
  for (vector<const TableEntryBase*>::reverse_iterator riter =
       cleanup_list_.rbegin(); riter != cleanup_list_.rend(); ++riter) {
    delete *riter;
  }
}

// Finds and returns entry associated with bindId.
const TableEntryBase* BindTable::FindEntry(TypeId bindId) const {
  map<TypeId, const TableEntryBase*>::const_iterator iter =
      bind_map_.find(bindId);

  if (iter == bind_map_.end()) {
    return NULL;
  }

  return iter->second;
}

// Adds entry for bindId.
bool BindTable::AddEntry(TypeId bindId, const TableEntryBase* entry) {
  AddToCleanupList(entry);

  if (bind_map_.insert(make_pair(bindId, entry)).second) {
    return true;
  }

  return false;
}

// Adds an entry cleanup list.
void BindTable::AddToCleanupList(const TableEntryBase* entry) {
  cleanup_list_.push_back(entry);
}

// Returns a string that describes category of the type.
const char* GetCategoryString(TypesCategory::Enum category, bool is_const) {
  switch (category) {
    case TypesCategory::IS_VALUE: {
      return "value of type ";
    }

    case TypesCategory::IS_POINTER:
      if (is_const) {
        return "const pointer to ";
      } else {
        return "pointer to ";
      }

    case TypesCategory::IS_REFERENCE:
      if (is_const) {
        return "const reference to ";
      } else {
        return "reference to ";
      }
  }

  return NULL;
}

}  // namespace internal
}  // namespace guicpp

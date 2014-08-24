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


// Test for classes declared in table.h.

#include "guicpp/internal/guicpp_table.h"

#include <string>

#include "include/gmock/gmock.h"
#include "include/gtest/gtest.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_macros.h"
#include "include/guicpp_test_helper.h"
#include "guicpp/internal/guicpp_util.h"

namespace guicpp {
namespace internal {
using guicpp_test::MockCleanupAction;
using guicpp_test::TestBaseClass;
using guicpp_test::TestDeleteMarker;
using guicpp_test::TestPointerEntry;
using guicpp_test::TestSimpleInjectableClass;
using guicpp_test::TestValueEntry;
using testing::_;
using testing::InSequence;
using testing::MockFunction;

// Tests methods of TableEntry<T> via subclasses TestPointerEntry and
// TestValueEntry.
TEST(TableEntryTest, GetTypeId_ReturnsTypeIdAsGivenByContextUtil) {
  TestPointerEntry<TestSimpleInjectableClass> pointer_entry(NULL);
  EXPECT_EQ(TypesCategory::IS_POINTER, pointer_entry.GetCategory());

  TestValueEntry<TestSimpleInjectableClass> value_entry(
      TestSimpleInjectableClass(10));
  EXPECT_EQ(TypesCategory::IS_VALUE, value_entry.GetCategory());

  // Both pointer type and non-pointer entries take same type id.
  EXPECT_EQ(TypeIdProvider<TestSimpleInjectableClass>::GetTypeId(),
            pointer_entry.GetTypeId());

  EXPECT_EQ(TypeIdProvider<TestSimpleInjectableClass>::GetTypeId(),
            value_entry.GetTypeId());
}

// Test implementation of TableEntry that helps us to check when entries are
// deleted.
class DeleteCheckerEntry: public TableEntry<TestSimpleInjectableClass*> {
 public:
  explicit DeleteCheckerEntry(TestDeleteMarker* marker)
      : delete_marker_(marker) {}

  virtual ~DeleteCheckerEntry() {
    delete_marker_->Call(this);
  }

  TestSimpleInjectableClass* Get(
      const Injector* /* injector */,
      const LocalContext* /* local_context */) const {
    return NULL;
  }

  TableEntryBase::BindType GetBindType() const {
    // We can return any value except INVALID_BIND
    return TableEntryBase::BIND_TO_INSTANCE;
  }

 private:
  TestDeleteMarker* const delete_marker_;
};

// These three classes serve only to test their TypeIds.
class TestTypeIdClass_1 {};
GUICPP_INJECTABLE(TestTypeIdClass_1);

class TestTypeIdClass_2 {};
GUICPP_INJECTABLE(TestTypeIdClass_2);

class TestTypeIdClass_3 {};
GUICPP_INJECTABLE(TestTypeIdClass_3);

// Tests for BindTable.
TEST(BindTableTest, FindEntry_ReturnsEntryForTypeIdAndNullIfNoEntryFound) {
  TestDeleteMarker delete_marker;

  // We don't check order of deletion in this test.
  EXPECT_CALL(delete_marker, Call(_)).Times(2);

  BindTable* bind_table = new BindTable();

  // bind_table takes ownership of entry.
  const TableEntryBase* entry1 = new DeleteCheckerEntry(&delete_marker);
  bind_table->AddEntry(entry1->GetTypeId(), entry1);

  const TableEntryBase* entry2 = new DeleteCheckerEntry(&delete_marker);
  TypeId id1 = TypeIdProvider<TestTypeIdClass_1>::GetTypeId();
  bind_table->AddEntry(id1, entry2);

  EXPECT_EQ(entry1, bind_table->FindEntry(entry1->GetTypeId()));
  EXPECT_EQ(entry2, bind_table->FindEntry(id1));

  // If there are no entry corresponding to some type_id, it returns NULL.
  TypeId id2 = TypeIdProvider<TestTypeIdClass_2>::GetTypeId();
  EXPECT_EQ(NULL, bind_table->FindEntry(id2));

  delete bind_table;
}

TEST(BindTableTest, EntriesAreDeletedInReverseOrderOfAddition) {
  TestDeleteMarker delete_marker;
  EXPECT_CALL(delete_marker, Call(_)).Times(0);
  const TableEntryBase* entry1 = new DeleteCheckerEntry(&delete_marker);
  const TableEntryBase* entry2 = new DeleteCheckerEntry(&delete_marker);
  const TableEntryBase* entry3 = new DeleteCheckerEntry(&delete_marker);

  MockFunction<void(string description)>  checkpoint;
  {
    InSequence sequence;

    // This checkpoint is invoked after entries are added to cleanup list
    // and just before deleting the bind_table.
    EXPECT_CALL(checkpoint, Call("begin cleanup"));

    // All bind entries are deleted when bind_table is deleted in reverse order.
    EXPECT_CALL(delete_marker, Call(entry3));
    EXPECT_CALL(delete_marker, Call(entry2));
    EXPECT_CALL(delete_marker, Call(entry1));
  }

  BindTable* bind_table = new BindTable();

  // Adding entry1.
  TypeId tid1 = TypeIdProvider<TestTypeIdClass_1>::GetTypeId();
  bind_table->AddEntry(tid1, entry1);

  // Adding entry2.
  TypeId tid2 = TypeIdProvider<TestTypeIdClass_2>::GetTypeId();
  bind_table->AddEntry(tid2, entry2);

  // Adding entry3.
  TypeId tid3 = TypeIdProvider<TestTypeIdClass_3>::GetTypeId();
  bind_table->AddEntry(tid3, entry3);

  checkpoint.Call("begin cleanup");
  delete bind_table;
}

// Tests for Bind overriding in BindTable.
TEST(BindTableTest, AddEntry_FailsForDuplicateEntry) {
  TestDeleteMarker delete_marker;
  EXPECT_CALL(delete_marker, Call(_)).Times(0);
  const TableEntryBase* entry1 = new DeleteCheckerEntry(&delete_marker);
  const TableEntryBase* entry2 = new DeleteCheckerEntry(&delete_marker);

  MockFunction<void(string description)>  checkpoint;
  {
    InSequence sequence;

    // This checkpoint is invoked after entries are added to cleanup list
    // and just before deleting the bind_table.
    EXPECT_CALL(checkpoint, Call("begin cleanup"));

    // All bind entries are deleted when bind_table is deleted in reverse order.
    EXPECT_CALL(delete_marker, Call(entry2));
    EXPECT_CALL(delete_marker, Call(entry1));
  }

  BindTable* bind_table = new BindTable();

  TypeId tid = TypeIdProvider<TestTypeIdClass_1>::GetTypeId();
  bind_table->AddEntry(tid, entry1);
  EXPECT_EQ(entry1, bind_table->FindEntry(tid));

  // Attempt to override with override flag set to false will fail.
  EXPECT_FALSE(bind_table->AddEntry(tid, entry2));
  EXPECT_EQ(entry1, bind_table->FindEntry(tid));  // Still bound to entry1

  checkpoint.Call("begin cleanup");
  delete bind_table;
}

TEST(BindTableTest, TableEntryReader_GetReadsValueType) {
  TestSimpleInjectableClass test_value(10);
  TestValueEntry<TestSimpleInjectableClass> test_entry(test_value);

  TestSimpleInjectableClass entry_value =
      TableEntryReader<TestSimpleInjectableClass>::Get(&test_entry, NULL, NULL);

  EXPECT_EQ(test_value.value(), entry_value.value());
}

TEST(BindTableTest, TableEntryReader_GetReadsPointerType) {
  TestSimpleInjectableClass test_value(10);
  TestPointerEntry<TestSimpleInjectableClass> test_entry(&test_value);

  TestSimpleInjectableClass* entry_pointer =
      TableEntryReader<TestSimpleInjectableClass*>::Get(
          &test_entry, NULL, NULL);

  EXPECT_EQ(&test_value, entry_pointer);
}

TEST(BindTableTest, TableEntryReader_GetReadsPointerAsConstPointerType) {
  TestSimpleInjectableClass test_value(10);
  TestPointerEntry<TestSimpleInjectableClass> test_entry(&test_value);

  const TestSimpleInjectableClass* entry_pointer =
      TableEntryReader<const TestSimpleInjectableClass*>::Get(
          &test_entry, NULL, NULL);

  EXPECT_EQ(&test_value, entry_pointer);
}

TEST(BindTableTest, TableEntryReader_GetReadsConstPointerType) {
  const TestSimpleInjectableClass test_value(10);
  TestPointerEntry<const TestSimpleInjectableClass> test_entry(&test_value);

  const TestSimpleInjectableClass* entry_pointer =
      TableEntryReader<const TestSimpleInjectableClass*>::Get(
          &test_entry, NULL, NULL);

  EXPECT_EQ(&test_value, entry_pointer);
}

}  // namespace internal
}  // namespace guicpp

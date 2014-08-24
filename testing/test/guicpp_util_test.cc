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


#include "guicpp/internal/guicpp_util.h"

#include <string>

#include "include/gtest/gtest.h"
#include "guicpp/internal/guicpp_port.h"
#include "guicpp/guicpp_annotations.h"
#include "guicpp/guicpp_at.h"
#include "guicpp/internal/guicpp_types.h"
#include "include/guicpp_test_helper.h"

namespace guicpp {
namespace internal {
using guicpp_test::TestLabelOne;
using guicpp_test::TestLabelTwo;

// TODO(bnmouli): Add test for TypeIdProvider

TEST(TypeInfoTest, IdentifiesCategoryCorrectly) {
  EXPECT_EQ(TypeInfo<int>::Category::value, TypesCategory::IS_VALUE);
  EXPECT_EQ(TypeInfo<int*>::Category::value, TypesCategory::IS_POINTER);
  EXPECT_EQ(TypeInfo<int&>::Category::value, TypesCategory::IS_REFERENCE);
  EXPECT_EQ(TypeInfo<int* const>::Category::value, TypesCategory::IS_POINTER);

  EXPECT_EQ(TypeInfo<const int>::Category::value, TypesCategory::IS_VALUE);
  EXPECT_EQ(TypeInfo<const int*>::Category::value, TypesCategory::IS_POINTER);
  EXPECT_EQ(TypeInfo<const int&>::Category::value,
            TypesCategory::IS_REFERENCE);
  EXPECT_EQ(TypeInfo<const int* const>::Category::value,
            TypesCategory::IS_POINTER);
}

TEST(TypeInfoTest, IdentifiesSpecifierCorrectly) {
  EXPECT_TRUE((guicpp::internal::is_same<
               TypeInfo<int>::TypeSpecifier, int>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               TypeInfo<int*>::TypeSpecifier, int>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               TypeInfo<int&>::TypeSpecifier, int>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               TypeInfo<int* const>::TypeSpecifier, int>::value));

  EXPECT_TRUE((guicpp::internal::is_same<
               TypeInfo<const int>::TypeSpecifier, int>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               TypeInfo<const int*>::TypeSpecifier, int>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               TypeInfo<const int&>::TypeSpecifier, int>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               TypeInfo<const int* const>::TypeSpecifier, int>::value));
}

TEST(TypeInfoTest, IdentifiesConstnessCorrectly) {
  EXPECT_FALSE(TypeInfo<int>::IsConst::value);
  EXPECT_FALSE(TypeInfo<int*>::IsConst::value);
  EXPECT_FALSE(TypeInfo<int&>::IsConst::value);
  EXPECT_FALSE(TypeInfo<int* const>::IsConst::value);

  EXPECT_TRUE(TypeInfo<const int>::IsConst::value);
  EXPECT_TRUE(TypeInfo<const int*>::IsConst::value);
  EXPECT_TRUE(TypeInfo<const int&>::IsConst::value);
  EXPECT_TRUE(TypeInfo<const int* const>::IsConst::value);
}

// Annotation base classes for testing.
class TestAnnotation1;

class TestAnnotationBaseClass1 {
 public:
  typedef TestAnnotationBaseClass1 AnnotationType;
  typedef TestAnnotation1 AnnotationDefaultValue;
};

class TestAnnotation3;

class TestAnnotationBaseClass2 {
 public:
  typedef TestAnnotationBaseClass2 AnnotationType;
  typedef TestAnnotation3 AnnotationDefaultValue;
};

class TestAnnotation5;

class TestAnnotationBaseClass3 {
 public:
  typedef TestAnnotationBaseClass3 AnnotationType;
  typedef TestAnnotation5 AnnotationDefaultValue;
};

// Annotations of TestAnnotationBaseClass1.
class TestAnnotation1: public TestAnnotationBaseClass1 {};
class TestAnnotation2: public TestAnnotationBaseClass1 {};

// Annotations of TestAnnotationBaseClass2.
class TestAnnotation3: public TestAnnotationBaseClass2 {};
class TestAnnotation4: public TestAnnotationBaseClass2 {};

// Annotations of TestAnnotationBaseClass3.
class TestAnnotation5: public TestAnnotationBaseClass3 {};


// AtUtil tests
TEST(AtUtilTest, GetTypes_IdentifiesActualTypeAndAnnotationsCorrectly) {
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<int>::ActualType, int>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<int*>::ActualType, int*>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<int&>::ActualType, int&>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<int* const>::ActualType, int*>::value));

  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<const int>::ActualType, int>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<const int*>::ActualType, const int*>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<const int&>::ActualType, const int&>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<const int* const>::ActualType,
               const int*>::value));

  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<int>::ArgType, int>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<int*>::ArgType, int*>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<int&>::ArgType, int&>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<int* const>::ArgType, int* const>::value));

  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<const int>::ArgType, const int>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<const int*>::ArgType, const int*>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<const int&>::ArgType, const int&>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<const int* const>::ArgType,
               const int* const>::value));

  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<int>::Annotations, EmptyAnnotations>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<int*>::Annotations, EmptyAnnotations>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<int&>::Annotations, EmptyAnnotations>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<int* const>::Annotations,
               EmptyAnnotations>::value));

  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<const int>::Annotations,
               EmptyAnnotations>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<const int*>::Annotations,
               EmptyAnnotations>::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<const int&>::Annotations,
               EmptyAnnotations >::value));
  EXPECT_TRUE((guicpp::internal::is_same<
               AtUtil::GetTypes<const int* const>::Annotations,
               EmptyAnnotations>::value));
}

TEST(AtUtilTest, GetAt_GetsAnnotationType) {
  typedef Annotations<TestAnnotation1, TestAnnotation4> TestAnnotations;

  // Specified type is not the same as default type.
  EXPECT_TRUE((guicpp::internal::is_same<
               TestAnnotation4,
               AtUtil::GetAt<TestAnnotations, TestAnnotationBaseClass2>::Type
               >::value));

  // Specified type is the same as default type.
  EXPECT_TRUE((guicpp::internal::is_same<
               TestAnnotation1,
               AtUtil::GetAt<TestAnnotations, TestAnnotationBaseClass1>::Type
               >::value));

  // If annotation of given annotation-base-class is not present,
  // it evaluates to default type.
  EXPECT_TRUE((guicpp::internal::is_same<
               TestAnnotation5,
               AtUtil::GetAt<TestAnnotations, TestAnnotationBaseClass3>::Type
               >::value));
}

// TODO(bnmouli): Move to guicpp_inject_util.cc
TEST(GuicppInjectorUtilTest,
     InjectorUtil_GivesUniqueIdForDifferentAnnotationAndType) {
  // EmptyAnnotations is used to indicate that the type is not annotated.
  TypeId int_id =
      InjectorUtil::GetBindId<EmptyAnnotations, int>();
  TypeId label1_int_id =
      InjectorUtil::GetBindId<Annotations<TestLabelOne>, int>();
  TypeId label2_int_id =
      InjectorUtil::GetBindId<Annotations<TestLabelTwo>, int>();

  // InjectorUtil::GetBindId typeids for each label/type pair.
  EXPECT_NE(label1_int_id, label2_int_id);
  EXPECT_NE(int_id, label1_int_id);
  EXPECT_NE(int_id, label2_int_id);

  // GetLabelTypeId() returns different values for different argument
  // types even if these types use the same label.
  TypeId label1_char_id =
      InjectorUtil::GetBindId<Annotations<TestLabelOne>, char>();
  EXPECT_NE(label1_int_id, label1_char_id);

  // Types annotated with Assisted have different type id.
  TypeId assisted_int_id =
      InjectorUtil::GetFactoryArgsBindId<At<Assisted, int> >();

  TypeId assisted_label1_int_id =
      InjectorUtil::GetFactoryArgsBindId<At<Assisted, TestLabelOne, int> >();

  EXPECT_NE(int_id, assisted_int_id);
  EXPECT_NE(assisted_label1_int_id, label1_int_id);

  // Order of annotation does not matter.
  EXPECT_EQ(assisted_label1_int_id, (
      InjectorUtil::GetFactoryArgsBindId<At<TestLabelOne, Assisted, int> >()));

  // GetFactoryArgsBindId assumes default value of InjectType to be Assisted
  EXPECT_EQ(assisted_label1_int_id, (
      InjectorUtil::GetFactoryArgsBindId<At<TestLabelOne, int> >()));
}

}  // namespace internal
}  // namespace guicpp

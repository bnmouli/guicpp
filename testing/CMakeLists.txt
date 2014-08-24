# Copyright 2014 Google Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


cmake_minimum_required (VERSION 2.6)
project (guicpp_test CXX C)
enable_testing()

# guicpp/include/guicpp/internal/guicpp_port.h is not ported for
# multi thread. Feel free to remove this if mutex implemented.
add_definitions( -DGTEST_HAS_PTHREAD=0 )

set(guicpproot ../..)
set(guicpp ${guicpproot}/guicpp)
set(gmock ${guicpproot}/third_party/gmock-1.7.0/)

add_subdirectory("${guicpp}" "${guicpp_test_BINARY_DIR}/bin/guicpp")
add_subdirectory("${gmock}" "${guicpp_test_BINARY_DIR}/bin/gmock")

include_directories("${guicpp_test_SOURCE_DIR}"
                    "${guicpp_SOURCE_DIR}/include"
                    "${gmock_SOURCE_DIR}"
                    "${gmock_SOURCE_DIR}/include"
                    "${gtest_SOURCE_DIR}"
                    "${gtest_SOURCE_DIR}/include")

cxx_library(guicpp_main
            "${cxx_strict}"
            src/guicpp_main.cc
            src/guicpp_test_helper.cc
            src/guicpp_test_modules.cc
            "${gtest_SOURCE_DIR}/src/gtest-all.cc"
            "${gmock_SOURCE_DIR}/src/gmock-all.cc")

target_link_libraries(guicpp_main guicpp)

cxx_test(guicpp_binder_test guicpp_main)
cxx_test(guicpp_builder_death_test guicpp_main)
cxx_test(guicpp_builder_test guicpp_main)
cxx_test(guicpp_entries_test guicpp_main)
cxx_test(guicpp_factory_test guicpp_main)
cxx_test(guicpp_inject_util_test guicpp_main)
cxx_test(guicpp_injector_death_test guicpp_main)
cxx_test(guicpp_injector_test guicpp_main)
cxx_test(guicpp_local_context_test guicpp_main)
cxx_test(guicpp_macros_test guicpp_main)
cxx_test(guicpp_provider_test guicpp_main)
cxx_test(guicpp_singleton_test guicpp_main)
cxx_test(guicpp_strings_test guicpp_main)
cxx_test(guicpp_table_death_test guicpp_main)
cxx_test(guicpp_table_test guicpp_main)
cxx_test(guicpp_util_test guicpp_main)

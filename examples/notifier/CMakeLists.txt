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


set(guicpp_dir ../../)

cmake_minimum_required (VERSION 2.6)
project (notifier CXX C)

add_subdirectory("${guicpp_dir}" "${notifier_BINARY_DIR}/guicpp")
include_directories("${notifier_SOURCE_DIR}" "${guicpp_SOURCE_DIR}/include")

add_executable(nfe
               http_server/implementation/server.cc
               contact_list/contact_list.cc
               notifier_frontend/nfe_module.cc
               notifier_frontend/nfe_server.cc
               notifier_frontend/nfe_guicpp.cc
               notifier_frontend/nfe_main.cc
               notifier_frontend/nfe_request_handler.cc
               messengers/message_sender.cc
               messengers/notifiers.cc)

target_link_libraries(nfe guicpp)

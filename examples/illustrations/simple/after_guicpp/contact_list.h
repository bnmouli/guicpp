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


// Defines ContactInfo and ContactList.

#ifndef CONTACT_LIST_H_
#define CONTACT_LIST_H_

#include <map>
#include <string>
#include <iostream>

#include "guicpp/guicpp.h"

// ContactInfo information
struct ContactInfo {
  std::string nickname;
  std::string complete_name;
  std::string email_id;
  std::string mobile_number;
};

// Guic++ is not expected to Instantiate this without explicit binding
GUICPP_INJECTABLE(ContactInfo);

// In-memory list of contacts.
class ContactList {
 public:
  ContactList();
  ~ContactList();

  // Loads contacts from csv "contacts_csv" file.
  // Each line in the csv file must be of the format:
  //   <nickname>,<complete_name>,<email>,<mobile_number>
  //
  // Example:
  //   tom,Tom The Cat,tomcat@gmail.com,+919876543210
  void Load(const std::string& contacts_csv);

  // Returns contact information associated with nickname.
  const ContactInfo* GetContact(const std::string& nickname) const;

 private:
  void ParseCSV(std::istream* csvfile);

  std::map<std::string, ContactInfo> contact_map_;

  // Top prohibit copy and assign
  explicit ContactList(const ContactList& from);
  void operator=(const ContactList& from);
};

GUICPP_INJECT_CTOR(ContactList, ());

#endif  // CONTACT_LIST_H_

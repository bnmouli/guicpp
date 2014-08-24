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
#include <iostream>

#include "common/notifier_port.h"
#include "guicpp/guicpp.h"

namespace notifier_example {

// ContactInfo information
struct ContactInfo {
  string nickname;
  string complete_name;
  string email_id;
  string mobile_number;
};

// Guic++ is not expected to instantiate this type without explicit binding.
GUICPP_INJECTABLE(ContactInfo);

// In-memory list of contacts.
class ContactList {
 public:
  ~ContactList();

  const ContactInfo* GetContact(const string& nickname) const;

  // Loads contacts from csv "contacts_csv" file.
  // Returns an instance of ContactList containg the contact list.
  // Each line in the csv file must be of the format:
  //   <nickname>,<complete_name>,<email>,<mobile_number>
  //
  // Example:
  //   tom,Tom The Cat,tomcat@gmail.com,+919876543210
  static ContactList* Load(const string& contacts_csv);

 private:
  void ParseCSV(std::istream* csvfile);
  ContactList();  // Use Load to get instance.

  std::map<string, ContactInfo> contact_map_;

  SIMPLE_DISALLOW_COPY_AND_ASSIGN(ContactList);
};

GUICPP_INJECTABLE(ContactList);

}  // namespace notifier_example

#endif  // CONTACT_LIST_H_

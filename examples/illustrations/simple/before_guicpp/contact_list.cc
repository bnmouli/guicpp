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


// Defines methods of ContactInfo and ContactList.

#include "contact_list.h"

#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>

static const int kMaxLineLength = 10000;

ContactList::ContactList() {}
ContactList::~ContactList() {}

const ContactInfo* ContactList::GetContact(const std::string& nickname) const {
  std::map<std::string, ContactInfo>::const_iterator iter =
      contact_map_.find(nickname);
  if (iter != contact_map_.end()) {
    return &iter->second;
  }

  return NULL;
}

void ContactList::Load(const std::string& contacts_csv) {
  std::ifstream csvfile;
  csvfile.open(contacts_csv.c_str());
  ParseCSV(&csvfile);
  csvfile.close();
}

static const char *readValue(const char* start, std::string* value) {
  if (!start) return NULL;
  const char *end = strpbrk(start, ",\n");
  if (end) {
    value->assign(start, end - start);
    return end + 1;
  }

  value->assign(start);
  return NULL;
}

// CSV file parser.
void ContactList::ParseCSV(std::istream* csvfile) {
  char line[kMaxLineLength];
  while (csvfile->good()) {
    csvfile->getline(line, kMaxLineLength);
    const char* start = line;
    const char* end = strpbrk(start, ",\n");

    ContactInfo contact;
    start = readValue(start, &contact.nickname);
    start = readValue(start, &contact.complete_name);
    start = readValue(start, &contact.email_id);
    start = readValue(start, &contact.mobile_number);

    contact_map_[contact.nickname] = contact;
  }
}

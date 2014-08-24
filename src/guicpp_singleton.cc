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


#include <algorithm>
#include "guicpp/guicpp_singleton.h"

namespace guicpp {
namespace internal {
using std::find;

void ScopeSetupContext::AddToInitList(SetupInterface* init) {
  // It is an error to add same object more than once to the list.
  GUICPP_DCHECK_(find(init_list_.begin(), init_list_.end(), init)
                 == init_list_.end());
  init_list_.push_back(init);
}

void ScopeSetupContext::AddToCleanupList(SetupInterface* cleanup) {
  // It is a bug to add a provider to the cleanup list unless it's already in
  // the init list. We derive the max size on cleanup list based on size of
  // init list.
  GUICPP_DCHECK_(find(init_list_.begin(), init_list_.end(), cleanup)
                 != init_list_.end());

  WriterMutexLock mu(&mu_);
  cleanup_list_[cleanup_list_size_++] = cleanup;
}

void ScopeSetupContext::Init(const Injector* injector) {
  injector_ = injector;

  cleanup_list_.reset(new SetupInterface*[init_list_.size()]);

  for (vector<SetupInterface*>::iterator iter = init_list_.begin();
       iter != init_list_.end();
       ++iter) {
    (*iter)->Init(injector);
  }
}

void ScopeSetupContext::Cleanup() {
  ReaderMutexLock mu(&mu_);  // This read lock may be unnecessary.

  for (int i = cleanup_list_size_ -1; i >= 0; --i) {
    cleanup_list_[i]->Cleanup();
  }
}

}  // namespace internal
}  // namespace guicpp

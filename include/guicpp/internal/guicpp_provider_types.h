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


// This file contains classes used to define providers.

#ifndef GUICPP_PROVIDER_TYPES_H_
#define GUICPP_PROVIDER_TYPES_H_

#include "guicpp/internal/guicpp_port.h"

namespace guicpp {
template <typename T> class AbstractProvider;

namespace internal {
// A base class used only to identify whether a given type is a provider
// by using guicpp::internal::is_convertible.
class ProviderBase: public InternalType {};

// This class is the base class for Provider classes. It is always invoked by
// passing a function prototype as template argument. There are specialized
// versions of this class for function prototypes (in provider_helpers.h).
// In effect, the following non-specialized version is never used.
//
// The ProviderGet class that is specialized for a function prototype T,
// contains an abstract method "Get()" with the same prototype as T.
template <typename T>
class ProviderGet: public ProviderBase {
 public:
  virtual ~ProviderGet() {}

 private:
  // Instantiation is disabled to ensure that this non-specialized template
  // class is never used.
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(ProviderGet);
};

}  // namespace internal
}  // namespace guicpp

#endif  // GUICPP_PROVIDER_TYPES_H_

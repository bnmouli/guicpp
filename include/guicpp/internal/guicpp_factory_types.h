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


// Types used by factory. The types defined here have specialized
// definitions in factory_helpers.h

#ifndef GUICPP_FACTORY_TYPES_H_
#define GUICPP_FACTORY_TYPES_H_

#include <string>
#include "guicpp/internal/guicpp_types.h"
#include "guicpp/internal/guicpp_port.h"

namespace guicpp {
namespace internal {
// A base class used only to identify if the given type is
// a factory or not using guicpp::internal::is_convertible
class FactoryBase: public InternalType {};

// This class is the base class of Factory interface classes. It is always
// invoked by passing a function prototype as template argument. There are
// specialized versions of this class for function prototypes
// (in factory_helpers.h). In effect, the following non-specialized version
// is never used.
//
// The FactoryInterface class that is specialized for a function prototype T,
// contains an abstract method "Get()" with the same prototype as T.
template <typename T>
class FactoryInterface: public FactoryBase {
 public:
  virtual ~FactoryInterface() {}

 private:
  // Instantiation is disabled to ensure that this non-specialized template
  // class is never used.
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(FactoryInterface);
};

// The real implementation of factory. Header factory_helpers.h has specialized
// definitions of this class which has necessary implementation.
template <typename Annotations, typename FactoryType, typename GetSignature>
class RealFactory {
 private:
  GUICPP_DISALLOW_IMPLICIT_CONSTRUCTORS_(RealFactory);
};

// Logging related.

class Logger {
 public:
  virtual ~Logger() { }

  virtual void Log(const std::string& message) const = 0;

 protected:
  Logger() {}
};

// Class that logs the message.
// This should also take care of any formating that needs to be done.
class LogMessage {
 public:
  virtual ~LogMessage() {}

  virtual void Log(Logger* logger) const = 0;

 protected:
  LogMessage(LocalContext* context);

 private:
  const LogMessage* next;
};

}  // namespace internal
}  // namespace guicpp

#endif  // GUICPP_FACTORY_TYPES_H_

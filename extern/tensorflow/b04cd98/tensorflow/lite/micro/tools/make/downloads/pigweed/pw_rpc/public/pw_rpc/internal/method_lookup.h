// Copyright 2020 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.
#pragma once

#include "pw_rpc/internal/method.h"

namespace pw::rpc::internal {

// Gets a Method object from a generated RPC service class. Getter functions are
// provided for each supported method implementation. The
//
// To ensure the MethodUnion actually holds the requested method type, the
// method ID is accessed in a static_assert. It is invalid to access an unset
// union member in a constant expression, so this results in a compiler error.
class MethodLookup {
 public:
  template <typename Service, uint32_t method_id>
  static constexpr const auto& GetRawMethod() {
    const auto& method = GetMethodUnion<Service, method_id>().raw_method();
    static_assert(method.id() == method_id, "Incorrect method implementation");
    return method;
  }

  template <typename Service, uint32_t method_id>
  static constexpr const auto& GetNanopbMethod() {
    const auto& method = GetMethodUnion<Service, method_id>().nanopb_method();
    static_assert(method.id() == method_id, "Incorrect method implementation");
    return method;
  }

 private:
  template <typename Service, uint32_t method_id>
  static constexpr const auto& GetMethodUnion() {
    constexpr auto method = GetMethodUnionPointer<Service>(method_id);
    static_assert(method != nullptr,
                  "The selected function is not an RPC service method");
    return *method;
  }

  template <typename Service>
  static constexpr
      typename decltype(GeneratedService<Service>::kMethods)::const_pointer
      GetMethodUnionPointer(uint32_t method_id) {
    for (size_t i = 0; i < GeneratedService<Service>::kMethodIds.size(); ++i) {
      if (GeneratedService<Service>::kMethodIds[i] == method_id) {
        return &GeneratedService<Service>::kMethods[i];
      }
    }
    return nullptr;
  }
};

}  // namespace pw::rpc::internal

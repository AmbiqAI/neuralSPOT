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

#include <type_traits>
#include <utility>

#include "pw_rpc/internal/method.h"
#include "pw_rpc/internal/method_type.h"

namespace pw::rpc::internal {

// Base class for different combinations of possible service methods. Derived
// classes should contain a union of different method types, one of which is a
// base Method.
class MethodUnion {
 public:
  constexpr const Method& method() const;
};

class CoreMethodUnion : public MethodUnion {
 public:
  constexpr const Method& method() const { return impl_.method; }

 private:
  // All derived MethodUnions must contain a union of different method
  // implementations as their only member.
  union {
    Method method;
  } impl_;
};

constexpr const Method& MethodUnion::method() const {
  // This is an ugly hack. As all MethodUnion classes contain a union of Method
  // derivatives, CoreMethodUnion is used to extract a generic Method from the
  // specific implementation.
  return static_cast<const CoreMethodUnion*>(this)->method();
}

// Templated false value for use in static_assert(false) statements.
template <typename...>
constexpr std::false_type kCheckMethodSignature{};

// In static_assert messages, use newlines in GCC since it displays them
// correctly. Clang displays \n, which is not helpful.
#ifdef __clang__
#define _PW_RPC_FORMAT_ERROR_MESSAGE(msg, signature) msg " " signature
#else
#define _PW_RPC_FORMAT_ERROR_MESSAGE(msg, signature) \
  "\n" msg "\n\n    " signature "\n"
#endif  // __clang__

#define _PW_RPC_FUNCTION_ERROR(type, return_type, args)                   \
  _PW_RPC_FORMAT_ERROR_MESSAGE(                                           \
      "This RPC is a " type                                               \
      " RPC, but its function signature is not correct. The function "    \
      "signature is determined by the protobuf library in use, but " type \
      " RPC implementations generally take the form:",                    \
      return_type " MethodName(ServerContext&, " args ")")

// This function is called if an RPC method implementation's signature is not
// correct. It triggers a static_assert with an error message tailored to the
// expected RPC type.
template <auto method,
          MethodType expected,
          typename InvalidImpl = MethodImplementation<method>>
constexpr auto InvalidMethod(uint32_t) {
  if constexpr (expected == MethodType::kUnary) {
    static_assert(
        kCheckMethodSignature<decltype(method)>,
        _PW_RPC_FUNCTION_ERROR("unary", "Status", "Request, Response"));
  } else if constexpr (expected == MethodType::kServerStreaming) {
    static_assert(
        kCheckMethodSignature<decltype(method)>,
        _PW_RPC_FUNCTION_ERROR(
            "server streaming", "void", "Request, ServerWriter<Response>&"));
  } else if constexpr (expected == MethodType::kClientStreaming) {
    static_assert(
        kCheckMethodSignature<decltype(method)>,
        _PW_RPC_FUNCTION_ERROR(
            "client streaming", "Status", "ServerReader<Request>&, Response"));
  } else if constexpr (expected == MethodType::kBidirectionalStreaming) {
    static_assert(kCheckMethodSignature<decltype(method)>,
                  _PW_RPC_FUNCTION_ERROR(
                      "bidirectional streaming",
                      "void",
                      "ServerReader<Request>&, ServerWriter<Response>&"));
  } else {
    static_assert(kCheckMethodSignature<decltype(method)>,
                  "Unsupported MethodType");
  }
  return InvalidImpl::Invalid();
}

#undef _PW_RPC_FORMAT_ERROR_MESSAGE
#undef _PW_RPC_FUNCTION_ERROR

// This function checks the type of the method and calls the appropriate
// function to create the method instance.
template <auto method, typename MethodImpl, MethodType type, typename... Args>
constexpr auto GetMethodFor(uint32_t id, Args&&... args) {
  if constexpr (MethodTraits<decltype(method)>::kType != type) {
    return InvalidMethod<method, type>(id);
  } else if constexpr (type == MethodType::kUnary) {
    return MethodImpl::template Unary<method>(id, std::forward<Args>(args)...);
  } else if constexpr (type == MethodType::kServerStreaming) {
    return MethodImpl::template ServerStreaming<method>(
        id, std::forward<Args>(args)...);
  } else if constexpr (type == MethodType::kClientStreaming) {
    return MethodImpl::template ClientStreaming<method>(
        id, std::forward<Args>(args)...);
  } else if constexpr (type == MethodType::kBidirectionalStreaming) {
    return MethodImpl::template BidirectionalStreaming<method>(
        id, std::forward<Args>(args)...);
  } else {
    static_assert(kCheckMethodSignature<MethodImpl>, "Invalid MethodType");
  }
}

}  // namespace pw::rpc::internal

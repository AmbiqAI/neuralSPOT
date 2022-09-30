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

#include "pw_polyfill/standard_library/namespace.h"

_PW_POLYFILL_BEGIN_NAMESPACE_STD

template <typename T>
constexpr remove_reference_t<T>&& move(T&& object) {
  return (remove_reference_t<T> &&) object;
}

// Forward declare these classes, which are specialized in other headers.
template <decltype(sizeof(0)), typename>
struct tuple_element;

template <typename>
struct tuple_size;

_PW_POLYFILL_END_NAMESPACE_STD

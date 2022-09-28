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

#include <cstddef>

#include "pw_polyfill/standard_library/namespace.h"

_PW_POLYFILL_BEGIN_NAMESPACE_STD

#define __cpp_lib_nonmember_container_access 201411L

template <typename C>
auto begin(C& container) -> decltype(container.begin()) {
  return container.begin();
}

template <typename C>
auto begin(const C& container) -> decltype(container.begin()) {
  return container.begin();
}

template <typename C>
constexpr auto data(C& container) -> decltype(container.data()) {
  return container.data();
}

template <typename C>
constexpr auto data(const C& container) -> decltype(container.data()) {
  return container.data();
}

template <typename T, decltype(sizeof(int)) kSize>
constexpr T* data(T (&array)[kSize]) noexcept {
  return array;
}

template <typename C>
auto end(C& container) -> decltype(container.end()) {
  return container.end();
}

template <typename C>
auto end(const C& container) -> decltype(container.end()) {
  return container.end();
}

template <typename C>
constexpr auto size(const C& container) -> decltype(container.size()) {
  return container.size();
}

template <typename T, decltype(sizeof(int)) kSize>
constexpr decltype(sizeof(int)) size(const T (&)[kSize]) noexcept {
  return kSize;
}

// NOT IMPLEMENTED: iterator_traits does not work
template <typename>
struct iterator_traits {};

// NOT IMPLEMENTED: Reverse iterators are not implemented.
template <typename>
struct reverse_iterator;

_PW_POLYFILL_END_NAMESPACE_STD

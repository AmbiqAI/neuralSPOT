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

template <class InputIterator, class OutputIterator>
constexpr OutputIterator copy(InputIterator first,
                              InputIterator last,
                              OutputIterator dest) {
  while (first != last) {
    *dest++ = *first++;
  }
  return dest;
}

template <typename T>
constexpr const T& min(const T& lhs, const T& rhs) {
  return (rhs < lhs) ? rhs : lhs;
}

template <typename T>
constexpr const T& max(const T& lhs, const T& rhs) {
  return (lhs < rhs) ? rhs : lhs;
}

template <class InputIterator, typename T>
constexpr InputIterator find(InputIterator first,
                             InputIterator last,
                             const T& value) {
  for (; first != last; ++first) {
    if (*first == value) {
      return first;
    }
  }
  return last;
}

template <typename T>
constexpr T&& forward(remove_reference_t<T>& value) {
  return static_cast<T&&>(value);
}

template <typename T>
constexpr T&& forward(remove_reference_t<T>&& value) {
  return static_cast<T&&>(value);
}

template <class LhsIterator, class RhsIterator>
constexpr bool equal(LhsIterator first_l,
                     LhsIterator last_l,
                     RhsIterator first_r,
                     RhsIterator last_r) {
  while (first_l != last_l && first_r != last_r) {
    if (*first_l != *first_r) {
      return false;
    }
    ++first_l;
    ++first_r;
  }
  return first_l == last_l && first_r == last_r;
}

template <class LhsIterator, class RhsIterator>
constexpr bool lexicographical_compare(LhsIterator first_l,
                                       LhsIterator last_l,
                                       RhsIterator first_r,
                                       RhsIterator last_r) {
  while (first_l != last_l && first_r != last_r) {
    if (*first_l < *first_r) {
      return true;
    }
    if (*first_r < *first_l) {
      return false;
    }

    ++first_l;
    ++first_r;
  }
  return (first_l == last_l) && (first_r != last_r);
}

_PW_POLYFILL_END_NAMESPACE_STD

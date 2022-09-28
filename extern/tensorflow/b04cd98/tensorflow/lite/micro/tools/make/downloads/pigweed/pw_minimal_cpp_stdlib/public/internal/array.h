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

#include <iterator>

#include "pw_polyfill/standard_library/namespace.h"

_PW_POLYFILL_BEGIN_NAMESPACE_STD

template <typename T, decltype(sizeof(0)) kSize>
struct array {
  using value_type = T;
  using size_type = decltype(kSize);
  using difference_type =
      decltype(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = T*;
  using const_iterator = const T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  // NOT IMPLEMENTED: at() does not bounds checking.
  constexpr reference at(size_type index) { return data()[index]; }
  constexpr const_reference at(size_type index) const { return data()[index]; }

  constexpr reference operator[](size_type index) { return data()[index]; }
  constexpr const_reference operator[](size_type index) const {
    return data()[index];
  }

  constexpr reference front() { return data()[0]; }
  constexpr const_reference front() const { return data()[0]; }

  constexpr reference back() {
    static_assert(kSize > 0);
    return data()[size() - 1];
  }
  constexpr const_reference back() const {
    static_assert(kSize > 0);
    return data()[size() - 1];
  }

  constexpr pointer data() noexcept {
    static_assert(kSize > 0);
    return __data;
  }
  constexpr const_pointer data() const noexcept {
    static_assert(kSize > 0);
    return __data;
  }

  constexpr iterator begin() noexcept { return data(); }
  constexpr const_iterator begin() const noexcept { return data(); }
  constexpr const_iterator cbegin() const noexcept { return begin(); }

  constexpr iterator end() noexcept { return data() + kSize; }
  constexpr const_iterator end() const noexcept { return data() + kSize; }
  constexpr const_iterator cend() const noexcept { return end(); }

  // NOT IMPLEMENTED
  constexpr reverse_iterator rbegin() noexcept;
  constexpr const_reverse_iterator rbegin() const noexcept;
  constexpr const_reverse_iterator crbegin() const noexcept;

  // NOT IMPLEMENTED
  constexpr reverse_iterator rend() noexcept;
  constexpr const_reverse_iterator rend() const noexcept;
  constexpr const_reverse_iterator crend() const noexcept;

  [[nodiscard]] constexpr bool empty() const noexcept { return kSize == 0u; }

  constexpr size_type size() const noexcept { return kSize; }

  constexpr size_type max_size() const noexcept { return size(); }

  constexpr void fill(const T& value) {
    for (T& array_value : __data) {
      array_value = value;
    }
  }

  // NOT IMPLEMENTED
  constexpr void swap(array& other) noexcept;

  T __data[kSize];
};

// NOT IMPLEMENTED: comparison operators, get, swap, tuple specializations

_PW_POLYFILL_END_NAMESPACE_STD

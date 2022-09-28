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

#include "pw_polyfill/standard_library/namespace.h"

_PW_POLYFILL_BEGIN_NAMESPACE_STD

template <typename T>
class initializer_list {
 public:
  using value_type = T;
  using reference = const T&;
  using const_reference = const T&;
  using size_type = decltype(sizeof(0));
  using iterator = const T*;
  using const_iterator = const T*;

  constexpr initializer_list() : begin_(nullptr), size_(0) {}

  size_type size() const { return size_; }

  iterator begin() const { return begin_; }
  iterator end() const { return begin_ + size_; }

 private:
  // The order of these members must stay the same. The compiler makes
  // assumptions about this class, and reordering these breaks things.
  const T* begin_;
  size_type size_;
};

_PW_POLYFILL_END_NAMESPACE_STD

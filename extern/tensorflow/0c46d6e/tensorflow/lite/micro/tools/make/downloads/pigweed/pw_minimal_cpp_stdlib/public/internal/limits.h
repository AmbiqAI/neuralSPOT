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

#include <limits.h>

#include "pw_polyfill/standard_library/namespace.h"

_PW_POLYFILL_BEGIN_NAMESPACE_STD

template <typename T>
struct numeric_limits {
  static constexpr bool is_specialized = false;
};

// Only a few of the numeric_limits methods are implemented.
#define _PW_LIMITS_SPECIALIZATION(                               \
    type, val_signed, val_int, min_value, max_value)             \
  template <>                                                    \
  struct numeric_limits<type> {                                  \
    static constexpr bool is_specialized = true;                 \
                                                                 \
    static constexpr bool is_signed = (val_signed);              \
    static constexpr bool is_integer = (val_int);                \
                                                                 \
    static constexpr type min() noexcept { return (min_value); } \
    static constexpr type max() noexcept { return (max_value); } \
  }

#define _PW_INTEGRAL_LIMIT(type, sname, uname)            \
  _PW_LIMITS_SPECIALIZATION(                              \
      signed type, true, true, sname##_MIN, sname##_MAX); \
  _PW_LIMITS_SPECIALIZATION(unsigned type, false, true, 0u, uname##_MAX)

_PW_LIMITS_SPECIALIZATION(bool, false, true, false, true);
_PW_LIMITS_SPECIALIZATION(char, char(-1) < char(0), true, CHAR_MIN, CHAR_MAX);

_PW_INTEGRAL_LIMIT(char, SCHAR, UCHAR);
_PW_INTEGRAL_LIMIT(short, SHRT, USHRT);
_PW_INTEGRAL_LIMIT(int, INT, UINT);
_PW_INTEGRAL_LIMIT(long, LONG, ULONG);

#ifndef LLONG_MIN
#define LLONG_MIN ((long long)(~0ull ^ (~0ull >> 1)))
#define LLONG_MAX ((long long)(~0ull >> 1))

#define ULLONG_MIN (0ull)
#define ULLONG_MAX (~0ull)
#endif  // LLONG_MIN

_PW_INTEGRAL_LIMIT(long long, LLONG, ULLONG);

#undef _PW_LIMITS_SPECIALIZATION
#undef _PW_INTEGRAL_LIMIT

_PW_POLYFILL_END_NAMESPACE_STD

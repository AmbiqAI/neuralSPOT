/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef TENSORFLOW_LITE_KERNELS_INTERNAL_REFERENCE_NEG_H_
#define TENSORFLOW_LITE_KERNELS_INTERNAL_REFERENCE_NEG_H_

#include <cstdint>
#include <limits>
#include "tensorflow/lite/kernels/internal/types.h"

namespace tflite {

namespace reference_ops {

template <typename T>
inline void Negate(
  const RuntimeShape& input_shape, const T* input_data,
  const RuntimeShape& output_shape, T* output_data
) {
  const int flat_size = MatchingFlatSize(input_shape, output_shape);

  for (int i = 0; i < flat_size; ++i) {
    output_data[i] = -input_data[i];
  }
}

template <typename T>
T NegateSaturateQuantized(int32_t value) {
  if (value > std::numeric_limits<T>::max()) {
    return std::numeric_limits<T>::max();
  }
  if (value < std::numeric_limits<T>::min()) {
    return std::numeric_limits<T>::min();
  }
  return static_cast<T>(value);
}

template <typename T>
void NegateQuantized(
  const RuntimeShape& input_shape, const T* input_data,
  const RuntimeShape& output_shape, T* output_data,
  int32_t zero_point
) {

  const int flat_size = MatchingFlatSize(input_shape, output_shape);

  for (int i = 0; i < flat_size; ++i) {
    int32_t unclamped = 2 * zero_point - static_cast<int32_t>(input_data[i]);
    output_data[i] = NegateSaturateQuantized<T>(unclamped);
  }
}


}  // namespace reference_ops
}  // namespace tflite

#endif  // TENSORFLOW_LITE_KERNELS_INTERNAL_REFERENCE_NEG_H_

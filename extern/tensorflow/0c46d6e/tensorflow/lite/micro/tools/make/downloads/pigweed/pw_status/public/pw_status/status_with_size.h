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
#include <type_traits>

#include "pw_status/status.h"

namespace pw {

class StatusWithSize;

namespace internal {

// TODO(pwbug/268): Remove this class after migrating to the helper functions.
template <int kStatusShift>
class StatusWithSizeConstant {
 private:
  friend class ::pw::StatusWithSize;

  explicit constexpr StatusWithSizeConstant(Status value)
      : value_(static_cast<size_t>(value.code()) << kStatusShift) {}

  const size_t value_;
};

}  // namespace internal

// StatusWithSize stores a status and an unsigned integer. The integer must not
// exceed StatusWithSize::max_size(), which is 134,217,727 (2**27 - 1) on 32-bit
// systems.
//
// StatusWithSize is useful for reporting the number of bytes read or written in
// an operation along with the status. For example, a function that writes a
// formatted string may want to report both the number of characters written and
// whether it ran out of space.
//
// StatusWithSize is more efficient than its alternatives. It packs a status and
// size into a single word, which can be returned from a function in a register.
// Because they are packed together, the size is limited to max_size().
//
// StatusWithSize's alternatives result in larger code size. For example:
//
//   1. Return status, pass size output as a pointer argument.
//
//      Requires an additional argument and forces the output argument to the
//      stack in order to pass an address, increasing code size.
//
//   2. Return an object with Status and size members.
//
//      At least for ARMv7-M, the returned struct is created on the stack, which
//      increases code size.
//
class StatusWithSize {
 private:
  static constexpr size_t kStatusBits = 5;
  static constexpr size_t kSizeMask = ~static_cast<size_t>(0) >> kStatusBits;
  static constexpr size_t kStatusMask = ~kSizeMask;
  static constexpr size_t kStatusShift = sizeof(size_t) * 8 - kStatusBits;

  using Constant = internal::StatusWithSizeConstant<kStatusShift>;

 public:
  // Non-OK StatusWithSizes can be constructed from these constants, such as:
  //
  //   StatusWithSize result = StatusWithSize::NotFound();
  //
  // These constants are DEPRECATED! Use the helper functions below instead. For
  // example, change StatusWithSize::NotFound() to StatusWithSize::NotFound().
  //
  // TODO(pwbug/268): Migrate to the functions and remove these constants.
  static constexpr Constant CANCELLED{Status::Cancelled()};
  static constexpr Constant UNKNOWN{Status::Unknown()};
  static constexpr Constant INVALID_ARGUMENT{Status::InvalidArgument()};
  static constexpr Constant DEADLINE_EXCEEDED{Status::DeadlineExceeded()};
  static constexpr Constant NOT_FOUND{Status::NotFound()};
  static constexpr Constant ALREADY_EXISTS{Status::AlreadyExists()};
  static constexpr Constant PERMISSION_DENIED{Status::PermissionDenied()};
  static constexpr Constant RESOURCE_EXHAUSTED{Status::ResourceExhausted()};
  static constexpr Constant FAILED_PRECONDITION{Status::FailedPrecondition()};
  static constexpr Constant ABORTED{Status::Aborted()};
  static constexpr Constant OUT_OF_RANGE{Status::OutOfRange()};
  static constexpr Constant UNIMPLEMENTED{Status::Unimplemented()};
  static constexpr Constant INTERNAL{Status::Internal()};
  static constexpr Constant UNAVAILABLE{Status::Unavailable()};
  static constexpr Constant DATA_LOSS{Status::DataLoss()};
  static constexpr Constant UNAUTHENTICATED{Status::Unauthenticated()};

  // Functions that create a StatusWithSize with the specified status code. For
  // codes other than OK, the size defaults to 0.
  static constexpr StatusWithSize Ok(size_t size) {
    return StatusWithSize(size);
  }
  static constexpr StatusWithSize Cancelled(size_t size = 0) {
    return StatusWithSize(Status::Cancelled(), size);
  }
  static constexpr StatusWithSize Unknown(size_t size = 0) {
    return StatusWithSize(Status::Unknown(), size);
  }
  static constexpr StatusWithSize InvalidArgument(size_t size = 0) {
    return StatusWithSize(Status::InvalidArgument(), size);
  }
  static constexpr StatusWithSize DeadlineExceeded(size_t size = 0) {
    return StatusWithSize(Status::DeadlineExceeded(), size);
  }
  static constexpr StatusWithSize NotFound(size_t size = 0) {
    return StatusWithSize(Status::NotFound(), size);
  }
  static constexpr StatusWithSize AlreadyExists(size_t size = 0) {
    return StatusWithSize(Status::AlreadyExists(), size);
  }
  static constexpr StatusWithSize PermissionDenied(size_t size = 0) {
    return StatusWithSize(Status::PermissionDenied(), size);
  }
  static constexpr StatusWithSize Unauthenticated(size_t size = 0) {
    return StatusWithSize(Status::Unauthenticated(), size);
  }
  static constexpr StatusWithSize ResourceExhausted(size_t size = 0) {
    return StatusWithSize(Status::ResourceExhausted(), size);
  }
  static constexpr StatusWithSize FailedPrecondition(size_t size = 0) {
    return StatusWithSize(Status::FailedPrecondition(), size);
  }
  static constexpr StatusWithSize Aborted(size_t size = 0) {
    return StatusWithSize(Status::Aborted(), size);
  }
  static constexpr StatusWithSize OutOfRange(size_t size = 0) {
    return StatusWithSize(Status::OutOfRange(), size);
  }
  static constexpr StatusWithSize Unimplemented(size_t size = 0) {
    return StatusWithSize(Status::Unimplemented(), size);
  }
  static constexpr StatusWithSize Internal(size_t size = 0) {
    return StatusWithSize(Status::Internal(), size);
  }
  static constexpr StatusWithSize Unavailable(size_t size = 0) {
    return StatusWithSize(Status::Unavailable(), size);
  }
  static constexpr StatusWithSize DataLoss(size_t size = 0) {
    return StatusWithSize(Status::DataLoss(), size);
  }

  // Creates a StatusWithSize with Status::Ok() and a size of 0.
  explicit constexpr StatusWithSize() : size_(0) {}

  // Creates a StatusWithSize with Status::Ok() and the provided size.
  // std::enable_if is used to prevent enum types (e.g. Status) from being used.
  // TODO(hepler): Add debug-only assert that size <= max_size().
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  explicit constexpr StatusWithSize(T size) : size_(size) {}

  // Creates a StatusWithSize with the provided status and size.
  explicit constexpr StatusWithSize(Status status, size_t size)
      : StatusWithSize((static_cast<size_t>(status.code()) << kStatusShift) |
                       size) {}

  // Allow implicit conversions from the StatusWithSize constants.
  constexpr StatusWithSize(Constant constant) : size_(constant.value_) {}

  constexpr StatusWithSize(const StatusWithSize&) = default;
  constexpr StatusWithSize& operator=(const StatusWithSize&) = default;

  // Returns the size. The size is always present, even if status() is an error.
  constexpr size_t size() const { return size_ & kSizeMask; }

  // The maximum valid value for size.
  static constexpr size_t max_size() { return kSizeMask; }

  // True if status() == Status::Ok().
  constexpr bool ok() const { return (size_ & kStatusMask) == 0u; }

  constexpr Status status() const {
    return static_cast<Status::Code>((size_ & kStatusMask) >> kStatusShift);
  }

  // Functions for checking which status the StatusWithSize contains.
  [[nodiscard]] constexpr bool IsCancelled() const {
    return status().IsCancelled();
  }
  [[nodiscard]] constexpr bool IsUnknown() const {
    return status().IsUnknown();
  }
  [[nodiscard]] constexpr bool IsInvalidArgument() const {
    return status().IsInvalidArgument();
  }
  [[nodiscard]] constexpr bool IsDeadlineExceeded() const {
    return status().IsDeadlineExceeded();
  }
  [[nodiscard]] constexpr bool IsNotFound() const {
    return status().IsNotFound();
  }
  [[nodiscard]] constexpr bool IsAlreadyExists() const {
    return status().IsAlreadyExists();
  }
  [[nodiscard]] constexpr bool IsPermissionDenied() const {
    return status().IsPermissionDenied();
  }
  [[nodiscard]] constexpr bool IsResourceExhausted() const {
    return status().IsResourceExhausted();
  }
  [[nodiscard]] constexpr bool IsFailedPrecondition() const {
    return status().IsFailedPrecondition();
  }
  [[nodiscard]] constexpr bool IsAborted() const {
    return status().IsAborted();
  }
  [[nodiscard]] constexpr bool IsOutOfRange() const {
    return status().IsOutOfRange();
  }
  [[nodiscard]] constexpr bool IsUnimplemented() const {
    return status().IsUnimplemented();
  }
  [[nodiscard]] constexpr bool IsInternal() const {
    return status().IsInternal();
  }
  [[nodiscard]] constexpr bool IsUnavailable() const {
    return status().IsUnavailable();
  }
  [[nodiscard]] constexpr bool IsDataLoss() const {
    return status().IsDataLoss();
  }
  [[nodiscard]] constexpr bool IsUnauthenticated() const {
    return status().IsUnauthenticated();
  }

 private:
  size_t size_;
};

}  // namespace pw

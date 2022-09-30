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

#include <cstdint>

namespace pw::kvs::internal {

// Whether an entry is present or deleted.
enum class EntryState : bool { kValid, kDeleted };

// Essential metadata for an entry that is stored in memory.
struct KeyDescriptor {
  uint32_t key_hash;
  uint32_t transaction_id;

  EntryState state;  // TODO: Pack into transaction ID? or something?
};

}  // namespace pw::kvs::internal

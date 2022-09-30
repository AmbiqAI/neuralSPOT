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

#include <array>
#include <cstddef>
#include <limits>
#include <span>

#include "pw_stream/stream.h"
#include "pw_sys_io/sys_io.h"

namespace pw::stream {

class SysIoWriter : public Writer {
 private:
  Status DoWrite(std::span<const std::byte> data) override {
    return pw::sys_io::WriteBytes(data).status();
  }
};

class SysIoReader : public Reader {
 private:
  StatusWithSize DoRead(ByteSpan dest) override {
    return pw::sys_io::ReadBytes(dest);
  }
};

}  // namespace pw::stream

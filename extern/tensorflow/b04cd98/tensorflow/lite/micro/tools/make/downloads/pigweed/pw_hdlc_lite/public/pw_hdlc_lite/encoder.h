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

#include "pw_bytes/span.h"
#include "pw_status/status.h"
#include "pw_stream/stream.h"

namespace pw::hdlc_lite {

// Writes an HDLC information frame (I-frame) to the provided writer. The frame
// contains the following:
//
//   - HDLC flag byte (0x7e)
//   - Address
//   - Control byte (fixed at 0; sequence numbers are not used currently).
//   - Data (0 or more bytes)
//   - Frame check sequence (CRC-32)
//   - HDLC flag byte (0x7e)
//
Status WriteInformationFrame(uint8_t address,
                             ConstByteSpan data,
                             stream::Writer& writer);

}  // namespace pw::hdlc_lite

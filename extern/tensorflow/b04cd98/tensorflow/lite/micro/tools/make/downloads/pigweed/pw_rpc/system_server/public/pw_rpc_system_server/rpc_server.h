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

#include "pw_rpc/server.h"
#include "pw_stream/stream.h"

namespace pw::rpc::system_server {

// Initialization.
void Init();

// Get the reference of RPC Server instance.
pw::rpc::Server& Server();

// Start the server and processing packets. May not return.
Status Start();

}  // namespace pw::rpc::system_server

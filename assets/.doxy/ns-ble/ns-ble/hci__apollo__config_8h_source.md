

# File hci\_apollo\_config.h

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-ble**](dir_ec3c5c5ea2d338d436d6fa61f38fc381.md) **>** [**src**](dir_cf8bc0902f5dfb1bbd89749c3ff54123.md) **>** [**hci\_apollo\_config.h**](hci__apollo__config_8h.md)

[Go to the documentation of this file](hci__apollo__config_8h.md)

```C++

//*****************************************************************************
//
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision ambiqvos-754956a890 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <stdint.h>
#include "am_bsp.h"

#ifndef HCI_APOLLO_CONFIG_H
    #define HCI_APOLLO_CONFIG_H

//*****************************************************************************
//
// Pin numbers and configuration.
//
// NOTE: RTS, CTS, and RESET are implemented as GPIOs, so no "CFG" field is
// needed.
//
//*****************************************************************************
//#define HCI_APOLLO_POWER_PIN            AM_BSP_GPIO_EM9304_POWER
//#define HCI_APOLLO_POWER_CFG            AM_BSP_GPIO_CFG_EM9304_POWER

    #define HCI_APOLLO_RESET_PIN AM_BSP_GPIO_EM9304_RESET

    //*****************************************************************************
    //
    // Other options.
    //
    // These options are provided in case your board setup is a little more
    // unusual. Most boards shouldn't need these features. If in doubt, leave all
    // of these features disabled.
    //
    //*****************************************************************************
    #define HCI_APOLLO_CFG_OVERRIDE_ISR 1 // Override the exactle UART ISR

#endif // HCI_APOLLO_CONFIG_H

```


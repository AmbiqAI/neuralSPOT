

# File Platform.h

[**File List**](files.md) **>** [**arducam**](dir_42188ad7aa97eaa796727a73223a8863.md) **>** [**Platform.h**](_platform_8h.md)

[Go to the documentation of this file](_platform_8h.md)

```C++


/*
 * This file is part of the Arducam SPI Camera project.
 *
 * Copyright 2021 Arducam Technology co., Ltd. All Rights Reserved.
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 */

#ifndef __PLATFORM_H
#define __PLATFORM_H

#define arducamSpiBegin()
#define arducamSpiTransfer(val) arducamSpiTransfer(val) //  SPI communication sends a byte
#define arducamSpiReadBlock(p_val, count) arducamSpiTransferBlock(p_val, count)
#define arducamSpiCsPinHigh(pin)
#define arducamSpiCsPinLow(pin)
#define arducamCsOutputMode(pin)
#define arducamDelayMs(val) arducam_delay_ms(val) //  Delay Ms
#define arducamDelayUs(val) arducam_delay_us(val) // Delay Us

#endif /*__PLATFORM_H*/

```


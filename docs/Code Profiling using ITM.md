# Code Profiling using ITM

This application note describes how to use Apollo's built-in debugging hardware to perform code profiling using non-invasive program counter (PC) sampling over the ITM SWO interface. 

## Requirements

- SEGGER Jlink GDB Server
- Orbuculum ITM Tool Suite
- Arm GDB (via command line or VS Code)

### Glossary

- ITM - Instrumentation Trace Macrocell, an Arm component that allows non-invasive sampling of certain HW events (including periodic program-counter sampling).
- SWO - single wire output, a JLink protocol
- PC - program counter, used by Orbtop (in conjunction with your ELF file) to determine where the application is spending its cycles.

## Overview

The following setup is needed:

1. Apollo MCU needs to be configured to sample the program counter periodically and emit this data via SWO
2. The Jlink GDB Server needs to be started and connected to EVB - it will connect the SWO interface to a telnet port
3. A GDB session must be connected to the server in order to issue an command to enable Jlink's SWO interface.
4. Orbtop (one of the orbuculum apps) must be started and connected to the SWO telnet port

> **IMPORTANT**: The SWO viewer (GUI or via `make view`)conflicts with ITM sampling - if the SWO viewer is started, sampling will stop. 

### Configuring the MCU

Both profiling and ITM printing share the ITM interface. Both can be enabled at the same time, but ITM tracing can be enabled without enable ITM printing.

To enable ITM tracing without enabling ITM printing:

```c
#include "ns_perf_profile.h"

void main(void) {
  ...
  ret = ns_itm_pcsamp_enable();
}
```

To enable ITM tracing on top of ITM printing:

```c
#include "ns_perf_profile.h"

void main(void) {
  ...
  ns_itm_printf_enable();
  ret = ns_perf_enable_pcsamp();
}
```

### Starting a GDB Server

If you have embedded GDB enabled in VS Code, that is the easiest way to start a GDB server and connect a session to it. Otherwise, it can also be done from the command line (showing commands for Apollo510):

```bash
$> JLinkGDBServerCL  -device AP510NFA-CBR  -if SWD -ir
SEGGER J-Link GDB Server V8.10b Command Line Version

JLinkARM.dll V8.10b (DLL compiled Oct  9 2024 15:04:46)

WARNING: Unknown command line parameter --help found.
Command line: -device AP510NFA-CBR -if SWD -ir --help
-----GDB Server start settings-----
GDBInit file:                  none
GDB Server Listening port:     2331  # Note this port number
SWO raw output listening port: 2332  # Note this port number
Terminal I/O port:             2333
Accept remote connection:      yes
Generate logfile:              off
Verify download:               off
Init regs on start:            on
Silent mode:                   off
Single run mode:               off
Target connection timeout:     0 ms
------J-Link related settings------
J-Link Host interface:         USB
J-Link script:                 none
J-Link settings file:          none
------Target related settings------
Target device:                 AP510NFA-CBR
Target device parameters:      none
Target interface:              SWD
Target interface speed:        4000kHz
Target endian:                 little
```

This will start the server and wait for a client session to connect.

### Connecting a GDB Client and configuring SWO

In a separate terminal window, start a GDB client session:

```bash
$> arm-none-eabi-gdb firmware-to-be-profiled.axf # Specify the AXF or ELF file for your firmware

GNU gdb (Arm GNU Toolchain 13.2.rel1 (Build arm-13.7)) 13.2.90.20231008-git
...
Reading symbols from firmware-to-be-profiled.axf...
(gdb) target remote :2331  # Connect to the GDB server (the port is shown above)
Remote debugging using :2331
...
(gdb) monitor reset        # Reset the device
Resetting target
(gdb) monitor SWO start 0 1000000 # Connect the SWO interface (1000000 = 1Mbaud)
SWO started.
(gdb) c
Continuing.
```

### Use Orbtop to monitor the PC samples

```bash
$> orbtop -c 15  -e ../../../neuralSPOT/build/apollo5b_evb/arm-none-eabi/examples/vision/vision.axf -E -l -s localhost:2332 -p ITM

 40.72%     4233 br_util_delay_cycles
 20.31%     2112 br_util_delay_cycles::405
 15.89%     1652 main::614
 15.51%     1613 main::630
  2.52%      262 am_hal_timer_clear
  0.91%       95 am_hal_timer_interrupt_clear
  0.37%       39 usbd_int_set::1158
  0.34%       36 am_hal_interrupt_master_disable
  0.26%       28 No Function Name
  0.25%       27 am_hal_iom_blocking_transfer
  0.20%       21 tud_task_ext::481
  0.17%       18 am_hal_interrupt_master_set
  0.17%       18 tud_task_ext::610
  0.16%       17 tu_fifo_read::706
  0.16%       17 dcd_int_disable::277
-----------------
 97.94%    10188 of  10395  Samples


 Exception         |   Count  |  MaxD | TotalTicks  |   %   |  AveTicks  |  minTicks  |  maxTicks  |  maxWall
-------------------+----------+-------+-------------+-------+------------+------------+------------+----------

[---H] Interval = 1000ms
```




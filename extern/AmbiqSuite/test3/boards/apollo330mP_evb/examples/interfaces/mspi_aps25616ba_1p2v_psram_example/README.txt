Name:
=====
 mspi_hex_ddr_aps25616ba_psram_example


Description:
============
 Example of the MSPI operation with DDR HEX SPI PSRAM APS25616BA.



Purpose:
========
This example demonstrates MSPI DDR HEX operation using the
 APS25616BA PSRAM device.

 Starting from Apollo510, the MSPI XIP read bandwidth is boosted by the
 ARM CM55 cache and the MSPI CPU read combine feature. By default,
 the CPU read queue is on(CPURQEN). Cache prefetch(RID3) and cache miss(RID2)
 requests deemed appropriate by MSPI hardware are combined and processed
 with a 2:1 ratio(GQARB) between general queue and CPU read queue.

 am_hal_mspi_cpu_read_burst_t default =
 {
     .eGQArbBais                         = AM_HAL_MSPI_GQARB_2_1_BAIS,
     .bCombineRID2CacheMissAccess        = true,
     .bCombineRID3CachePrefetchAccess    = true,
     .bCombineRID4ICacheAccess           = false,
     .bCPUReadQueueEnable                = true,
 }

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.

 AM_DEBUG_PRINTF

 When defined, debug messages will be sent over ITM/SWO at 1M Baud.


******************************************************************************



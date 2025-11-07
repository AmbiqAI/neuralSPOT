/*
 * Copyright (c) 2025 Ambiq Micro, Inc.
 *
 */

#ifndef RPMSG_CONFIG_H__
#define RPMSG_CONFIG_H__

/*----------------------------------------------------------------------------------------
|  STATUS  |     RX BUFFER(VQ0)     |     TX BUFFER(VQ0)     |  RX VRING  |  TX VRING  |
----------------------------------------------------------------------------------------*/
// RPMSG SERVICE SHARE MEMORY: STATUS + RX BUFFER + TX BUFFER + RX VRING + TX VRING
// SHARE MEMORY: RX BUFFER + TX BUFFER + RX VRING + TX VRING
// SHARE BUFFER: RX BUFFER + TX BUFFER

/** The memory alignment should be multiple of cache line size. */
#define CONFIG_IPC_SERVICE_STATIC_VRINGS_MEM_ALIGNMENT                  (32)

/** Number of endpoints one instance supports. */
#define CONFIG_IPC_SERVICE_BACKEND_RPMSG_NUM_ENDPOINTS_PER_INSTANCE     (2)

/** The maximum packet size usage is for BT classic 2-DH5 packet, the maximum 
 * payload length is 679 and the maximum total packet size is 683. There is
 * 16-byte for RPMSG packet header, total 699. Define 704 for 32-byte alignment.
 */
#define RPMSG_BUFFER_SIZE  (704)

#endif // RPMSG_CONFIG_H__

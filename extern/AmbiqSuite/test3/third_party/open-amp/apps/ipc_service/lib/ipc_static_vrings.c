/*
 * Copyright (c) 2021 Carlo Caione <ccaione@baylibre.com>
 * Copyright (c) 2022, Ambiq Micro, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ipc_static_vrings.h>
#include "libmetal/lib/errno.h"


#define RPMSG_VQ_0 (0) /* TX virtqueue queue index */
#define RPMSG_VQ_1 (1) /* RX virtqueue queue index */

static void ipc_virtio_notify(struct virtqueue *vq)
{
    struct ipc_static_vrings *vr;

    vr = CONTAINER_OF(vq->vq_dev, struct ipc_static_vrings, vdev);

    if (vr->notify_cb)
    {
        vr->notify_cb(vq, vr->priv);
    }
}

static void ipc_virtio_set_features(struct virtio_device *vdev, uint32_t features)
{
    /* No need for implementation */
    (void)vdev;
    (void)features;
}

static void ipc_virtio_set_status(struct virtio_device *p_vdev, unsigned char status)
{
    struct ipc_static_vrings *vr;

    if (p_vdev->role != VIRTIO_DEV_DRIVER)
    {
        return;
    }

    vr = CONTAINER_OF(p_vdev, struct ipc_static_vrings, vdev);

    *(volatile uint8_t *)vr->status_reg_addr = status;

    /* Ambiq: Clean the D-Cache after writing the status */
    VRING_FLUSH((void *)vr->status_reg_addr, sizeof(status));
}

static uint32_t ipc_virtio_get_features(struct virtio_device *vdev)
{
    (void)vdev;
    return BIT_GET(VIRTIO_RPMSG_F_NS);
}

static unsigned char ipc_virtio_get_status(struct virtio_device *p_vdev)
{
    struct ipc_static_vrings *vr;
    uint8_t ret;

    vr = CONTAINER_OF(p_vdev, struct ipc_static_vrings, vdev);

    ret = VIRTIO_CONFIG_STATUS_DRIVER_OK;

    if (p_vdev->role == VIRTIO_DEV_DEVICE)
    {
        /* Ambiq: Invalidate the D-Cache before reading the status */
        VRING_INVALIDATE((void *)vr->status_reg_addr, sizeof(ret));
        ret = *(volatile uint8_t *)vr->status_reg_addr;
    }

    return ret;
}

const static struct virtio_dispatch dispatch =
{
    .get_status = ipc_virtio_get_status,
    .get_features = ipc_virtio_get_features,
    .set_status = ipc_virtio_set_status,
    .set_features = ipc_virtio_set_features,
    .notify = ipc_virtio_notify,
};

static int vq_setup(struct ipc_static_vrings *vr, unsigned int role)
{
    vr->vq[RPMSG_VQ_0] = virtqueue_allocate(vr->vring_size);
    if (vr->vq[RPMSG_VQ_0] == NULL)
    {
        return -ENOMEM;
    }

    vr->vq[RPMSG_VQ_1] = virtqueue_allocate(vr->vring_size);
    if (vr->vq[RPMSG_VQ_1] == NULL)
    {
        return -ENOMEM;
    }

    vr->rvrings[RPMSG_VQ_0].io = &vr->shm_io;
    vr->rvrings[RPMSG_VQ_0].info.vaddr = (void *)vr->tx_addr;
    vr->rvrings[RPMSG_VQ_0].info.num_descs = vr->vring_size;
    vr->rvrings[RPMSG_VQ_0].info.align = MEM_ALIGNMENT;
    vr->rvrings[RPMSG_VQ_0].vq = vr->vq[RPMSG_VQ_0];

    vr->rvrings[RPMSG_VQ_1].io = &vr->shm_io;
    vr->rvrings[RPMSG_VQ_1].info.vaddr = (void *)vr->rx_addr;
    vr->rvrings[RPMSG_VQ_1].info.num_descs = vr->vring_size;
    vr->rvrings[RPMSG_VQ_1].info.align = MEM_ALIGNMENT;
    vr->rvrings[RPMSG_VQ_1].vq = vr->vq[RPMSG_VQ_1];

    vr->vdev.role = role;

    vr->vdev.vrings_num = VRING_COUNT;
    vr->vdev.func = &dispatch;
    vr->vdev.vrings_info = &vr->rvrings[0];

    return 0;
}

static int vq_teardown(struct ipc_static_vrings *vr, unsigned int role)
{
    memset(&vr->vdev, 0, sizeof(struct virtio_device));

    memset(&(vr->rvrings[RPMSG_VQ_1]), 0, sizeof(struct virtio_vring_info));
    memset(&(vr->rvrings[RPMSG_VQ_0]), 0, sizeof(struct virtio_vring_info));

    virtqueue_free(vr->vq[RPMSG_VQ_1]);
    virtqueue_free(vr->vq[RPMSG_VQ_0]);

    return 0;
}

int ipc_static_vrings_init(struct ipc_static_vrings *vr, unsigned int role)
{
	struct metal_init_params metal_params = METAL_INIT_DEFAULTS;
    int err = 0;

    if (!vr)
    {
        return -EINVAL;
    }

	err = metal_init(&metal_params);
	if (err != 0)
    {
		return err;
	}

    vr->shm_physmap[0] = vr->shm_addr;

    metal_io_init(&vr->shm_io, (void *)vr->shm_addr,
                  vr->shm_physmap, vr->shm_size, -1, 0, NULL);

    return vq_setup(vr, role);
}

int ipc_static_vrings_deinit(struct ipc_static_vrings *vr, unsigned int role)
{
    int err;

    err = vq_teardown(vr, role);
    if (err != 0)
    {
        return err;
    }

    return 0;
}


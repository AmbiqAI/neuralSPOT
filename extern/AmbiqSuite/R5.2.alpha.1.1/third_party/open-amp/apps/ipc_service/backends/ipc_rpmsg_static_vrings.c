/*
 * Copyright (c) 2021 Carlo Caione <ccaione@baylibre.com>
 * Copyright (c) 2022, Ambiq Micro, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmetal/lib/errno.h"
#include "libmetal/lib/atomic.h"

#include <ipc_service_backend.h>
#include <ipc_static_vrings.h>
#include <ipc_rpmsg.h>

#include "ipc_rpmsg_static_vrings.h"
#include "rpmsg_internal.h"

#define STATE_READY (0)
#define STATE_BUSY (1)
#define STATE_INITED (2)

struct backend_data_t
{
    /* RPMsg */
    struct ipc_rpmsg_instance rpmsg_inst;

    /* Static VRINGs */
    struct ipc_static_vrings vr;

    /* General */
    unsigned int role;
    atomic_int state;

    /* TX buffer size */
    int tx_buffer_size;
};

static void rpmsg_service_unbind(struct rpmsg_endpoint *ep)
{
    rpmsg_destroy_ept(ep);
}

static struct ipc_rpmsg_ept *get_ept_slot_with_name(struct ipc_rpmsg_instance *rpmsg_inst,
                                                    const char *name)
{
    struct ipc_rpmsg_ept *rpmsg_ept;

    for (size_t i = 0; i < NUM_ENDPOINTS; i++)
    {
        rpmsg_ept = &rpmsg_inst->endpoint[i];

        if (strcmp(name, rpmsg_ept->name) == 0)
        {
            return &rpmsg_inst->endpoint[i];
        }
    }

    return NULL;
}

static struct ipc_rpmsg_ept *get_available_ept_slot(struct ipc_rpmsg_instance *rpmsg_inst)
{
    return get_ept_slot_with_name(rpmsg_inst, "");
}

static bool check_endpoints_freed(struct ipc_rpmsg_instance *rpmsg_inst)
{
    struct ipc_rpmsg_ept *rpmsg_ept;

    for (size_t i = 0; i < NUM_ENDPOINTS; i++)
	{
        rpmsg_ept = &rpmsg_inst->endpoint[i];

        if (rpmsg_ept->bound == true)
		{
            return false;
        }
    }

    return true;
}

/*
 * Returns:
 *  - true:  when the endpoint was already cached / registered
 *  - false: when the endpoint was never registered before
 *
 * Returns in **rpmsg_ept:
 *  - The endpoint with the name *name if it exists
 *  - The first endpoint slot available when the endpoint with name *name does
 *    not exist
 *  - NULL in case of error
 */
static bool get_ept(struct ipc_rpmsg_instance *rpmsg_inst,
                    struct ipc_rpmsg_ept **rpmsg_ept, const char *name)
{
    struct ipc_rpmsg_ept *ept;

    ept = get_ept_slot_with_name(rpmsg_inst, name);
    if (ept != NULL)
    {
        (*rpmsg_ept) = ept;
        return true;
    }

    ept = get_available_ept_slot(rpmsg_inst);
    if (ept != NULL)
    {
        (*rpmsg_ept) = ept;
        return false;
    }

    (*rpmsg_ept) = NULL;

    return false;
}

static void advertise_ept(struct ipc_rpmsg_instance *rpmsg_inst, struct ipc_rpmsg_ept *rpmsg_ept,
                          const char *name, uint32_t dest)
{
    struct rpmsg_device *rdev;
    int err;

    rdev = rpmsg_virtio_get_rpmsg_device(&rpmsg_inst->rvdev);

    err = rpmsg_create_ept(&rpmsg_ept->ep, rdev, name, RPMSG_ADDR_ANY,
                           dest, rpmsg_inst->cb, rpmsg_service_unbind);
    if (err != 0)
    {
        return;
    }

    rpmsg_ept->bound = true;
    if (rpmsg_inst->bound_cb)
    {
        rpmsg_inst->bound_cb(rpmsg_ept);
    }
}

static void ns_bind_cb(struct rpmsg_device *rdev, const char *name, uint32_t dest)
{
    struct ipc_rpmsg_instance *rpmsg_inst;
    struct rpmsg_virtio_device *p_rvdev;
    struct ipc_rpmsg_ept *rpmsg_ept;
    bool ept_cached;

    p_rvdev = CONTAINER_OF(rdev, struct rpmsg_virtio_device, rdev);
    rpmsg_inst = CONTAINER_OF(p_rvdev->shpool, struct ipc_rpmsg_instance, shm_pool);

    if (name == NULL || name[0] == '\0')
    {
        return;
    }

    __metal_mutex_acquire(&rpmsg_inst->mtx);
    ept_cached = get_ept(rpmsg_inst, &rpmsg_ept, name);

    if (rpmsg_ept == NULL)
    {
        metal_mutex_release(&rpmsg_inst->mtx);
        return;
    }

    if (ept_cached)
    {
        /*
         * The endpoint was already registered by the HOST core. The
         * endpoint can now be advertised to the REMOTE core.
         */
        metal_mutex_release(&rpmsg_inst->mtx);
        advertise_ept(rpmsg_inst, rpmsg_ept, name, dest);
    }
    else
    {
        /*
         * The endpoint is not registered yet, this happens when the
         * REMOTE core registers the endpoint before the HOST has
         * had the chance to register it. Cache it saving name and
         * destination address to be used by the next register_ept()
         * call by the HOST core.
         */
        strncpy(rpmsg_ept->name, name, sizeof(rpmsg_ept->name));
        rpmsg_ept->name[RPMSG_NAME_SIZE - 1] = '\0';
        rpmsg_ept->dest = dest;
        metal_mutex_release(&rpmsg_inst->mtx);
    }
}

static void bound_cb(struct ipc_rpmsg_ept *ept)
{
    rpmsg_send(&ept->ep, (uint8_t *)"", 0);

    if (ept->cb->bound)
    {
        ept->cb->bound(ept->priv);
    }
}

static int ept_cb(struct rpmsg_endpoint *ep, void *data, size_t len, uint32_t src, void *priv)
{
    struct ipc_rpmsg_ept *ept;

    ept = (struct ipc_rpmsg_ept *)priv;

    /*
     * the remote processor has send a ns announcement, we use an empty
     * message to advice the remote side that a local endpoint has been
     * created and that the processor is ready to communicate with this
     * endpoint
     *
     * ipc_rpmsg_register_ept
     *  rpmsg_send_ns_message --------------> ns_bind_cb
     *                                        bound_cb
     *                ept_cb <--------------- rpmsg_send [empty message]
     *              bound_cb
     */
    if (len == 0)
    {
        if (!ept->bound)
        {
            ept->bound = true;
            bound_cb(ept);
        }
        return RPMSG_SUCCESS;
    }

    if (ept->cb->received)
    {
        ept->cb->received(data, len, ept->priv);
    }

    return RPMSG_SUCCESS;
}

static int vr_shm_configure(struct ipc_static_vrings *vr, const struct backend_config *conf)
{
    unsigned int num_desc;

    num_desc = optimal_num_desc(conf->shm_size, conf->buffer_size);
    if (num_desc == 0)
    {
        return -ENOMEM;
    }

	/*
	 * conf->shm_addr  +--------------+  vr->status_reg_addr
	 *		   |    STATUS    |
	 *		   +--------------+  vr->shm_addr
	 *		   |              |
	 *		   |              |
	 *		   |   RX BUFS    |
	 *		   |              |
	 *		   |              |
	 *		   +--------------+
	 *		   |              |
	 *		   |              |
	 *		   |   TX BUFS    |
	 *		   |              |
	 *		   |              |
	 *		   +--------------+  vr->rx_addr (aligned)
	 *		   |   RX VRING   |
	 *		   +--------------+  vr->tx_addr (aligned)
	 *		   |   TX VRING   |
	 *		   +--------------+
	 */
    vr->shm_addr = ROUND_UP(conf->shm_addr + VDEV_STATUS_SIZE, MEM_ALIGNMENT);
    vr->shm_size = shm_size(num_desc, conf->buffer_size);

    vr->rx_addr = vr->shm_addr + VRING_COUNT * vq_ring_size(num_desc, conf->buffer_size);
    vr->tx_addr = ROUND_UP(vr->rx_addr + vring_size(num_desc, MEM_ALIGNMENT), MEM_ALIGNMENT);

    vr->status_reg_addr = conf->shm_addr;

    vr->vring_size = num_desc;

    return 0;
}

static void virtio_notify_cb(struct virtqueue *vq, void *priv)
{
    struct backend_config *conf = priv;
    if (conf->mbox_send)
	{
        conf->mbox_send(conf->mbox_data_send);
	}
}

void ipc_receive(struct ipc_device *instance)
{
    struct backend_data_t *data;
    unsigned int vq_id;

    data = instance->data;
    vq_id = (data->role == ROLE_HOST) ? VIRTQUEUE_ID_HOST : VIRTQUEUE_ID_REMOTE;

    virtqueue_notification(data->vr.vq[vq_id]);
}

static struct ipc_rpmsg_ept *register_ept_on_host(struct ipc_rpmsg_instance *rpmsg_inst,
                                                  const struct ipc_ept_cfg *cfg)
{
    struct ipc_rpmsg_ept *rpmsg_ept;
    bool ept_cached;

    __metal_mutex_acquire(&rpmsg_inst->mtx);

    ept_cached = get_ept(rpmsg_inst, &rpmsg_ept, cfg->name);
    if (rpmsg_ept == NULL)
    {
        metal_mutex_release(&rpmsg_inst->mtx);
        return NULL;
    }

    rpmsg_ept->cb = &cfg->cb;
    rpmsg_ept->priv = cfg->priv;
    rpmsg_ept->bound = false;
    rpmsg_ept->ep.priv = rpmsg_ept;

    if (ept_cached)
    {
        /*
         * The endpoint was cached in the NS bind callback. We can finally
         * advertise it.
         */
        metal_mutex_release(&rpmsg_inst->mtx);
        advertise_ept(rpmsg_inst, rpmsg_ept, cfg->name, rpmsg_ept->dest);
    }
    else
    {
        /*
         * There is no endpoint in the cache because the REMOTE has
         * not registered the endpoint yet. Cache it.
         */
        strncpy(rpmsg_ept->name, cfg->name, sizeof(rpmsg_ept->name));
		rpmsg_ept->name[RPMSG_NAME_SIZE - 1] = '\0';
        metal_mutex_release(&rpmsg_inst->mtx);
    }

    return rpmsg_ept;
}

static struct ipc_rpmsg_ept *register_ept_on_remote(struct ipc_rpmsg_instance *rpmsg_inst,
                                                    const struct ipc_ept_cfg *cfg)
{
    struct ipc_rpmsg_ept *rpmsg_ept;
    int err;

    rpmsg_ept = get_available_ept_slot(rpmsg_inst);
    if (rpmsg_ept == NULL)
    {
        return NULL;
    }

    rpmsg_ept->cb = &cfg->cb;
    rpmsg_ept->priv = cfg->priv;
    rpmsg_ept->bound = false;
    rpmsg_ept->ep.priv = rpmsg_ept;

    strncpy(rpmsg_ept->name, cfg->name, sizeof(rpmsg_ept->name));
    rpmsg_ept->name[RPMSG_NAME_SIZE - 1] = '\0';

    err = ipc_rpmsg_register_ept(rpmsg_inst, RPMSG_REMOTE, rpmsg_ept);
    if (err != 0)
    {
        return NULL;
    }

    return rpmsg_ept;
}

static int register_ept(const struct ipc_device *instance, void **token,
                        const struct ipc_ept_cfg *cfg)
{
    struct backend_data_t *data = instance->data;
    struct ipc_rpmsg_instance *rpmsg_inst;
    struct ipc_rpmsg_ept *rpmsg_ept;

    /* Instance is not ready */
    if (__atomic_load_n((int *)&(data->state), __ATOMIC_SEQ_CST) !=
        STATE_INITED)
    {
        return -EBUSY;
    }

    /* Empty name is not valid */
    if (cfg->name == NULL || cfg->name[0] == '\0')
    {
        return -EINVAL;
    }

    rpmsg_inst = &data->rpmsg_inst;

    rpmsg_ept = (data->role == ROLE_HOST) ? register_ept_on_host(rpmsg_inst, cfg) : register_ept_on_remote(rpmsg_inst, cfg);
    if (rpmsg_ept == NULL)
    {
        return -EINVAL;
    }

    (*token) = rpmsg_ept;

    return 0;
}

static int deregister_ept(const struct ipc_device *instance, void *token)
{
    struct backend_data_t *data = instance->data;
    struct ipc_rpmsg_ept *rpmsg_ept;

    /* Instance is not ready */
    if (__atomic_load_n((int *)&(data->state), __ATOMIC_SEQ_CST) !=
        STATE_INITED)
    {
        return -EBUSY;
    }

    rpmsg_ept = (struct ipc_rpmsg_ept *) token;

    /* Endpoint is not registered with instance */
	if (!rpmsg_ept)
    {
        return -ENOENT;
    }

    rpmsg_destroy_ept(&rpmsg_ept->ep);

    memset(rpmsg_ept, 0, sizeof(struct ipc_rpmsg_ept));

    return 0;
}

static int send(const struct ipc_device *instance, void *token,
                const void *msg, size_t len)
{
    struct backend_data_t *data = instance->data;
    struct ipc_rpmsg_ept *rpmsg_ept;
    int                    ret;

    /* Instance is not ready */
    if (__atomic_load_n((int *)&(data->state), __ATOMIC_SEQ_CST) !=
        STATE_INITED)
    {
        return -EBUSY;
    }

    /* Empty message is not allowed */
    if (len == 0)
    {
        return -EBADMSG;
    }

    rpmsg_ept = (struct ipc_rpmsg_ept *)token;

    /* Endpoint is not registered with instance */
    if (!rpmsg_ept)
    {
        return -ENOENT;
    }

    ret = rpmsg_send(&rpmsg_ept->ep, msg, len);

    /* No buffers available */
    if (ret == RPMSG_ERR_NO_BUFF)
    {
        return -ENOMEM;
    }

    return ret;
}

static int send_nocopy(const struct ipc_device *instance, void *token,
                const void *msg, size_t len)
{
    struct backend_data_t *data = instance->data;
    struct ipc_rpmsg_ept *rpmsg_ept;

    /* Instance is not ready */
    if (__atomic_load_n((int *)&(data->state), __ATOMIC_SEQ_CST) !=
        STATE_INITED)
    {
        return -EBUSY;
    }

    /* Empty message is not allowed */
    if (len == 0)
    {
        return -EBADMSG;
    }

    rpmsg_ept = (struct ipc_rpmsg_ept *)token;

    /* Endpoint is not registered with instance */
    if (!rpmsg_ept)
    {
        return -ENOENT;
    }

    return rpmsg_send_nocopy(&rpmsg_ept->ep, msg, len);
}

static int open(const struct ipc_device *instance)
{
    const struct backend_config *conf = instance->config;
    struct backend_data_t *data = instance->data;
    struct ipc_rpmsg_instance *rpmsg_inst;
    struct rpmsg_device *rdev;
    int err;
    int c = STATE_READY;

    if (!atomic_compare_exchange_strong(&data->state, &c, STATE_BUSY))
    {
        return -EALREADY;
    }

    err = vr_shm_configure(&data->vr, conf);
    if (err != 0)
    {
        goto error;
    }

    data->vr.notify_cb = virtio_notify_cb;
    data->vr.priv = (void *)conf;

    err = ipc_static_vrings_init(&data->vr, conf->role);
    if (err != 0)
    {
        goto error;
    }

	conf->mbox_init();

    rpmsg_inst = &data->rpmsg_inst;

    rpmsg_inst->bound_cb = bound_cb;
    rpmsg_inst->cb = ept_cb;

    err = ipc_rpmsg_init(rpmsg_inst, data->role, conf->buffer_size,
	                     &data->vr.shm_io, &data->vr.vdev,
                         (void *)data->vr.shm_addr,
						 data->vr.shm_size, ns_bind_cb);
    if (err != 0)
    {
        goto error;
    }

	rdev = rpmsg_virtio_get_rpmsg_device(&rpmsg_inst->rvdev);

	data->tx_buffer_size = rpmsg_virtio_get_buffer_size(rdev);
	if (data->tx_buffer_size < 0)
    {
		err = -EINVAL;
		goto error;
	}

    atomic_store(&data->state, STATE_INITED);
    return 0;

error:
    /* Back to the ready state */
    atomic_store(&data->state, STATE_READY);
    return err;
}

static int close(const struct ipc_device *instance)
{
    const struct backend_config *conf = instance->config;
    struct backend_data_t *data = instance->data;
    struct ipc_rpmsg_instance *rpmsg_inst;
    int err;

    /* Instance is not ready */
    if (__atomic_load_n((int *)&(data->state), __ATOMIC_SEQ_CST) !=
        STATE_INITED)
    {
        return -EBUSY;
    }
    atomic_store(&data->state, STATE_BUSY);

    rpmsg_inst = &data->rpmsg_inst;

    if (!check_endpoints_freed(rpmsg_inst))
    {
        return -EBUSY;
    }

    err = ipc_rpmsg_deinit(rpmsg_inst, data->role);
    if (err != 0)
    {
        goto error;
    }

    err = ipc_static_vrings_deinit(&data->vr, conf->role);
    if (err != 0)
    {
        goto error;
    }

    memset(&data->vr, 0, sizeof(struct ipc_static_vrings));
    memset(rpmsg_inst, 0, sizeof(struct ipc_rpmsg_instance));

    atomic_store(&data->state, STATE_READY);
    return 0;

error:
    /* Back to the inited state */
    atomic_store(&data->state, STATE_INITED);
    return err;
}


static int get_tx_buffer_size(const struct ipc_device *instance, void *token)
{
    struct backend_data_t *data = instance->data;

    return data->tx_buffer_size;
}

static int get_tx_buffer(const struct ipc_device *instance, void *token,
                         void **r_data, uint32_t *size, int wait)
{
    struct backend_data_t *data = instance->data;
    struct ipc_rpmsg_ept *rpmsg_ept;
    void                 *payload;

    rpmsg_ept = (struct ipc_rpmsg_ept *)token;

    /* Endpoint is not registered with instance */
    if (!rpmsg_ept)
    {
        return -ENOENT;
    }

    if (!r_data || !size)
    {
        return -EINVAL;
    }

    /* OpenAMP only supports a binary wait / no-wait */

    /* The user requested a specific size */
    if ((*size) && (*size > data->tx_buffer_size))
    {
        /* Too big to fit */
        *size = data->tx_buffer_size;
        return -ENOMEM;
    }

    payload = rpmsg_get_tx_payload_buffer(&rpmsg_ept->ep, size, wait);
    if (!payload)
    {
        return -ENOBUFS;
    }

    (*r_data) = payload;

    return 0;
}

static int hold_rx_buffer(const struct ipc_device *instance, void *token,
                          void *data)
{
    struct ipc_rpmsg_ept *rpmsg_ept;

    rpmsg_ept = (struct ipc_rpmsg_ept *)token;

    /* Endpoint is not registered with instance */
    if (!rpmsg_ept)
    {
        return -ENOENT;
    }

    rpmsg_hold_rx_buffer(&rpmsg_ept->ep, data);

    return 0;
}

static int release_rx_buffer(const struct ipc_device *instance, void *token,
                             void *data)
{
    struct ipc_rpmsg_ept *rpmsg_ept;

    rpmsg_ept = (struct ipc_rpmsg_ept *)token;

    /* Endpoint is not registered with instance */
    if (!rpmsg_ept)
    {
        return -ENOENT;
    }

    rpmsg_release_rx_buffer(&rpmsg_ept->ep, data);

    return 0;
}

static int drop_tx_buffer(const struct ipc_device *instance, void *token,
                          const void *data)
{
    /* Not yet supported by OpenAMP */
    return -ENOTSUP;
}

const static struct ipc_service_backend backend_ops =
{
    .open_instance       = open,
    .close_instance      = close,
    .register_endpoint   = register_ept,
    .deregister_endpoint = deregister_ept,
    .send                = send,
    .send_nocopy         = send_nocopy,
    .drop_tx_buffer      = drop_tx_buffer,
    .get_tx_buffer       = get_tx_buffer,
    .get_tx_buffer_size  = get_tx_buffer_size,
    .hold_rx_buffer      = hold_rx_buffer,
    .release_rx_buffer   = release_rx_buffer,
};

static struct backend_data_t backend_data = {0};

int ipc_service_backend_init(struct ipc_device *instance)
{
    if ((instance == NULL) || (instance->name == NULL))
    {
        return -EINVAL;
    }

	const struct backend_config *conf = instance->config;

	instance->data = &backend_data;
	instance->api = &backend_ops;
	backend_data.role = conf->role;
	backend_data.tx_buffer_size = conf->buffer_size;

    if (conf->role == ROLE_HOST)
    {
        memset((void *)conf->shm_addr, 0, VDEV_STATUS_SIZE);
        /* Ambiq: we should flush the data cache to make sure the
         * device status can be updated as expected.
         */
        CACHE_FLUSH((void *)conf->shm_addr, VDEV_STATUS_SIZE);
    }

	metal_mutex_init(&backend_data.rpmsg_inst.mtx);
	atomic_store(&backend_data.state, STATE_READY);
	
	return 0;
}

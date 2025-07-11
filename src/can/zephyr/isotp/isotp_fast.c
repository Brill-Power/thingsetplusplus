/*
 * Copyright (c) 2019 Alexander Wachter
 * Copyright (c) 2023 Enphase Energy
 * Copyright (c) 2023-2025 Brill Power
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "isotp_fast_internal.h"
#include <assert.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>
#include "stdbool.h"

LOG_MODULE_REGISTER(isotp_fast, CONFIG_ISOTP_FAST_LOG_LEVEL);

static void receive_work_handler(struct k_work *work);
static void receive_timeout_handler(struct k_timer *timer);
static void receive_state_machine(struct isotp_fast_recv_ctx *rctx);
static void send_work_handler(struct k_work *work);
static void send_timeout_handler(struct k_timer *timer);

/**
 * Determines whether two @ref isotp_fast_addr structures are equal.
 */
static inline bool isotp_fast_addr_equal(struct isotp_fast_addr *left,
                                         struct isotp_fast_addr *right)
{
#ifdef CONFIG_ISOTP_FAST_EXTENDED_ADDRESSING
    return left->ext_id == right->ext_id && left->ext_addr == right->ext_addr;
#else
    return left->ext_id == right->ext_id;
#endif
}

/* Memory slab to hold send contexts */
K_MEM_SLAB_DEFINE(isotp_send_ctx_slab, sizeof(struct isotp_fast_send_ctx),
                  CONFIG_ISOTP_FAST_TX_BUF_COUNT, 4);

/* Memory slab to hold receive contexts */
K_MEM_SLAB_DEFINE(isotp_recv_ctx_slab, sizeof(struct isotp_fast_recv_ctx),
                  CONFIG_ISOTP_FAST_RX_BUF_COUNT, 4);

#ifdef CONFIG_ISOTP_FAST_BLOCKING_RECEIVE
/* Memory slab to hold blocking receive contexts */
K_MEM_SLAB_DEFINE(isotp_recv_await_ctx_slab, sizeof(struct isotp_fast_recv_await_ctx),
                  CONFIG_ISOTP_FAST_RX_BUF_COUNT, 4);
#endif

/**
 * Pool of buffers for incoming messages. The current implementation
 * sizes these to match the size of a CAN frame less the 1 header byte
 * that ISO-TP consumes. The important configuration options determining
 * the size of the buffer are therefore CONFIG_ISOTP_FAST_RX_BUF_COUNT (i.e. broad
 * number of buffers) and CONFIG_ISOTP_FAST_RX_MAX_PACKET_COUNT (i.e. how big a
 * message does one anticipate receiving).
 */
NET_BUF_POOL_DEFINE(isotp_rx_pool,
                    CONFIG_ISOTP_FAST_RX_MAX_PACKET_COUNT,
                    CAN_MAX_DLEN - 1, sizeof(int), NULL);

static int get_send_ctx(struct isotp_fast_ctx *ctx, struct isotp_fast_addr tx_addr, bool create_if_not_found,
                        struct isotp_fast_send_ctx **sctx)
{
    struct isotp_fast_send_ctx *context;

    SYS_SLIST_FOR_EACH_CONTAINER(&ctx->isotp_send_ctx_list, context, node)
    {
        if (isotp_fast_addr_equal(&context->tx_addr, &tx_addr)) {
            LOG_DBG("Found existing send context for recipient %x", tx_addr.ext_id);
            *sctx = context;
            return 0;
        }
    }

    if (create_if_not_found) {
        int err = k_mem_slab_alloc(&isotp_send_ctx_slab, (void **)&context, K_NO_WAIT);
        if (err != 0) {
            return ISOTP_NO_CTX_LEFT;
        }
        *sctx = context;
        context->ctx = ctx;
        context->tx_addr = tx_addr;
        context->bs = ctx->opts->bs;
        context->stmin = ctx->opts->stmin;
        context->state = ISOTP_TX_SEND_FF;
        context->error = 0;
        k_sem_init(&context->sem, 0, 1);
        k_work_init(&context->work, send_work_handler);
        k_timer_init(&context->timer, send_timeout_handler, NULL);
        sys_slist_append(&ctx->isotp_send_ctx_list, &context->node);
        LOG_DBG("Created new send context for recipient %x", tx_addr.ext_id);

        return 0;
    } else {
        return -ESRCH; // no such context, apparently
    }
}

static inline void free_send_ctx(struct isotp_fast_send_ctx *sctx)
{
    struct isotp_fast_addr tx_addr = sctx->tx_addr;
    LOG_DBG("Freeing send context %x", tx_addr.ext_id);
    k_timer_stop(&sctx->timer);
    sys_slist_find_and_remove(&sctx->ctx->isotp_send_ctx_list, &sctx->node);
    k_mem_slab_free(&isotp_send_ctx_slab, sctx);
    LOG_DBG("Freed send context %x", tx_addr.ext_id);
}

static inline void free_recv_ctx(struct isotp_fast_recv_ctx *rctx)
{
    struct isotp_fast_addr rx_addr = rctx->rx_addr;
    LOG_DBG("Freeing receive context %x", rx_addr.ext_id);
    k_timer_stop(&rctx->timer);
    sys_slist_find_and_remove(&rctx->ctx->isotp_recv_ctx_list, &rctx->node);
    net_buf_unref(rctx->buffer);
#ifdef ISOTP_FAST_RECEIVE_QUEUE
    k_msgq_purge(&rctx->recv_queue);
    k_msgq_cleanup(&rctx->recv_queue);
#endif
    k_mem_slab_free(&isotp_recv_ctx_slab, rctx);
    LOG_DBG("Freed receive context %x", rx_addr.ext_id);
}

static void free_recv_ctx_if_unowned(struct isotp_fast_recv_ctx *rctx)
{
#ifdef ISOTP_FAST_RECEIVE_QUEUE
    if (rctx->pending) {
        return;
    }
#endif
    free_recv_ctx(rctx);
}

static int get_recv_ctx(struct isotp_fast_ctx *ctx, struct isotp_fast_addr rx_addr, bool create_if_not_found,
                        struct isotp_fast_recv_ctx **rctx)
{
    struct isotp_fast_recv_ctx *context;

    SYS_SLIST_FOR_EACH_CONTAINER(&ctx->isotp_recv_ctx_list, context, node)
    {
        if (isotp_fast_addr_equal(&context->rx_addr, &rx_addr)) {
            LOG_DBG("Found existing receive context %x", rx_addr.ext_id);
            *rctx = context;
            context->frag = net_buf_alloc(&isotp_rx_pool, K_NO_WAIT);
            if (context->frag == NULL) {
                LOG_ERR("No free buffers");
                free_recv_ctx(*rctx);
                return ISOTP_NO_NET_BUF_LEFT;
            }
#ifndef ISOTP_FAST_RECEIVE_QUEUE
            net_buf_frag_add(context->buffer, context->frag);
#endif
            return 0;
        }
    }

    if (create_if_not_found) {
        int err = k_mem_slab_alloc(&isotp_recv_ctx_slab, (void **)&context, K_NO_WAIT);
        if (err != 0) {
            LOG_ERR("No space for receive context - error %d.", err);
            return ISOTP_NO_CTX_LEFT;
        }
        context->buffer = net_buf_alloc(&isotp_rx_pool, K_NO_WAIT);
        if (!context->buffer) {
            k_mem_slab_free(&isotp_recv_ctx_slab, context);
            LOG_ERR("No net bufs.");
            return ISOTP_NO_NET_BUF_LEFT;
        }
        context->frag = context->buffer;
        *rctx = context;
        context->ctx = ctx;
        context->state = ISOTP_RX_STATE_WAIT_FF_SF;
        context->rx_addr = rx_addr;
        context->error = 0;
    #ifdef ISOTP_FAST_RECEIVE_QUEUE
        k_msgq_init(&context->recv_queue, context->recv_queue_pool, sizeof(struct net_buf *),
                    CONFIG_ISOTP_FAST_RX_MAX_PACKET_COUNT);
        LOG_DBG("Queue of length %d created", k_msgq_num_free_get(&context->recv_queue));
    #endif
        k_work_init(&context->work, receive_work_handler);
        k_timer_init(&context->timer, receive_timeout_handler, NULL);
        sys_slist_append(&ctx->isotp_recv_ctx_list, &context->node);
        LOG_DBG("Created new receive context %x", rx_addr.ext_id);

        return 0;
    } else {
        return -ESRCH; // no such context, apparently
    }
}

struct isotp_fast_addr isotp_fast_get_reply_addr(struct isotp_fast_ctx *ctx,
                                                 const struct isotp_fast_addr *addr)
{
    switch (ctx->opts->addressing_mode) {
#ifdef CONFIG_ISOTP_FAST_NORMAL_ADDRESSING
        case ISOTP_FAST_ADDRESSING_MODE_NORMAL:
            return ctx->tx_addr;
#endif
#ifdef CONFIG_ISOTP_FAST_FIXED_ADDRESSING
        case ISOTP_FAST_ADDRESSING_MODE_FIXED:
            return isotp_fast_get_tx_addr_fixed(addr);
#endif
#ifdef CONFIG_ISOTP_FAST_EXTENDED_ADDRESSING
        case ISOTP_FAST_ADDRESSING_MODE_EXTENDED:
            return (struct isotp_fast_addr){
                .ext_addr = addr->ext_addr, // is this right?
            };
#ifdef CONFIG_ISOTP_FAST_FIXED_ADDRESSING
        case ISOTP_FAST_ADDRESSING_MODE_FIXED | ISOTP_FAST_ADDRESSING_MODE_EXTENDED:
            struct isotp_fast_addr result = isotp_fast_get_tx_addr_fixed(addr);
            result.ext_addr = addr->ext_addr;
            return result;
#endif /* CONFIG_ISOTP_FAST_FIXED_ADDRESSING */
#endif /* CONFIG_ISOTP_FAST_EXTENDED_ADDRESSING */
#ifdef CONFIG_ISOTP_FAST_CUSTOM_ADDRESSING
        case ISOTP_FAST_ADDRESSING_MODE_CUSTOM:
            return ctx->get_tx_addr_callback(addr);
#endif /* CONFIG_ISOTP_FAST_CUSTOM_ADDRESSING */
    }

    /*
     * This cannot be reached unless addressing_mode was set to an invalid value. As C cannot
     * guarantee that no invalid value is set to the enum, we still need to return something to
     * avoid a warning for -Wreturn-type.
     */
    return *addr;
}

static void receive_report_error(struct isotp_fast_recv_ctx *rctx, int8_t err)
{
    rctx->state = ISOTP_RX_STATE_ERR;
    rctx->error = err;
}

static void send_report_error(struct isotp_fast_send_ctx *sctx, int8_t err)
{
    sctx->state = ISOTP_TX_ERR;
    sctx->error = err;
}

static inline uint32_t receive_get_ff_length(uint8_t *data)
{
    uint32_t len;
    uint8_t pci = data[0];

    len = ((pci & ISOTP_PCI_FF_DL_UPPER_MASK) << 8) | data[1];

    /* Jumbo packet (32 bit length)*/
    /* TODO: this probably isn't supported at the moment, given that max length is 4095 */
    if (!len) {
        len = UNALIGNED_GET((uint32_t *)data);
        len = sys_be32_to_cpu(len);
    }

    return len;
}

static inline uint32_t receive_get_sf_length(uint8_t *data, int *index)
{
    uint8_t len = data[0] & ISOTP_PCI_SF_DL_MASK;
    (*index)++;

    /* Single frames > 16 bytes (CAN-FD only) */
    if (IS_ENABLED(CONFIG_CAN_FD_MODE) && !len) {
        len = data[1];
        (*index)++;
    }

    return len;
}

static void receive_can_tx(const struct device *dev, int error, void *arg)
{
    struct isotp_fast_recv_ctx *rctx = (struct isotp_fast_recv_ctx *)arg;

    ARG_UNUSED(dev);

    if (error != 0) {
        LOG_ERR("Error sending FC frame (%d)", error);
        receive_report_error(rctx, ISOTP_N_ERROR);
        k_work_submit(&rctx->work);
    }
}

static void receive_send_fc(struct isotp_fast_recv_ctx *rctx, uint8_t fs)
{
    struct isotp_fast_addr reply_addr = isotp_fast_get_reply_addr(rctx->ctx, &rctx->rx_addr);
    /* swap bus and address for FC frame */
    struct can_frame frame = {
        .flags =
            CAN_FRAME_IDE | ((rctx->ctx->opts->flags & ISOTP_MSG_FDF) != 0 ? CAN_FRAME_FDF : 0),
        .id = reply_addr.ext_id,
    };
    uint8_t *data = frame.data;
    uint8_t payload_len;
    int ret;

    __ASSERT_NO_MSG(!(fs & ISOTP_PCI_TYPE_MASK));

    *data++ = ISOTP_PCI_TYPE_FC | fs;
    *data++ = rctx->ctx->opts->bs;
    *data++ = rctx->ctx->opts->stmin;
    payload_len = data - frame.data;
    frame.dlc = can_bytes_to_dlc(payload_len);

    ret = can_send(rctx->ctx->can_dev, &frame, K_MSEC(ISOTP_A_TIMEOUT_MS), receive_can_tx, rctx);
    if (ret) {
        LOG_ERR("Can't send FC, (%d)", ret);
        receive_report_error(rctx, ISOTP_N_TIMEOUT_A);
        receive_state_machine(rctx);
    }
}

#ifdef CONFIG_ISOTP_FAST_BLOCKING_RECEIVE
static void notify_waiting_receiver(struct isotp_fast_recv_ctx *rctx)
{
    struct isotp_fast_recv_await_ctx *awaiter;
    SYS_SLIST_FOR_EACH_CONTAINER(&rctx->ctx->wait_recv_list, awaiter, node)
    {
        if ((awaiter->sender.id & awaiter->sender.mask)
            == (rctx->rx_addr.ext_id & awaiter->sender.mask))
        {
            LOG_DBG("Matched waiting receiver %x:%x to sender %x", awaiter->sender.id,
                    awaiter->sender.mask, rctx->rx_addr);
            awaiter->rctx = rctx;
            rctx->pending = true;
            if (k_sem_count_get(&awaiter->sem) == 0) {
                k_sem_give(&awaiter->sem);
            }
            else if (rctx->error) {
                /* if error state, we might already be waiting on the queue for the next
                   message, so purge the queue to unblock the waiter so it will see the error */
                k_msgq_purge(&rctx->recv_queue);
            }
            return;
        }
    }

    LOG_DBG("No matching receiver for sender %x", rctx->rx_addr);
}
#endif

static void receive_state_machine(struct isotp_fast_recv_ctx *rctx)
{
#ifdef CONFIG_ISOTP_FAST_PER_FRAME_DISPATCH
    struct net_buf *frag;
    while (k_msgq_get(&rctx->recv_queue, &frag, K_NO_WAIT) == 0) {
        int *p_rem_len = net_buf_user_data(frag);
        LOG_DBG("Remaining length %d (%d), enqueued %d", *p_rem_len, rctx->rem_len,
                k_msgq_num_used_get(&rctx->recv_queue));
        rctx->ctx->recv_callback(frag, *p_rem_len, rctx->rx_addr, rctx->ctx->recv_cb_arg);
        net_buf_unref(frag);
    }
#endif

    switch (rctx->state) {
        case ISOTP_RX_STATE_PROCESS_SF:
            LOG_DBG("SM process SF of length %d", rctx->rem_len);
            rctx->rem_len = 0;
            rctx->state = ISOTP_RX_STATE_RECYCLE;
#ifdef CONFIG_ISOTP_FAST_BLOCKING_RECEIVE
            notify_waiting_receiver(rctx);
#endif
            receive_state_machine(rctx);
            break;

        case ISOTP_RX_STATE_PROCESS_FF:
            LOG_DBG("SM process FF. Length: %d", rctx->rem_len + rctx->frag->len);
            if (rctx->ctx->opts->bs == 0
                && rctx->rem_len > CONFIG_ISOTP_FAST_RX_MAX_PACKET_COUNT * (CAN_MAX_DLEN - 1))
            {
                LOG_ERR("Pkt length is %d but buffer has only %d bytes", rctx->rem_len,
                        CONFIG_ISOTP_FAST_RX_MAX_PACKET_COUNT * (CAN_MAX_DLEN - 1));
                receive_report_error(rctx, ISOTP_N_BUFFER_OVERFLW);
                receive_state_machine(rctx);
                break;
            }
#ifdef CONFIG_ISOTP_FAST_BLOCKING_RECEIVE
            notify_waiting_receiver(rctx);
#endif

            if (rctx->ctx->opts->bs) {
                rctx->bs = rctx->ctx->opts->bs;
            }

            rctx->wft = ISOTP_WFT_FIRST;
            rctx->state = ISOTP_RX_STATE_TRY_ALLOC;
            __fallthrough;
        case ISOTP_RX_STATE_TRY_ALLOC:
            LOG_DBG("SM try to allocate");
            k_timer_stop(&rctx->timer);

#ifdef CONFIG_ISOTP_FAST_BLOCKING_RECEIVE
            notify_waiting_receiver(rctx);
#endif

            rctx->state = ISOTP_RX_STATE_SEND_FC;
            __fallthrough;
        case ISOTP_RX_STATE_SEND_FC:
            LOG_DBG("SM send CTS FC frame");
            receive_send_fc(rctx, ISOTP_PCI_FS_CTS);
            k_timer_start(&rctx->timer, K_MSEC(ISOTP_CR_TIMEOUT_MS), K_NO_WAIT);
            rctx->state = ISOTP_RX_STATE_WAIT_CF;
            break;

        case ISOTP_RX_STATE_SEND_WAIT:
            if (++rctx->wft < CONFIG_ISOTP_WFTMAX) {
                LOG_DBG("Send wait frame number %d", rctx->wft);
                receive_send_fc(rctx, ISOTP_PCI_FS_WAIT);
                k_timer_start(&rctx->timer, K_MSEC(ISOTP_ALLOC_TIMEOUT_MS), K_NO_WAIT);
                rctx->state = ISOTP_RX_STATE_TRY_ALLOC;
                break;
            }

            LOG_ERR("Sent %d wait frames. Giving up to alloc now", rctx->wft);
            receive_report_error(rctx, ISOTP_N_BUFFER_OVERFLW);
            __fallthrough;
        case ISOTP_RX_STATE_ERR:
            // LOG_DBG("SM ERR state. err nr: %d", ctx->error_nr);
            k_timer_stop(&rctx->timer);
            if (rctx->ctx->recv_error_callback) {
                rctx->ctx->recv_error_callback(rctx->error, rctx->rx_addr, rctx->ctx->recv_cb_arg);
            }
#ifdef CONFIG_ISOTP_FAST_BLOCKING_RECEIVE
            notify_waiting_receiver(rctx);
#endif
            if (rctx->error == ISOTP_N_BUFFER_OVERFLW) {
                receive_send_fc(rctx, ISOTP_PCI_FS_OVFLW);
            }

            // net_buf_unref(ctx->buffer);
            // ctx->buffer = NULL;
            // ctx->state = ISOTP_RX_STATE_RECYCLE;
            rctx->state = ISOTP_RX_STATE_UNBOUND;
            free_recv_ctx_if_unowned(rctx);
            break;
        case ISOTP_RX_STATE_RECYCLE:
#ifndef ISOTP_FAST_RECEIVE_QUEUE
            LOG_DBG("Message complete; dispatching");
            rctx->ctx->recv_callback(rctx->buffer, 0, rctx->rx_addr, rctx->ctx->recv_cb_arg);
#endif
#ifdef CONFIG_ISOTP_FAST_BLOCKING_RECEIVE
            notify_waiting_receiver(rctx);
#endif
            rctx->state = ISOTP_RX_STATE_UNBOUND;
            free_recv_ctx_if_unowned(rctx);
            break;
        case ISOTP_RX_STATE_UNBOUND:
            break;

        default:
            break;
    }
}

static void process_ff_sf(struct isotp_fast_recv_ctx *rctx, struct can_frame *frame)
{
    int index = 0;
    uint8_t payload_len;

    switch (frame->data[index] & ISOTP_PCI_TYPE_MASK) {
        case ISOTP_PCI_TYPE_FF:
            LOG_DBG("Got FF IRQ");
            if (frame->dlc != ISOTP_FF_DL_MIN) {
                LOG_DBG("FF DLC invalid. Ignore");
                return;
            }

            rctx->rem_len = receive_get_ff_length(frame->data);
            rctx->state = ISOTP_RX_STATE_PROCESS_FF;
            rctx->sn_expected = 1;
            index += 2;
            payload_len = CAN_MAX_DLEN - index;
            LOG_DBG("FF total length %d, FF len %d", rctx->rem_len, payload_len);
            break;

        case ISOTP_PCI_TYPE_SF:
            LOG_DBG("Got SF IRQ");
            rctx->rem_len = receive_get_sf_length(frame->data, &index);
            payload_len = MIN(rctx->rem_len, CAN_MAX_DLEN - index);
            LOG_DBG("SF length %d", payload_len);
            if (payload_len > can_dlc_to_bytes(frame->dlc)) {
                LOG_DBG("SF DL does not fit. Ignore");
                return;
            }

            rctx->state = ISOTP_RX_STATE_PROCESS_SF;
            break;

        default:
            LOG_DBG("Got unexpected frame. Ignore");
            receive_report_error(rctx, ISOTP_N_UNEXP_PDU);
            k_work_submit(&rctx->work);
            return;
    }

    LOG_DBG("Current buffer size %d; adding %d", rctx->buffer->len, payload_len);
    net_buf_add_mem(rctx->frag, &frame->data[index], payload_len);
    rctx->rem_len -= payload_len;
#ifdef ISOTP_FAST_RECEIVE_QUEUE
    int *p_rem_len = net_buf_user_data(rctx->frag);
    *p_rem_len = rctx->rem_len;
    k_msgq_put(&rctx->recv_queue, &rctx->frag, K_NO_WAIT);
    LOG_DBG("Enqueued item; remaining length %d, queue size %d", *p_rem_len,
            k_msgq_num_used_get(&rctx->recv_queue));
#endif
}

static void process_cf(struct isotp_fast_recv_ctx *rctx, struct can_frame *frame)
{
    int index = 0;
    uint32_t data_len;

    if ((frame->data[index] & ISOTP_PCI_TYPE_MASK) != ISOTP_PCI_TYPE_CF) {
        LOG_DBG("Waiting for CF but got something else (%d)",
                frame->data[index] >> ISOTP_PCI_TYPE_POS);
        LOG_DBG("%x %x %x %x %x %x %x %x ...", frame->data[0], frame->data[1], frame->data[2], frame->data[3], frame->data[4], frame->data[5], frame->data[6], frame->data[7]);
        receive_report_error(rctx, ISOTP_N_UNEXP_PDU);
        k_work_submit(&rctx->work);
        return;
    }

    k_timer_start(&rctx->timer, K_MSEC(ISOTP_CR_TIMEOUT_MS), K_NO_WAIT);

    if ((frame->data[index++] & ISOTP_PCI_SN_MASK) != rctx->sn_expected++) {
        LOG_ERR("Sequence number mismatch");
        receive_report_error(rctx, ISOTP_N_WRONG_SN);
        k_work_submit(&rctx->work);
        return;
    }

    LOG_DBG("Got CF irq. Appending data");
    data_len = MIN(rctx->rem_len, can_dlc_to_bytes(frame->dlc) - index);
    net_buf_add_mem(rctx->frag, &frame->data[index], data_len);
    rctx->rem_len -= data_len;
#ifdef ISOTP_FAST_RECEIVE_QUEUE
    int *p_rem_len = net_buf_user_data(rctx->frag);
    *p_rem_len = rctx->rem_len;
    k_msgq_put(&rctx->recv_queue, &rctx->frag, K_NO_WAIT); /* what if this fails? */
    LOG_DBG("Enqueued item; remaining length %d, queue size %d", *p_rem_len,
            k_msgq_num_used_get(&rctx->recv_queue));
#endif
    LOG_DBG("Added %d bytes; %d bytes remaining", data_len, rctx->rem_len);

    if (rctx->rem_len == 0) {
        rctx->state = ISOTP_RX_STATE_RECYCLE;
        k_work_submit(&rctx->work); // to dispatch complete message
        return;
    }

    if (rctx->ctx->opts->bs && !--rctx->bs) {
        LOG_DBG("Block is complete. Allocate new buffer");
        rctx->bs = rctx->ctx->opts->bs;
        // rctx->ctx->recv_callback(rctx->buffer, rctx->rem_len, rctx->rx_addr,
        // rctx->ctx->recv_cb_arg);
        rctx->state = ISOTP_RX_STATE_TRY_ALLOC;
    }
}

static void receive_work_handler(struct k_work *work)
{
    struct isotp_fast_recv_ctx *rctx = CONTAINER_OF(work, struct isotp_fast_recv_ctx, work);

    receive_state_machine(rctx);
}

static void receive_timeout_handler(struct k_timer *timer)
{
    struct isotp_fast_recv_ctx *rctx = CONTAINER_OF(timer, struct isotp_fast_recv_ctx, timer);

    switch (rctx->state) {
        case ISOTP_RX_STATE_WAIT_CF:
            LOG_ERR("Timeout while waiting for CF");
            receive_report_error(rctx, ISOTP_N_TIMEOUT_CR);
            break;

        case ISOTP_RX_STATE_TRY_ALLOC:
            rctx->state = ISOTP_RX_STATE_SEND_WAIT;
            break;

        default:
            break;
    }

    k_work_submit(&rctx->work);
}

static void receive_can_rx(struct isotp_fast_ctx *ctx, struct isotp_fast_addr sender_addr, struct can_frame *frame)
{
    size_t index = 0;
    bool create_context = (frame->data[index] & ISOTP_PCI_TYPE_MASK) == ISOTP_PCI_TYPE_FF ||
        (frame->data[index] & ISOTP_PCI_TYPE_MASK) == ISOTP_PCI_TYPE_SF;
    struct isotp_fast_recv_ctx *rctx;
    int result = get_recv_ctx(ctx, sender_addr, create_context, &rctx);
    if (result == -ESRCH)
    {
        LOG_ERR("No existing context for continuing frame");
        LOG_DBG("%x %x %x %x %x %x %x %x ...", frame->data[0], frame->data[1], frame->data[2], frame->data[3], frame->data[4], frame->data[5], frame->data[6], frame->data[7]);
        return;
    }
    else if (result != 0)
    {
        LOG_ERR("RX buffer full");
        return;
    }

    switch (rctx->state) {
        case ISOTP_RX_STATE_WAIT_FF_SF:
            process_ff_sf(rctx, frame);
            break;

        case ISOTP_RX_STATE_WAIT_CF:
            process_cf(rctx, frame);
            /* still waiting for more CF */
            if (rctx->state == ISOTP_RX_STATE_WAIT_CF) {
                return;
            }
            break;

        default:
            LOG_DBG("Got a frame in a state where it is unexpected.");
    }

    k_work_submit(&rctx->work);
}

static inline void prepare_frame(struct can_frame *frame, struct isotp_fast_ctx *ctx,
                                 struct isotp_fast_addr can_id, int *index)
{
    frame->id = can_id.ext_id;
    frame->flags = CAN_FRAME_IDE | ((ctx->opts->flags & ISOTP_MSG_FDF) != 0 ? CAN_FRAME_FDF : 0);
#ifdef CONFIG_ISOTP_FAST_EXTENDED_ADDRESSING
    frame->data[index++] = can_id.ext_addr;
#endif
}

static k_timeout_t stmin_to_timeout(uint8_t stmin)
{
    /* According to ISO 15765-2 stmin should be 127ms if value is corrupt */
    if (stmin > ISOTP_STMIN_MAX || (stmin > ISOTP_STMIN_MS_MAX && stmin < ISOTP_STMIN_US_BEGIN)) {
        return K_MSEC(ISOTP_STMIN_MS_MAX);
    }

    if (stmin >= ISOTP_STMIN_US_BEGIN) {
        return K_USEC((stmin + 1 - ISOTP_STMIN_US_BEGIN) * 100U);
    }

    return K_MSEC(stmin);
}

static void send_process_fc(struct isotp_fast_send_ctx *sctx, struct can_frame *frame)
{
    uint8_t *data = frame->data;

    if ((*data & ISOTP_PCI_TYPE_MASK) != ISOTP_PCI_TYPE_FC) {
        LOG_ERR("Got unexpected PDU expected FC from %x", frame->id);
        send_report_error(sctx, ISOTP_N_UNEXP_PDU);
        return;
    }

    switch (*data++ & ISOTP_PCI_FS_MASK) {
        case ISOTP_PCI_FS_CTS:
            sctx->state = ISOTP_TX_SEND_CF;
            sctx->wft = 0;
            sctx->backlog = 0;
            k_sem_reset(&sctx->sem);
            sctx->bs = *data++;
            sctx->stmin = *data++;
            LOG_DBG("Got CTS. BS: %d, STmin: %d", sctx->bs, sctx->stmin);
            break;

        case ISOTP_PCI_FS_WAIT:
            LOG_DBG("Got WAIT frame");
            k_timer_start(&sctx->timer, K_MSEC(ISOTP_BS_TIMEOUT_MS), K_NO_WAIT);
            if (sctx->wft >= CONFIG_ISOTP_WFTMAX) {
                LOG_WRN("Got too many wait frames");
                send_report_error(sctx, ISOTP_N_WFT_OVRN);
            }

            sctx->wft++;
            break;

        case ISOTP_PCI_FS_OVFLW:
            LOG_ERR("Got overflow FC frame from %x", frame->id);
            send_report_error(sctx, ISOTP_N_BUFFER_OVERFLW);
            break;

        default:
            send_report_error(sctx, ISOTP_N_INVALID_FS);
    }
}

static void send_can_rx(struct isotp_fast_send_ctx *sctx, struct can_frame *frame)
{
    if (sctx->state == ISOTP_TX_WAIT_FC) {
        k_timer_stop(&sctx->timer);
        send_process_fc(sctx, frame);
    }
    else {
        LOG_ERR("Got unexpected PDU from %x (current state: %d)", frame->id, sctx->state);
        send_report_error(sctx, ISOTP_N_UNEXP_PDU);
    }

    k_work_submit(&sctx->work);
}

static void can_rx_callback(const struct device *dev, struct can_frame *frame, void *arg)
{
    struct isotp_fast_ctx *ctx = arg;
    int index = 0;
    struct isotp_fast_addr sender_addr = {
        .ext_id = frame->id,
#ifdef CONFIG_ISOTP_FAST_EXTENDED_ADDRESSING
        .ext_addr = frame->data[index++],
#endif
    };
    struct isotp_fast_addr reply_addr = isotp_fast_get_reply_addr(ctx, &sender_addr);

    if ((frame->data[index++] & ISOTP_PCI_TYPE_MASK) == ISOTP_PCI_TYPE_FC) {
        LOG_DBG("Got flow control frame from %x", frame->id);
        /* inbound flow control for a message we are currently transmitting */
        struct isotp_fast_send_ctx *sctx;
        if (get_send_ctx(ctx, reply_addr, false, &sctx) != 0) {
            LOG_DBG("Ignoring flow control frame from %x", frame->id);
            return;
        }
        send_can_rx(sctx, frame);
    }
    else {
        receive_can_rx(ctx, sender_addr, frame);
    }
}

static void send_can_tx_callback(const struct device *dev, int error, void *arg)
{
    struct isotp_fast_send_ctx *sctx = arg;

    ARG_UNUSED(dev);

    sctx->backlog--;
    k_sem_give(&sctx->sem);

    if (sctx->state == ISOTP_TX_WAIT_BACKLOG) {
        if (sctx->backlog > 0) {
            return;
        }

        sctx->state = ISOTP_TX_WAIT_FIN;
    }

    k_work_submit(&sctx->work);
}

static inline int send_ff(struct isotp_fast_send_ctx *sctx)
{
    struct can_frame frame;
    int index = 0;
    int ret;
    uint16_t len = sctx->rem_len;

    prepare_frame(&frame, sctx->ctx, sctx->tx_addr, &index);

    if (len > 0xFFF) {
        frame.data[index++] = ISOTP_PCI_TYPE_FF;
        frame.data[index++] = 0;
        frame.data[index++] = (len >> 3 * 8) & 0xFF;
        frame.data[index++] = (len >> 2 * 8) & 0xFF;
        frame.data[index++] = (len >> 8) & 0xFF;
        frame.data[index++] = len & 0xFF;
    }
    else {
        frame.data[index++] = ISOTP_PCI_TYPE_FF | (len >> 8);
        frame.data[index++] = len & 0xFF;
    }

    /* According to ISO FF has sn 0 and is incremented to one
     * although it's not part of the FF frame
     */
    sctx->sn = 1;
    uint16_t size = MIN(CAN_MAX_DLEN - index, len);
    memcpy(&frame.data[index], sctx->data, size);
    sctx->rem_len -= size;
    sctx->data += size;
    frame.dlc = can_bytes_to_dlc(CAN_MAX_DLEN);
    ret = can_send(sctx->ctx->can_dev, &frame, K_MSEC(ISOTP_A_TIMEOUT_MS), send_can_tx_callback,
                   sctx);
    return ret;
}

static inline int send_cf(struct isotp_fast_send_ctx *sctx)
{
    struct can_frame frame;
    int index = 0;
    int ret;
    uint16_t len;

    prepare_frame(&frame, sctx->ctx, sctx->tx_addr, &index);

    /*sn wraps around at 0xF automatically because it has a 4 bit size*/
    frame.data[index++] = ISOTP_PCI_TYPE_CF | sctx->sn;

    len = MIN(sctx->rem_len, CAN_MAX_DLEN - index);
    memcpy(&frame.data[index], sctx->data, len);
    sctx->rem_len -= len;
    sctx->data += len;

    frame.dlc = can_bytes_to_dlc(len + index);
    ret = can_send(sctx->ctx->can_dev, &frame, K_MSEC(ISOTP_A_TIMEOUT_MS), send_can_tx_callback,
                   sctx);
    if (ret == 0) {
        sctx->sn++;
        sctx->bs--;
        sctx->backlog++;
    }

    ret = ret ? ret : sctx->rem_len;
    return ret;
}

static void send_state_machine(struct isotp_fast_send_ctx *sctx)
{
    int ret;
    switch (sctx->state) {
        case ISOTP_TX_SEND_FF:
            send_ff(sctx);
            k_timer_start(&sctx->timer, K_MSEC(ISOTP_BS_TIMEOUT_MS), K_NO_WAIT);
            sctx->state = ISOTP_TX_WAIT_FC;
            break;

        case ISOTP_TX_SEND_CF:
            k_timer_stop(&sctx->timer);
            do {
                ret = send_cf(sctx);
                if (!ret) {
                    sctx->state = ISOTP_TX_WAIT_BACKLOG;
                    break;
                }

                if (ret < 0) {
                    LOG_ERR("Failed to send CF");
                    send_report_error(sctx, ret == -EAGAIN ? ISOTP_N_TIMEOUT_A : ISOTP_N_ERROR);
                    break;
                }

                if (sctx->ctx->opts->bs && !sctx->bs) {
                    k_timer_start(&sctx->timer, K_MSEC(ISOTP_BS_TIMEOUT_MS), K_NO_WAIT);
                    sctx->state = ISOTP_TX_WAIT_FC;
                    LOG_DBG("BS reached. Wait for FC again");
                    break;
                }
                else if (sctx->stmin) {
                    sctx->state = ISOTP_TX_WAIT_ST;
                    break;
                }

                /* Ensure FIFO style transmission of CF */
                k_sem_take(&sctx->sem, K_FOREVER);
            } while (ret > 0);
            break;

        case ISOTP_TX_WAIT_ST:
            k_timer_start(&sctx->timer, stmin_to_timeout(sctx->stmin), K_NO_WAIT);
            sctx->state = ISOTP_TX_SEND_CF;
            LOG_DBG("SM wait ST");
            break;

        case ISOTP_TX_ERR:
            sctx->ctx->sent_callback(sctx->error, sctx->tx_addr, sctx->cb_arg);
            LOG_ERR("Aborting TX to %x due to error %d", sctx->tx_addr.ext_id, sctx->error);
            sctx->state = ISOTP_TX_STATE_RESET;
            free_send_ctx(sctx);
            break;

            /*
             * We sent this synchronously in isotp_fast_send.
             * case ISOTP_TX_SEND_SF:
             *   __fallthrough;
             * */

        case ISOTP_TX_WAIT_FIN:
            LOG_DBG("SM finish");
            k_timer_stop(&sctx->timer);

            sctx->ctx->sent_callback(ISOTP_N_OK, sctx->tx_addr, sctx->cb_arg);
            sctx->state = ISOTP_TX_STATE_RESET;
            free_send_ctx(sctx);
            break;

        default:
            break;
    }
}

static void send_work_handler(struct k_work *work)
{
    struct isotp_fast_send_ctx *sctx = CONTAINER_OF(work, struct isotp_fast_send_ctx, work);

    send_state_machine(sctx);
}

static void send_timeout_handler(struct k_timer *timer)
{
    struct isotp_fast_send_ctx *sctx = CONTAINER_OF(timer, struct isotp_fast_send_ctx, timer);

    if (sctx->state != ISOTP_TX_SEND_CF) {
        LOG_ERR("Timed out waiting for FC frame");
        send_report_error(sctx, ISOTP_N_TIMEOUT_BS);
    }

    k_work_submit(&sctx->work);
}

static inline void prepare_filter(struct can_filter *filter, uint32_t rx_addr,
                                  const struct isotp_fast_opts *opts)
{
    filter->id = rx_addr;
    uint32_t mask = 0;
#ifdef CONFIG_ISOTP_FAST_CUSTOM_ADDRESSING
    mask = CONFIG_ISOTP_FAST_CUSTOM_ADDRESSING_RX_MASK;
#else
    switch (opts->addressing_mode) {
#ifdef CONFIG_ISOTP_FAST_NORMAL_ADDRESSING
        case ISOTP_FAST_ADDRESSING_MODE_NORMAL:
#endif
#ifdef CONFIG_ISOTP_FAST_EXTENDED_ADDRESSING
        case ISOTP_FAST_ADDRESSING_MODE_EXTENDED:
#endif
#if defined(CONFIG_ISOTP_FAST_NORMAL_ADDRESSING) || defined(CONFIG_ISOTP_FAST_EXTENDED_ADDRESSING)
            mask = CAN_EXT_ID_MASK;
            break;
#endif
#ifdef CONFIG_ISOTP_FAST_FIXED_ADDRESSING
        case ISOTP_FAST_ADDRESSING_MODE_FIXED:
#ifdef CONFIG_ISOTP_FAST_EXTENDED_ADDRESSING
        case ISOTP_FAST_ADDRESSING_MODE_FIXED | ISOTP_FAST_ADDRESSING_MODE_EXTENDED:
#endif /* CONFIG_ISOTP_FAST_EXTENDED_ADDRESSING */
#endif /* CONFIG_ISOTP_FAST_FIXED_ADDRESSING */
#if defined(CONFIG_ISOTP_FAST_FIXED_ADDRESSING) || defined(CONFIG_ISOTP_FAST_EXTENDED_ADDRESSING)
            mask = ISOTP_FIXED_ADDR_RX_MASK;
            break;
#endif
    }
#endif /* CONFIG_ISOTP_FAST_CUSTOM_ADDRESSING */
    filter->mask = mask;
    filter->flags = CAN_FILTER_IDE;
}

int isotp_fast_bind(struct isotp_fast_ctx *ctx, const struct device *can_dev,
                    const struct isotp_fast_addr rx_addr, const struct isotp_fast_opts *opts,
                    isotp_fast_recv_callback_t recv_callback, void *recv_cb_arg,
                    isotp_fast_recv_error_callback_t recv_error_callback,
                    isotp_fast_send_callback_t sent_callback)
{
    sys_slist_init(&ctx->isotp_send_ctx_list);
    sys_slist_init(&ctx->isotp_recv_ctx_list);
#ifdef CONFIG_ISOTP_FAST_BLOCKING_RECEIVE
    sys_slist_init(&ctx->wait_recv_list);
#endif

    ctx->can_dev = can_dev;
    ctx->opts = opts;
    ctx->recv_callback = recv_callback;
    ctx->recv_cb_arg = recv_cb_arg;
    ctx->recv_error_callback = recv_error_callback;
    ctx->sent_callback = sent_callback;
    ctx->rx_addr = rx_addr;

    struct can_filter filter;
    prepare_filter(&filter, rx_addr.ext_id, opts);
    ctx->filter_id = can_add_rx_filter(ctx->can_dev, can_rx_callback, ctx, &filter);

    LOG_INF("Successfully bound to %x:%x", filter.id, filter.mask);

    return ISOTP_N_OK;
}

#ifdef CONFIG_ISOTP_FAST_BLOCKING_RECEIVE
static void free_recv_await_ctx(struct isotp_fast_ctx *ctx, struct isotp_fast_recv_await_ctx *actx)
{
    sys_slist_find_and_remove(&ctx->wait_recv_list, &actx->node);
    if (actx->rctx) {
        free_recv_ctx(actx->rctx);
    }
    k_mem_slab_free(&isotp_recv_await_ctx_slab, actx);
}
#endif

int isotp_fast_unbind(struct isotp_fast_ctx *ctx)
{
    if (ctx->filter_id >= 0 && ctx->can_dev) {
        can_remove_rx_filter(ctx->can_dev, ctx->filter_id);
    }

#ifdef CONFIG_ISOTP_FAST_BLOCKING_RECEIVE
    struct isotp_fast_recv_await_ctx *actx;
    struct isotp_fast_recv_await_ctx *next;
    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&ctx->wait_recv_list, actx, next, node)
    {
        free_recv_await_ctx(ctx, actx);
    }
#endif
    return ISOTP_N_OK;
}

#ifdef CONFIG_ISOTP_FAST_BLOCKING_RECEIVE
int isotp_fast_recv(struct isotp_fast_ctx *ctx, struct can_filter sender, uint8_t *buf, size_t size,
                    k_timeout_t timeout)
{
    /* first try to find in-flight context */
    bool found = false;
    struct isotp_fast_recv_await_ctx *actx;
    SYS_SLIST_FOR_EACH_CONTAINER(&ctx->wait_recv_list, actx, node)
    {
        if (actx->sender.id == sender.id && actx->sender.mask == sender.mask) {
            found = true;
            break;
        }
    }

    int ret;
    if (!found) {
        /* create a new context */
        LOG_DBG("Creating new await context matching sender %x:%x", sender.id, sender.mask);
        int err = k_mem_slab_alloc(&isotp_recv_await_ctx_slab, (void **)&actx, K_NO_WAIT);
        if (err != 0) {
            return ISOTP_NO_CTX_LEFT;
        }

        actx->sender = sender;
        k_sem_init(&actx->sem, 0, 1);
        sys_slist_append(&ctx->wait_recv_list, &actx->node);

        /* try to find matching receive context in case there is already one pending */
        struct isotp_fast_recv_ctx *rctx;
        bool wait = true;
        SYS_SLIST_FOR_EACH_CONTAINER(&ctx->isotp_recv_ctx_list, rctx, node)
        {
            // TODO: handle extended addressing
            if ((sender.id & sender.mask) == (rctx->rx_addr.ext_id & sender.mask) && !rctx->pending)
            {
                LOG_DBG("Matched await context %x:%x to sender %x", sender.id, sender.mask,
                        rctx->rx_addr.ext_id);
                actx->rctx = rctx;
                rctx->pending = true;
                wait = false;
                break;
            }
        }

        if (wait) {
            /* completely new, so wait for something to happen */
            LOG_DBG("Waiting for message matching %x:%x", sender.id, sender.mask);
            ret = k_sem_take(&actx->sem, timeout);
            if (ret == -EAGAIN) {
                free_recv_await_ctx(ctx, actx);
                LOG_DBG("Timed out waiting for first message");
                return ISOTP_RECV_TIMEOUT;
            }
            LOG_DBG("Matched; processing message");
        }
    }

    if (actx->rctx->error != 0) {
        LOG_DBG("Error %d occurred", actx->rctx->error);
        ret = actx->rctx->error;
        free_recv_await_ctx(ctx, actx);
        return ret;
    }

    struct net_buf *frag;
    int pos = 0;
    int rem_len = 0;
    while ((ret = k_msgq_get(&actx->rctx->recv_queue, &frag, timeout)) == 0) {
        if (actx->rctx->error != 0) {
            LOG_DBG("Error %d occurred", actx->rctx->error);
            ret = actx->rctx->error;
            free_recv_await_ctx(ctx, actx);
            return ret;
        }
        if (pos == 0) {
            LOG_DBG("New messages received");
        }
        rem_len = *(int *)net_buf_user_data(frag);
        LOG_DBG("Remaining length %d, enqueued %d", rem_len,
                k_msgq_num_used_get(&actx->rctx->recv_queue));
        int len = MIN(frag->len, size - pos);
        memcpy(buf, frag->data, len);
        net_buf_unref(frag);
        pos += len;
        buf += len;
        if (size - pos < (CAN_MAX_DLEN - 1) && rem_len > (CAN_MAX_DLEN - 1)) {
            /* user recv buffer full */
            LOG_DBG("Buffer full; returning");
            break;
        }
        if (rem_len == 0) {
            break;
        }
    }
    actx->rctx->pending = false;
    if (rem_len == 0) {
        free_recv_await_ctx(ctx, actx);
    }
    if (ret == -EAGAIN) {
        free_recv_await_ctx(ctx, actx);
        LOG_DBG("Timed out waiting on more packets");
        return ISOTP_RECV_TIMEOUT;
    }
    return pos;
}
#endif /* CONFIG_ISOTP_FAST_BLOCKING_RECEIVE */

static void isotp_fast_sent_single_cb(const struct device *dev, int error, void *arg)
{
    struct isotp_fast_send_ctx *ctx = arg;
    ctx->ctx->sent_callback(error, ctx->tx_addr, ctx->cb_arg);
    free_send_ctx(ctx);
}

int isotp_fast_send(struct isotp_fast_ctx *ctx, const uint8_t *data, size_t len,
                    const struct isotp_fast_addr target_addr, void *cb_arg)
{
    if (len <= (CAN_MAX_DLEN - ISOTP_FAST_SF_LEN_BYTE)) {
        struct can_frame frame;
        int index = 0;
        prepare_frame(&frame, ctx, target_addr, &index);
#ifdef CONFIG_CAN_FD_MODE
        if (len > ISOTP_4BIT_SF_MAX_CAN_DL - 1) {
            frame.data[index++] = ISOTP_PCI_TYPE_SF;
            frame.data[index++] = (uint8_t)len;
        }
        else {
            frame.data[index++] = ISOTP_PCI_TYPE_SF | (uint8_t)len;
        }
#else
        frame.data[index++] = (uint8_t)len;
#endif
        frame.dlc = can_bytes_to_dlc(len + index);
        memcpy(&frame.data[index], data, len);
        struct isotp_fast_send_ctx *context;
        int ret = get_send_ctx(ctx, target_addr, true, &context);
        if (ret) {
            return ISOTP_NO_NET_BUF_LEFT;
        }
        context->cb_arg = cb_arg;
        ret = can_send(ctx->can_dev, &frame, K_MSEC(ISOTP_A_TIMEOUT_MS), isotp_fast_sent_single_cb, context);
        return ret;
    }
    else {
        if (len > ISOTP_FAST_MAX_LEN) {
            return ISOTP_N_BUFFER_OVERFLW;
        }
        struct isotp_fast_send_ctx *context;
        int ret = get_send_ctx(ctx, target_addr, true, &context);
        if (ret) {
            return ISOTP_NO_NET_BUF_LEFT;
        }
        context->data = data;
        context->rem_len = len;
        context->cb_arg = cb_arg;

        k_work_submit(&context->work);
    }
    return ISOTP_N_OK;
}

#ifdef CONFIG_ISOTP_FAST_FIXED_ADDRESSING
int isotp_fast_send_fixed(struct isotp_fast_ctx *ctx, const uint8_t *data, size_t len,
                          const uint8_t target_addr, void *cb_arg)
{
    struct isotp_fast_addr rx_addr = ctx->rx_addr;
    rx_addr.ext_id &= target_addr;
    struct isotp_fast_addr tx_addr = isotp_fast_get_tx_addr_fixed(&rx_addr);
    return isotp_fast_send(ctx, data, len, tx_addr, cb_arg);
}

struct isotp_fast_addr isotp_fast_get_tx_addr_fixed(const struct isotp_fast_addr *addr)
{
    /* swap source address (SA) and target address (TA) */
    uint8_t sa_new = (addr->ext_id & ISOTP_FIXED_ADDR_TA_MASK) >> ISOTP_FIXED_ADDR_TA_POS;
    uint8_t ta_new = (addr->ext_id & ISOTP_FIXED_ADDR_SA_MASK) >> ISOTP_FIXED_ADDR_SA_POS;

    return (struct isotp_fast_addr){
        .ext_id = (addr->ext_id & ISOTP_FIXED_ADDR_PRIO_MASK) | (addr->ext_id & 0xFF0000)
                  | (sa_new << ISOTP_FIXED_ADDR_SA_POS) | (ta_new << ISOTP_FIXED_ADDR_TA_POS),
    };
}
#endif

/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/zephyr/ThingSetZephyrCanServerTransport.hpp"
#include "thingset++/can/zephyr/CanFrame.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include "thingset++/internal/logging.hpp"

namespace ThingSet::Can::Zephyr {

static const isotp_fast_opts flowControlOptions = {
    .bs = 8, /* block size */
    .stmin = CONFIG_THINGSET_CAN_FRAME_SEPARATION_TIME,
#ifdef CONFIG_CAN_FD_MODE
    .flags = ISOTP_MSG_FDF,
#endif
    .addressing_mode = ISOTP_FAST_ADDRESSING_MODE_FIXED,
};

ThingSetZephyrCanServerTransport::ThingSetZephyrCanServerTransport(_ThingSetZephyrCanInterface &canInterface, uint8_t *rxBuffer,
                                                       size_t rxBufferSize, uint8_t *txBuffer, size_t txBufferSize)
    : ThingSetCanServerTransport(), _canInterface(canInterface), _rxBuffer(rxBuffer),
      _rxBufferSize(rxBufferSize), _txBuffer(txBuffer), _txBufferSize(txBufferSize)
{
    k_sem_init(&_lock, 1, 1);
}

ThingSetZephyrCanServerTransport::~ThingSetZephyrCanServerTransport()
{
    isotp_fast_unbind(&_requestResponseContext);
}

ThingSetCanInterface &ThingSetZephyrCanServerTransport::getInterface()
{
    return _canInterface;
}

bool ThingSetZephyrCanServerTransport::doPublish(const Can::CanID &id, uint8_t *buffer, size_t length)
{
    CanFrame frame(id);
    memcpy(frame.getData(), buffer, length);
    frame.setLength(length);
    frame.setFd(true);
    int result =
        can_send(_canInterface.getDevice(), frame.getFrame(), K_MSEC(CONFIG_THINGSET_CAN_REPORT_SEND_TIMEOUT), nullptr, nullptr);
    return result == 0;
}

static void onRequestResponseError(int8_t error, isotp_fast_addr addr, void *arg)
{}

static void onRequestResponseSent(int result, isotp_fast_addr addr, void *arg)
{}

bool ThingSetZephyrCanServerTransport::listen(std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
    isotp_fast_addr rxAddress = {
        .ext_id = CanID()
                      .setMessageType(MessageType::requestResponse)
                      .setMessagePriority(MessagePriority::channel)
                      .setTarget(_canInterface.getNodeAddress()),
    };
#pragma GCC diagnostic pop
    _inboundRequestCallback = callback;
    return isotp_fast_bind(&_requestResponseContext, _canInterface.getDevice(), rxAddress, &flowControlOptions,
                           onRequestResponseReceived, this, onRequestResponseError, onRequestResponseSent)
           == 0;
}

void ThingSetZephyrCanServerTransport::onRequestResponseReceived(net_buf *buffer, int remainingLength,
                                                                 isotp_fast_addr address, void *arg)
{
    uint8_t errorResponse[] = { ThingSetStatusCode::internalServerError };
    ThingSetZephyrCanServerTransport *self = (ThingSetZephyrCanServerTransport *)arg;
    size_t len = net_buf_frags_len(buffer);
    int result = k_sem_take(&self->_lock, K_SECONDS(1));
    bool taken;
    uint8_t *txBuffer;
    if ((taken = (result == 0))) {
        net_buf_linearize(self->_rxBuffer, self->_rxBufferSize, buffer, 0, len);
        // if a response to a request we sent, dispatch accordingly
        if (self->_rxBuffer[0] >= ThingSetStatusCode::created && self->_rxBuffer[0] <= ThingSetStatusCode::notAGateway)
        {
        }
        else if (self->_rxBuffer[0] >= ThingSetRequestType::get && self->_rxBuffer[0] <= ThingSetRequestType::update) {
            // otherwise assume it is a new request inbound
            result = self->_inboundRequestCallback(CanID::create(address.ext_id), self->_rxBuffer, len, self->_txBuffer, self->_txBufferSize);
            if (result > 0) {
                txBuffer = self->_txBuffer;
                len = result;
            }
            else {
                txBuffer = errorResponse;
                len = 1;
            }
        }
        else {
            errorResponse[0] = ThingSetStatusCode::badRequest;
            txBuffer = errorResponse;
            len = 1;
        }
    }
    else {
        txBuffer = errorResponse;
        len = 1;
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
    result = isotp_fast_send(&self->_requestResponseContext, txBuffer, len,
                             isotp_fast_addr{ .ext_id = CanID::create(address.ext_id).getReplyId() }, nullptr);
#pragma GCC diagnostic pop
    if (result != 0) {
        LOG_ERR("Error %d sending reply to message from 0x%d", result, address.ext_id);
    }
    if (taken) {
        k_sem_give(&self->_lock);
    }
}

} // namespace ThingSet::Can::Zephyr
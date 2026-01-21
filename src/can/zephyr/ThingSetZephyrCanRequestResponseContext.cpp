/*
 * Copyright (c) 2025 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/zephyr/ThingSetZephyrCanRequestResponseContext.hpp"
#include "thingset++/can/zephyr/ThingSetZephyrCanInterface.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include "thingset++/internal/logging.hpp"

namespace ThingSet::Can::Zephyr {

struct IsoTpFastAddress : public isotp_fast_addr {
    IsoTpFastAddress(const CanID &id)
    {
        setId(id);
    }

    IsoTpFastAddress &setId(const CanID &id)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
        ext_id = id.getId();
#pragma GCC diagnostic pop
        return *this;
    }
};

const isotp_fast_opts ThingSetZephyrCanRequestResponseContext::flowControlOptions = {
    .bs = 8, /* block size */
    .stmin = CONFIG_THINGSET_PLUS_PLUS_CAN_FRAME_SEPARATION_TIME,
#ifdef CONFIG_CAN_FD_MODE
    .flags = ISOTP_MSG_FDF,
#endif
    .addressing_mode = ISOTP_FAST_ADDRESSING_MODE_FIXED,
};

static void onRequestResponseError(int8_t error, isotp_fast_addr addr, void *arg);
static void onRequestResponseSent(int result, isotp_fast_addr addr, void *arg);

ThingSetZephyrCanRequestResponseContext::~ThingSetZephyrCanRequestResponseContext()
{
    unbindIfNecessary();
}

ThingSetZephyrCanInterface &ThingSetZephyrCanRequestResponseContext::getInterface()
{
    return _canInterface;
}

bool ThingSetZephyrCanRequestResponseContext::bind(std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    return bind(CanID::broadcastAddress, callback);
}

void ThingSetZephyrCanRequestResponseContext::unbindIfNecessary()
{
    if (_requestResponseContext.filter_id > -1) {
        isotp_fast_unbind(&_requestResponseContext);
        _requestResponseContext.filter_id = 1;
    }
}

bool ThingSetZephyrCanRequestResponseContext::bind(uint8_t otherNodeAddress, std::function<int(const CanID &, uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    unbindIfNecessary();
    auto canId = CanID()
                      .setMessageType(MessageType::requestResponse)
                      .setMessagePriority(MessagePriority::channel)
                      .setTarget(_canInterface.getNodeAddress());
    if (otherNodeAddress != CanID::broadcastAddress) {
        canId.setSource(otherNodeAddress);
    }
    _inboundRequestCallback = callback;
    return isotp_fast_bind(&_requestResponseContext, _canInterface.getDevice(), IsoTpFastAddress(canId),
                           &ThingSetZephyrCanRequestResponseContext::flowControlOptions, onRequestResponseReceived,
                           this, onRequestResponseError, onRequestResponseSent) == 0;
}

bool ThingSetZephyrCanRequestResponseContext::send(const uint8_t otherNodeAddress, uint8_t *buffer, size_t len)
{
    IsoTpFastAddress targetAddress(CanID()
                      .setMessageType(MessageType::requestResponse)
                      .setMessagePriority(MessagePriority::channel)
                      .setSource(_canInterface.getNodeAddress())
                      .setTarget(otherNodeAddress));
    return isotp_fast_send(&_requestResponseContext, buffer, len, targetAddress, nullptr) == 0;
}

void ThingSetZephyrCanRequestResponseContext::onRequestResponseReceived(net_buf *buffer, int remainingLength,
                                                                        isotp_fast_addr address, void *arg)
{
    ThingSetZephyrCanRequestResponseContext *self = (ThingSetZephyrCanRequestResponseContext *)arg;
    self->onRequestResponseReceived(buffer, remainingLength, address);
}

void ThingSetZephyrCanRequestResponseContext::onRequestResponseReceived(net_buf *buffer, int remainingLength,
                                                                        isotp_fast_addr address)
{
    uint8_t errorResponse[] = { ThingSetStatusCode::internalServerError };
    size_t len = net_buf_frags_len(buffer);
    int result = k_sem_take(&_lock, K_SECONDS(1));
    bool taken;
    uint8_t *txBuffer;
    if ((taken = (result == 0))) {
        LOG_DEBUG("Linearising buffer of length %d into %p", len, _rxBuffer);
        len = net_buf_linearize(_rxBuffer, _rxBufferSize, buffer, 0, len);
        // if a response to a request we sent
        if ((_rxBuffer[0] >= ThingSetStatusCode::created && _rxBuffer[0] <= ThingSetStatusCode::notAGateway) ||
            // or if it is a new request inbound
            // TODO: support text mode (which is not a simple greater/less than)
            (_rxBuffer[0] >= ThingSetBinaryRequestType::get && _rxBuffer[0] <= ThingSetBinaryRequestType::update) ||
            _rxBuffer[0] == ThingSetBinaryRequestType::forward) {
            // regardless, dispatch accordingly
            result = _inboundRequestCallback(CanID::create(address.ext_id), _rxBuffer, len, _txBuffer,
                _txBufferSize);
            if (result >= 0) { // length should be zero if this is a reply
                txBuffer = _txBuffer;
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
    if (len > 0) {
        result = isotp_fast_send(&_requestResponseContext, txBuffer, len,
                                IsoTpFastAddress(CanID::create(address.ext_id).getReplyId()), nullptr);
    }
    if (result != 0) {
        LOG_ERROR("Error %d sending reply to message from 0x%x", result, address.ext_id);
    }
    if (taken) {
        k_sem_give(&_lock);
    }
}

static void onRequestResponseError(int8_t error, isotp_fast_addr addr, void *arg)
{}

static void onRequestResponseSent(int result, isotp_fast_addr addr, void *arg)
{}

} // namespace ThingSet::Can::Zephyr
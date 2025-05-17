/*
 * Copyright (c) 2024 Brill Power.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingset++/can/zephyr/ThingSetZephyrCanInterface.hpp"
#include "thingset++/Eui.hpp"
#include "thingset++/ThingSetStatus.hpp"
#include "thingset++/can/CanID.hpp"
#include "thingset++/can/zephyr/CanFrame.hpp"
#include "thingset++/internal/logging.hpp"
#include <chrono>
#include <string.h>
#include <zephyr/random/random.h>

namespace ThingSet::Can::Zephyr {

static const isotp_fast_opts flowControlOptions = {
    .bs = 8, /* block size */
    .stmin = CONFIG_THINGSET_CAN_FRAME_SEPARATION_TIME,
#ifdef CONFIG_CAN_FD_MODE
    .flags = ISOTP_MSG_FDF,
#endif
    .addressing_mode = ISOTP_FAST_ADDRESSING_MODE_FIXED,
};

_ThingSetZephyrCanInterface::_ThingSetZephyrCanInterface(const device *const canDevice) : _canDevice(canDevice)
{
    _nodeAddress = CanID::broadcastAddress;
}

const device *const _ThingSetZephyrCanInterface::getDevice()
{
    return _canDevice;
}

ThingSetZephyrCanStubInterface::ThingSetZephyrCanStubInterface(const device *const canDevice)
    : _ThingSetZephyrCanInterface::_ThingSetZephyrCanInterface(canDevice)
{}

bool ThingSetZephyrCanStubInterface::bind(uint8_t nodeAddress)
{
    _nodeAddress = nodeAddress;
    return true;
}

bool ThingSetZephyrCanStubInterface::listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    return true;
}

ThingSetZephyrCanInterface::ThingSetZephyrCanInterface(const device *const canDevice, uint8_t *rxBuffer,
                                                       size_t rxBufferSize, uint8_t *txBuffer, size_t txBufferSize)
    : _ThingSetZephyrCanInterface::_ThingSetZephyrCanInterface(canDevice), _rxBuffer(rxBuffer),
      _rxBufferSize(rxBufferSize), _txBuffer(txBuffer), _txBufferSize(txBufferSize)
{
    k_sem_init(&_lock, 1, 1);
}

ThingSetZephyrCanInterface::~ThingSetZephyrCanInterface()
{
    isotp_fast_unbind(&_requestResponseContext);
}

void ThingSetZephyrCanInterface::onRequestResponseReceived(net_buf *buffer, int remainingLength,
                                                           isotp_fast_addr address, void *arg)
{
    uint8_t errorResponse[] = { ThingSetStatusCode::internalServerError };
    ThingSetZephyrCanInterface *self = (ThingSetZephyrCanInterface *)arg;
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
            result = self->_inboundRequestCallback(self->_rxBuffer, len, self->_txBuffer, self->_txBufferSize);
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

void ThingSetZephyrCanInterface::onAddressClaimReceived(const device *dev, can_frame *frame, void *arg)
{
    ThingSetZephyrCanInterface *self = (ThingSetZephyrCanInterface *)arg;
    auto canId = CanID::create(frame->id);
    if (self->_nodeAddress == canId.getSource()) {
        k_event_post(&self->_events, AddressClaimEvent::alreadyUsed);
    }

    if (self->_addressClaimCallback) {
        self->_addressClaimCallback(frame->data, canId.getSource());
    }
}

void ThingSetZephyrCanInterface::onAddressDiscoverSent(const device *dev, int error, void *arg)
{
    ThingSetZephyrCanInterface *self = (ThingSetZephyrCanInterface *)arg;
    if (error == 0) {
        k_event_post(&self->_events, AddressClaimEvent::discoverMessageSent);
    }
}

void ThingSetZephyrCanInterface::onAnyAddressDiscoverReceived(const device *dev, can_frame *frame, void *arg)
{
    ThingSetZephyrCanInterface *self = (ThingSetZephyrCanInterface *)arg;
    auto canId = CanID::create(frame->id);
    if (self->_nodeAddress == canId.getSource()) {
        /* received a discovery frame for an address we are already in the process of claiming */
        self->claimAddress(self->_nodeAddress);
    }
}

void ThingSetZephyrCanInterface::onAddressDiscoverReceived(const device *dev, can_frame *frame, void *arg)
{
    ThingSetZephyrCanInterface *self = (ThingSetZephyrCanInterface *)arg;
    self->claimAddress(self->_nodeAddress);
}

static void onRequestResponseError(int8_t error, isotp_fast_addr addr, void *arg)
{}

static void onRequestResponseSent(int result, isotp_fast_addr addr, void *arg)
{}

bool ThingSetZephyrCanInterface::claimAddress(uint8_t nodeAddress)
{
    CanFrame frame(CanID()
                       .setSource(nodeAddress)
                       .setTarget(CanID::broadcastAddress)
                       .setMessageType(MessageType::network)
                       .setMessagePriority(MessagePriority::networkManagement));
    memcpy(frame.getData(), Eui::getArray().data(), Eui::getArray().size());
    frame.setLength(Eui::getArray().size());
    return can_send(_canDevice, frame.getFrame(), K_MSEC(10), nullptr, nullptr) == 0;
}

int ThingSetZephyrCanInterface::addFilter(CanID &canId, void (*callback)(const device *, can_frame *, void *))
{
    canId = canId.setMessageType(MessageType::network);
    can_filter filter = {
        .id = canId,
        .mask = canId.getMask(),
        .flags = CAN_FILTER_IDE
#ifdef CAN_FILTER_DATA
                 | CAN_FILTER_DATA
#endif
        ,
    };
    return can_add_rx_filter(_canDevice, callback, this, &filter);
}

bool ThingSetZephyrCanInterface::bind(uint8_t nodeAddress)
{
    if (_nodeAddress == CanID::broadcastAddress) {
        LOG_INF("Starting address claim for CAN interface %s", _canDevice->name);
        k_event_init(&_events);

        can_set_mode(_canDevice, CAN_MODE_FD);
        can_start(_canDevice);

        int filterId = addFilter(CanID().setTarget(CanID::broadcastAddress).setBridge(0), onAddressClaimReceived);
        filterId = addFilter(CanID().setSource(CanID::anonymousAddress), onAnyAddressDiscoverReceived);

        CanFrame txFrame;
        while (1) {
            k_event_clear(&_events, AddressClaimEvent::alreadyUsed);

            /* send out address discovery frame */
            uint8_t rand = sys_rand32_get() & 0xFF;
            txFrame.setId(CanID()
                              .setMessageType(MessageType::network)
                              .setMessagePriority(MessagePriority::networkManagement)
                              .setRandomElement(rand)
                              .setTarget(_nodeAddress)
                              .setSource(CanID::anonymousAddress));
            int err = can_send(_canDevice, txFrame.getFrame(), K_MSEC(10), onAddressDiscoverSent, this);
            if (err != 0) {
                k_sleep(K_MSEC(100));
                continue;
            }

            uint32_t event = k_event_wait(&_events, AddressClaimEvent::alreadyUsed, false, K_MSEC(500));
            if (event & AddressClaimEvent::alreadyUsed) {
                /* try again with new random node_addr between 0x01 and 0xFD */
                _nodeAddress = CanID::minAddress + sys_rand32_get() % (CanID::maxAddress - CanID::minAddress);
                LOG_WRN("Node addr already in use, trying 0x%.2X", _nodeAddress);
            }
            else {
                can_bus_err_cnt err_cnt_before;
                can_get_state(_canDevice, NULL, &err_cnt_before);

                if (!claimAddress(_nodeAddress)) {
                    k_sleep(K_MSEC(100));
                    continue;
                }

                can_bus_err_cnt err_cnt_after;
                can_get_state(_canDevice, NULL, &err_cnt_after);

                if (err_cnt_after.tx_err_cnt <= err_cnt_before.tx_err_cnt) {
                    break;
                }

                /* Continue the loop in the very unlikely case of a collision because two nodes with
                 * different EUI-64 tried to claim the same node address at exactly the same time.
                 */
            }
        }

        can_remove_rx_filter(_canDevice, filterId);

        filterId =
            addFilter(CanID().setSource(CanID::anonymousAddress).setTarget(_nodeAddress), onAddressDiscoverReceived);
        if (filterId < 0) {
            LOG_ERR("Failed to add address discovery filter: %d", filterId);
            return false;
        }

        LOG_INF("CAN device %s bound to address 0x%x", _canDevice->name, _nodeAddress);
    }
    return true;
}

bool ThingSetZephyrCanInterface::listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
    isotp_fast_addr rxAddress = {
        .ext_id = CanID()
                      .setMessageType(MessageType::requestResponse)
                      .setMessagePriority(MessagePriority::channel)
                      .setTarget(_nodeAddress),
    };
#pragma GCC diagnostic pop
    _inboundRequestCallback = callback;
    return isotp_fast_bind(&_requestResponseContext, _canDevice, rxAddress, &flowControlOptions,
                           onRequestResponseReceived, this, onRequestResponseError, onRequestResponseSent)
           == 0;
}

} // namespace ThingSet::Can::Zephyr
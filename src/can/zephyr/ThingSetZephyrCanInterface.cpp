/*
 * Copyright (c) 2024 Brill Power. All rights reserved.
 *
 * SPDX-License-Identifier: Proprietary
 */
#include "can/zephyr/ThingSetZephyrCanInterface.hpp"
#include "can/zephyr/CanFrame.hpp"
#include "Eui.hpp"
#include "can/CanID.hpp"
#include "internal/logging.hpp"
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

ThingSetZephyrCanInterface::ThingSetZephyrCanInterface(const device *const canDevice)
    : _canDevice(canDevice)
{
    _nodeAddress = CanID::broadcastAddress;
}

ThingSetZephyrCanInterface::~ThingSetZephyrCanInterface()
{
    isotp_fast_unbind(&_listener.requestResponseContext);
}

void ThingSetZephyrCanInterface::onRequestResponseReceived(net_buf *buffer, int remainingLength, isotp_fast_addr address, void *arg)
{
    Listener *listener = (Listener *)arg;
    size_t len = net_buf_frags_len(buffer);
    uint8_t rxBuffer[1024];
    net_buf_linearize(rxBuffer, sizeof(rxBuffer), buffer, 0, len);
    // if a response to a request we sent, dispatch accordingly, but for now, just assume it is inbound
    uint8_t txBuffer[1024];
    len = listener->callback(rxBuffer, len, txBuffer, sizeof(txBuffer));
    if (len > 0) {
        int result = isotp_fast_send(&listener->requestResponseContext, txBuffer, len, isotp_fast_addr { .ext_id = CanID::create(address.ext_id).getReplyId() }, nullptr);
        if (result != 0) {
            LOG_ERR("Error %d sending reply to message from 0x%d", result, address.ext_id);
        }
    }
}

void ThingSetZephyrCanInterface::AddressDiscoverer::onAddressClaimReceived(const struct device *dev, struct can_frame *frame, void *arg)
{
    AddressDiscoverer *self = (AddressDiscoverer *)arg;
    auto canId = CanID::create(frame->id);
    if (self->nodeAddress == canId.getSource()) {
        k_event_post(&self->events, AddressClaimEvent::alreadyUsed);
    }

    // TODO: callback for claim forwarding
}

void ThingSetZephyrCanInterface::AddressDiscoverer::onAddressDiscoverSent(const struct device *dev, int error, void *arg)
{
    AddressDiscoverer *self = (AddressDiscoverer *)arg;
    if (error == 0) {
        k_event_post(&self->events, AddressClaimEvent::discoverMessageSent);
    }
}

static void onRequestResponseError(int8_t error, struct isotp_fast_addr addr, void *arg)
{
}

static void onRequestResponseSent(int result, void *arg)
{
}

bool ThingSetZephyrCanInterface::tryClaimAddress(uint8_t nodeAddress)
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

bool ThingSetZephyrCanInterface::bind(uint8_t nodeAddress)
{
    if (_nodeAddress == CanID::broadcastAddress) {
        LOG_INF("Starting address claim for CAN interface %s", _canDevice->name);
        AddressDiscoverer claimer { .nodeAddress = nodeAddress };
        k_event_init(&claimer.events);

        can_set_mode(_canDevice, CAN_MODE_FD);
        can_start(_canDevice);
        auto addressClaimId = CanID().setMessageType(MessageType::network)
            .setTarget(CanID::broadcastAddress)
            .setBridge(0);
        can_filter addressClaimFilter = {
            .id = addressClaimId,
            .mask = addressClaimId.getMask(),
            .flags = CAN_FILTER_IDE | CAN_FILTER_DATA,
        };
        int filterId = can_add_rx_filter(_canDevice, AddressDiscoverer::onAddressClaimReceived, &claimer, &addressClaimFilter);
        CanFrame txFrame;
        while (1) {
            k_event_clear(&claimer.events, AddressClaimEvent::alreadyUsed);

            /* send out address discovery frame */
            uint8_t rand = sys_rand32_get() & 0xFF;
            txFrame.setId(CanID().setMessageType(MessageType::network)
                .setMessagePriority(MessagePriority::networkManagement)
                .setRandomElement(rand)
                .setTarget(claimer.nodeAddress)
                .setSource(CanID::anonymousAddress));
            int err =
                can_send(_canDevice, txFrame.getFrame(), K_MSEC(10), AddressDiscoverer::onAddressDiscoverSent, &claimer);
            if (err != 0) {
                k_sleep(K_MSEC(100));
                continue;
            }

            uint32_t event =
                k_event_wait(&claimer.events, AddressClaimEvent::alreadyUsed, false, K_MSEC(500));
            if (event & AddressClaimEvent::alreadyUsed) {
                /* try again with new random node_addr between 0x01 and 0xFD */
                claimer.nodeAddress = CanID::minAddress
                    + sys_rand32_get() % (CanID::maxAddress - CanID::minAddress);
                LOG_WRN("Node addr already in use, trying 0x%.2X", claimer.nodeAddress);
            }
            else {
                can_bus_err_cnt err_cnt_before;
                can_get_state(_canDevice, NULL, &err_cnt_before);

                if (!tryClaimAddress(claimer.nodeAddress)) {
                    k_sleep(K_MSEC(100));
                    continue;
                }

                can_bus_err_cnt err_cnt_after;
                can_get_state(_canDevice, NULL, &err_cnt_after);

                if (err_cnt_after.tx_err_cnt <= err_cnt_before.tx_err_cnt) {
                    LOG_INF("Using CAN node address 0x%.2X on %s", claimer.nodeAddress,
                            _canDevice->name);
                    break;
                }

                /* Continue the loop in the very unlikely case of a collision because two nodes with
                * different EUI-64 tried to claim the same node address at exactly the same time.
                */
            }
        }
        can_remove_rx_filter(_canDevice, filterId);

        // no-op for now; negotiation happens elsewhere
        _nodeAddress = claimer.nodeAddress;
        LOG_INF("CAN device %s bound to address 0x%x", _canDevice->name, _nodeAddress);
    }
    return true;
}

bool ThingSetZephyrCanInterface::publish(CanID &id, uint8_t *buffer, size_t length)
{
    CanFrame frame(id);
    memcpy(frame.getData(), buffer, length);
    frame.setLength(length);
    frame.setFd(true);
    int result = can_send(_canDevice, frame.getFrame(), K_MSEC(CONFIG_THINGSET_CAN_REPORT_SEND_TIMEOUT), nullptr, nullptr);
    return result == 0;
}

bool ThingSetZephyrCanInterface::listen(std::function<int(uint8_t *, size_t, uint8_t *, size_t)> callback)
{
    isotp_fast_addr rxAddress = {
        .ext_id = CanID().setMessageType(MessageType::requestResponse)
            .setMessagePriority(MessagePriority::channel)
            .setTarget(_nodeAddress),
    };
    _listener.callback = callback;
    return isotp_fast_bind(&_listener.requestResponseContext, _canDevice, rxAddress,
        &flowControlOptions, onRequestResponseReceived, &_listener, onRequestResponseError,
        onRequestResponseSent) == 0;
}

} // namespace ThingSet::Can::Zephyr
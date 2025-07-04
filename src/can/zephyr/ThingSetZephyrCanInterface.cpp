/*
 * Copyright (c) 2024 Brill Power.
 * Copyright (c) The ThingSet Project Contributors
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

_ThingSetZephyrCanInterface::_ThingSetZephyrCanInterface(const device *const canDevice) : _canDevice(canDevice)
{
    _nodeAddress = CanID::broadcastAddress;
    k_sem_init(&_lock, 1, 1);
}

const device *const _ThingSetZephyrCanInterface::getDevice()
{
    return _canDevice;
}

ThingSetZephyrCanStubInterface::ThingSetZephyrCanStubInterface(const device *const canDevice)
    : _ThingSetZephyrCanInterface(canDevice)
{}

bool ThingSetZephyrCanStubInterface::bind(uint8_t nodeAddress)
{
    _nodeAddress = nodeAddress;
    return true;
}

ThingSetZephyrCanInterface::ThingSetZephyrCanInterface(const device *const canDevice)
    : _ThingSetZephyrCanInterface(canDevice, rxBuffer)
{
}

ThingSetZephyrCanInterface::~ThingSetZephyrCanInterface()
{
    can_remove_rx_filter(_canDevice, _discoverFilterId);
    can_remove_rx_filter(_canDevice, _claimFilterId);
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

static void onAddressClaimSent(const device *dev, int error, void *arg)
{}

bool ThingSetZephyrCanInterface::claimAddress(uint8_t nodeAddress)
{
    LOG_INFO("Asserting claim to 0x%.2X", nodeAddress);
    CanFrame frame(CanID()
                       .setSource(nodeAddress)
                       .setTarget(CanID::broadcastAddress)
                       .setMessageType(MessageType::network)
                       .setMessagePriority(MessagePriority::networkManagement));
    frame.setFd(true);
    memcpy(frame.getData(), Eui::getArray().data(), Eui::getArray().size());
    frame.setLength(Eui::getArray().size());
    return can_send(_canDevice, frame.getFrame(), K_MSEC(10), onAddressClaimSent, nullptr) == 0;
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
        _nodeAddress = nodeAddress;
        LOG_INFO("Starting address claim for CAN interface %s", _canDevice->name);
        k_event_init(&_events);

        can_set_mode(_canDevice, can_get_mode(_canDevice) | CAN_MODE_FD);
        can_start(_canDevice);

        _claimFilterId = addFilter(CanID().setTarget(CanID::broadcastAddress).setBridge(0), onAddressClaimReceived);
        int anyDiscoverFilterId = addFilter(CanID().setSource(CanID::anonymousAddress), onAnyAddressDiscoverReceived);

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
                              .setSource(CanID::anonymousAddress))
                   .setFd(true);
            int err = can_send(_canDevice, txFrame.getFrame(), K_MSEC(10), onAddressDiscoverSent, this);
            if (err != 0) {
                k_sleep(K_MSEC(100));
                continue;
            }

            uint32_t event = k_event_wait(&_events, AddressClaimEvent::alreadyUsed, false, K_MSEC(500));
            if (event & AddressClaimEvent::alreadyUsed) {
                /* try again with new random node_addr between 0x01 and 0xFD */
                uint8_t oldNodeAddress = _nodeAddress;
                _nodeAddress = CanID::minAddress + sys_rand32_get() % (CanID::maxAddress - CanID::minAddress);
                LOG_WARN("Node addr 0x%.2X already in use, trying 0x%.2X", oldNodeAddress, _nodeAddress);
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

        can_remove_rx_filter(_canDevice, anyDiscoverFilterId);

        _discoverFilterId =
            addFilter(CanID().setSource(CanID::anonymousAddress).setTarget(_nodeAddress), onAddressDiscoverReceived);
        if (_discoverFilterId < 0) {
            LOG_ERROR("Failed to add address discovery filter: %d", _discoverFilterId);
            return false;
        }

        LOG_INFO("CAN device %s bound to address 0x%x", _canDevice->name, _nodeAddress);
    }
    return true;
}

} // namespace ThingSet::Can::Zephyr
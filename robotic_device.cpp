/*
 * Driver for the Maplin A37JN Robotic Arm
 *
 * The device has five bidirectional DC motors, and an LED.
 *
 * Realistically this device should use a userland driver and libusb,
 * as an exercise I'm using an in-kernel driver.
 *
 * Copyright (C) 2013 Steve Cotton (steve@s.cotton.clara.co.uk)
 *
 * Thanks to Vadim Zaliva <http://notbrainsurgery.livejournal.com/38622.html>
 * for reverse-engineering the protocol.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License, or
 * (at your option) any later version.
 * 
 */

#include "robotic_device.h"

#include <iostream>
#include <map>
#include <string>
#include <system_error>

namespace {
    const uint16_t DEVICE_USB_ID_VENDOR = 0x1267;
    const uint16_t DEVICE_USB_ID_PRODUCT = 0x0000;
    const int INTERFACE_NUMBER = 0;

    std::map<ArmDevice::Motion, std::string> readableMotion {
        {ArmDevice::Motion::STOP, {"STOP"}},
        {ArmDevice::Motion::UP_RIGHT_CLOSE, {"UP_RIGHT_CLOSE"}},
        {ArmDevice::Motion::DOWN_LEFT_OPEN, {"DOWN_LEFT_OPEN"}}
    };

    // In the command sent over USB, each axis is represented by 2 bits.
    // Set one bit to move it one way, set the other bit to reverse.
    std::map<ArmDevice::Motion, int> bitMotion {
        {ArmDevice::Motion::STOP, 0x0},
        {ArmDevice::Motion::UP_RIGHT_CLOSE, 0x1},
        {ArmDevice::Motion::DOWN_LEFT_OPEN, 0x2}
    };
}

ArmDevice::ArmDevice() {
    m_ctx = nullptr;
    libusb_init(&m_ctx);

    m_dev = libusb_open_device_with_vid_pid(m_ctx, DEVICE_USB_ID_VENDOR, DEVICE_USB_ID_PRODUCT);
    if (!m_dev) {
        std::cout << "Failed to access the robotic arm" << std::endl;
        throw std::system_error();
    }

    int claimError = libusb_claim_interface(m_dev, INTERFACE_NUMBER);
    if (0 != claimError) {
        std::cout << "Found the robotic arm, but failed to interface (" << claimError << ")" << std::endl;
        throw std::system_error();
    }
}

void ArmDevice::motion(std::array<Motion, NUMBER_OF_AXIS> &movements, bool light) {
    for (int axis=0; axis < NUMBER_OF_AXIS; axis++) {
        Motion& motion = movements[axis];
        std::cout << "Movement on axis " << axis << " in direction " << readableMotion[motion] << std::endl;
    }
    std::cout << "Light " << light << std::endl;

    // Thanks to Vadim Zaliva <http://notbrainsurgery.livejournal.com/38622.html> for reverse-engineering the protocol
    uint8_t command[] {0, 0, 0};

    static_assert(3 == sizeof(command), "Mismatch between command data and the data encoder");
    static_assert(5 <= NUMBER_OF_AXIS, "Expected to need 2 bytes to encode axis movements");
    static_assert(8 >= NUMBER_OF_AXIS, "Expected to need 2 bytes to encode axis movements");

    uint8_t& first_byte = command[0];
    for (int axis=0; axis < 4; axis++) {
        Motion& motion = movements[axis];
        first_byte |= bitMotion[motion] << (2 * axis);
    }

    uint8_t& second_byte = command[1];
    for (int axis=4; axis < NUMBER_OF_AXIS; axis++) {
        Motion& motion = movements[axis];
        second_byte |= bitMotion[motion] << (2 * (axis - 4));
    }

    uint8_t& third_byte = command[2];
    if (light) {
        third_byte = 1;
    }

    sendCommand(command);
}

void ArmDevice::sendCommand(uint8_t command[COMMAND_LENGTH]) {
    int bytesSent = libusb_control_transfer(
            m_dev, 0x40, 0x6, 0x100, 0x0,
            command, COMMAND_LENGTH, 0);

    if (COMMAND_LENGTH != bytesSent) {
        std::cout << "I/O error talking to the robot (" << bytesSent << ")" << std::endl;
    }
}

void ArmDevice::stop() {
    uint8_t command[] {0, 0, 0};
    sendCommand(command);
}

ArmDevice::~ArmDevice() {
    if (m_dev) {
        stop();
        libusb_release_interface(m_dev, INTERFACE_NUMBER);
        libusb_close(m_dev);
    }
    libusb_exit(m_ctx);
}

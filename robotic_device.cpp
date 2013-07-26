/**
 * Driver for the Maplin A37JN Robotic Arm
 *
 * The device has five bidirectional DC motors, and an LED.
 *
 * Realistically this device should use a userland driver and libusb,
 * as an exercise I'm using an in-kernel driver.
 *
 * Copyright (C) 2013 Steve Cotton (steve@s.cotton.clara.co.uk)
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>


namespace {
    /** Endpoint to talk to the kernel driver */
    const char* DEVICE_FILENAME = "/dev/mrarm0";

    static const int ARBITARY_IOCTL_NUM_STOP = 0xfedcba01;
    static const int ARBITARY_IOCTL_NUM_CLOSE = 0xfedcba02;
    static const int ARBITARY_IOCTL_NUM_OPEN = 0xfedcba03;

    std::map<ArmDevice::Motion, std::string> readableMotion {
        {ArmDevice::Motion::STOP, {"STOP"}},
        {ArmDevice::Motion::RIMWARDS, {"RIMWARDS"}},
        {ArmDevice::Motion::WIDDERSHINS, {"WIDDERSHINS"}}
    };

    std::map<ArmDevice::Motion, const int> kernelMotion {
        {ArmDevice::Motion::STOP, ARBITARY_IOCTL_NUM_STOP},
        {ArmDevice::Motion::RIMWARDS, ARBITARY_IOCTL_NUM_CLOSE},
        {ArmDevice::Motion::WIDDERSHINS, ARBITARY_IOCTL_NUM_OPEN}
    };
}

ArmDevice::ArmDevice() {
    deviceFd = open(DEVICE_FILENAME, O_WRONLY);
    if (0 > deviceFd) {
        std::cout << "Failed to access the robotic arm" << std::endl;
        throw new IOException();
    }
}

void ArmDevice::motion(std::array<Motion, NUMBER_OF_AXIS> &movements) {
    for (int axis=0; axis < NUMBER_OF_AXIS; axis++) {
        Motion& motion = movements[axis];
        std::cout << "Movement on axis " << axis << " in direction " << readableMotion[motion] << std::endl;
    }

    int number_to_send_to_kernel = kernelMotion[movements[0]];
    if (0 != ioctl(deviceFd, number_to_send_to_kernel)) {
        std::cerr << "Error sending ioctl: " << errno << std::endl;
    }
}

ArmDevice::~ArmDevice() {
    if (0 <= deviceFd) {
        close(deviceFd);
    }
}

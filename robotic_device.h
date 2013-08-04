/*
 * Driver for the Maplin A37JN Robotic Arm
 *
 * The device has five bidirectional DC motors, and an LED.
 *
 * Copyright (C) 2013 Steve Cotton (steve@s.cotton.clara.co.uk)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License, or
 * (at your option) any later version.
 * 
 */
#ifndef __ROBOTIC_DEVICE_H
#define __ROBOTIC_DEVICE_H

#include <array>
#include <string>

#include <libusb.h>

/** Endpoint to talk to the kernel driver */
class ArmDevice {
private:
    libusb_context* m_ctx;
    libusb_device_handle* m_dev;

    /**
     * How many bytes the underlying USB data structure takes.
     */
    static const unsigned int COMMAND_LENGTH = 3;

public:
    /**
     * What movement to make on a particular axis.
     */
    enum class Motion {
        STOP,
        /** Depending on axis, lift, turn the base right, or close the claw */
        UP_RIGHT_CLOSE,
        /** Depending on axis, lower, turn the base left, or open the claw */
        DOWN_LEFT_OPEN
    };

    /**
     * How many motors there are in the device (each one gets a Motion).
     */
    static const unsigned int NUMBER_OF_AXIS = 5;

public:
    /**
     * Constructor, will throw an exception if the device can not be accessed.
     */
    ArmDevice();

    /**
     * Turn on or off the movement motors.  Calling this will change all the
     * motor's movements (any set to STOP will stop, even if they were moving
     * before).
     *
     * @param light turn the LED on or off - again this overwrites the current situation
     */
    void motion(std::array<Motion, NUMBER_OF_AXIS> &movements, bool light);

    /**
     * Stop all the motors moving.  Equivalent to calling motion() with all
     * of the movements set to STOP, and light==false.
     */
    void stop();

    /**
     * Destruction will automatically stop the device moving.
     */
    ~ArmDevice();

private:
    /**
     * Low-level communication.
     */
    void sendCommand(uint8_t command[COMMAND_LENGTH]);
};

#endif

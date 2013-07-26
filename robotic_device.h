/**
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

#include <map>
#include <string>

class IOException {
};

/** Endpoint to talk to the kernel driver */
class ArmDevice {
private:
    int deviceFd;

public:
    /**
     * What movement to make on a particular axis.
     * Which direction is which is still to be decided, hence the odd names.
     */
    enum class Motion {
        STOP,
        RIMWARDS,
        WIDDERSHINS
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
     * Turn on or off the movement motors.
     */
    void motion(std::array<Motion, NUMBER_OF_AXIS> &movements);

    /**
     * Destruction will automatically stop the device moving.
     */
    ~ArmDevice();
};


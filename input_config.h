/*
 * Control the Maplin A37JN Robotic Arm via a joystick.
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

#include "robotic_device.h"

/**
 * Placeholder for user-definable mapping of the input axis to the robot's axis.
 * For now it is a hardcoded list.
 */
namespace InputMapping {
    namespace {
        static std::map<const int, const int> outputToInput {
            {0, 5},
            {1, 3},
            {2, 2},
            {3, 1},
            {4, 0}
        };
    }

    /**
     * Given an output axis, state which joystick axis should be read.
     */
    int getInputForOutput(int output) {
        return outputToInput[output];
    }
};

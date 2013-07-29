/*
 * Control the Maplin A37JN Robotic Arm via a joystick.
 *
 * Copyright (C) 2013 Steve Cotton (steve@s.cotton.clara.co.uk)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License, or
 * (at your option) any later version.
 * 
 */

#include "input_config.h"

#include <stdexcept>
#include <iostream>

using InputMapping::AxisMapping;
using InputMapping::ButtonMapping;

namespace {
    std::vector<AxisMapping> axisMapping {
        // no joystick axis is mapped to output zero (open/close the claw)
        {3, 1, false},
        {2, 2, true},
        {1, 3, false},
        {0, 4, true}
    };

    std::vector<ButtonMapping> buttonMapping {
        {5, 0, ArmDevice::Motion::DOWN_LEFT_OPEN},
        {7, 0, ArmDevice::Motion::UP_RIGHT_CLOSE}
    };

    // buttons which turn on the light, can also be mapped to movement
    std::vector<int> lightButtons {{6}, {7}};
};

const std::vector<AxisMapping> InputMapping::getAxisMappings() {
    return axisMapping;
}

const std::vector<ButtonMapping> InputMapping::getButtonMappings() {
    return buttonMapping;
}

const std::vector<int> InputMapping::getLightButtons() {
    return lightButtons;
}

void InputMapping::sanityCheckConfig(int numAxis, int numButtons) {
    for (auto mapping = axisMapping.begin(); mapping != axisMapping.end(); mapping++) {
        const int inAxis = mapping->inAxis;
        const int outAxis = mapping->outAxis;

        if (0 > inAxis) {
            std::cout << "The joystick configuration is wrong (negative numbers for axis)." << std::endl;
        } else if (inAxis >= numAxis) {
            std::cout << "Your joystick has " << numAxis << " axes, but controlling the robot needs at least " << (inAxis + 1) << std::endl;
        }

        if (0 > outAxis || outAxis >= ArmDevice::NUMBER_OF_AXIS) {
            std::cerr << "This configuration expects a different robotic device; it's configured to drive motor #" << outAxis << std::endl;
            throw std::runtime_error("Input configured to drive different device");
        }
    }

    for (auto mapping = buttonMapping.begin(); mapping != buttonMapping.end(); mapping++) {
        const int inButton = mapping->inButton;
        const int outAxis = mapping->outAxis;

        if (0 > inButton) {
            std::cout << "The joystick configuration is wrong (negative numbers for buttons)." << std::endl;
        } else if (inButton >= numButtons) {
            std::cout << "Your joystick has " << numButtons << " buttons, but controlling the robot needs at least " << (inButton + 1) << std::endl;
        }

        if (0 > outAxis || outAxis >= ArmDevice::NUMBER_OF_AXIS) {
            std::cerr << "This configuration expects a different robotic device; it's configured to drive motor #" << outAxis << std::endl;
            throw std::runtime_error("Input configured to drive different device");
        }
    }
}

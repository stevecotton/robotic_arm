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

#include <map>
#include <vector>

#include "robotic_device.h"

/**
 * Placeholder for the user to configure which joystick sticks or buttons
 * move the arm.
 *
 * For now it is a hardcoded list.
 */
namespace InputMapping {

    /** Configuration for a joystick stick or pad */
    class AxisMapping {
    public:
        const int inAxis;
        const int outAxis;
        /** True if positive input values map to DOWN_LEFT_OPEN */
        const bool reversed;

        AxisMapping(int in, int out, bool rev) :
                inAxis(in),
                outAxis(out),
                reversed(rev) {
        }
    };

    /** Configuration for pressed/not pressed buttons */
    class ButtonMapping {
    public:
        const int inButton;
        const int outAxis;
        const ArmDevice::Motion direction;

        ButtonMapping(int in, int out, ArmDevice::Motion dir) :
                inButton(in),
                outAxis(out),
                direction(dir) {
        }
    };

    const std::vector<AxisMapping> getAxisMappings();
    const std::vector<ButtonMapping> getButtonMappings();

    /**
     * These buttons turn on the light.
     */
    const std::vector<int> getLightButtons();

    /**
     * Check that the mapping in InputMapping is sensible - that the
     * joystick has enough axis/buttons, and more importantly that the
     * hardware to be driven has the expected number of motors.
     *
     * Needing a more advanced joystick merely gets a text warning,
     * trying to drive non-existant motors throws a std::runtime_error.
     *
     * @param numAxis the spec of the joystick that we're using
     */
    void sanityCheckConfig(int numAxis, int numButtons);
};

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

#include <iostream>

#include <SDL.h>

#include "robotic_device.h"
#include "input_config.h"

namespace {
    /** How far the joystick has to be pushed to be registered as being moved */
    const Sint16 INPUT_VALUE_THRESHOLD=16192;
}

int main(int argc, char** argv) {
    ArmDevice device;

    std::cout << "Setting up SDL" << std::endl;
    // SDL_INIT_VIDEO is needed for SDL_WaitEvent
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    SDL_JoystickEventState(SDL_ENABLE);

    int numJoysticks = SDL_NumJoysticks();
    if (1 != numJoysticks) {
        std::cout << "Found " << numJoysticks << " possible joysticks." << std::endl;
        std::cout << "Handling none or more than one is currently unsupported." << std::endl;
        // TODO if this ever reaches release quality
    } else {
        // Eventually this might support stick selection, but for now only one stick will work
        const int joystickIndex = 0;

        SDL_Joystick* inputStick = SDL_JoystickOpen(joystickIndex);

        const int numAxis = SDL_JoystickNumAxes(inputStick);
        const int numButtons = SDL_JoystickNumButtons(inputStick);
        InputMapping::sanityCheckConfig(numAxis, numButtons);

        bool keepRunning = true;
        while (keepRunning) {
            std::cout << "eventloop" << std::endl;
            SDL_Event event;
            int waitResult = SDL_WaitEvent(&event);

            if (1 != waitResult) {
                std::cout << "Event loop failed: " << waitResult << std::endl;
                event.type = SDL_QUIT;
            }

            switch (event.type) {
                case SDL_QUIT: {
                    keepRunning = false;
                    break;
                }
                case SDL_JOYBUTTONDOWN:
                case SDL_JOYBUTTONUP:
                case SDL_JOYAXISMOTION: {
                    // For any change on the joystick, simply reread the whole state
                    std::cout << "Event " << (int)event.type << std::endl;
                    std::array<ArmDevice::Motion, ArmDevice::NUMBER_OF_AXIS> movement = {ArmDevice::Motion::STOP};

                    auto& axisMapping = InputMapping::getAxisMappings();
                    for (auto mapping = axisMapping.begin(); mapping != axisMapping.end(); mapping++) {
                        const int inAxis = mapping->inAxis;
                        const int outAxis = mapping->outAxis;

                        if (0 > inAxis) {
                            std::cout << "The joystick configuration is wrong (negative numbers for axis)." << std::endl;
                        } else if (inAxis >= numAxis) {
                            std::cout << "Your joystick ." << std::endl;
                        }


                        // SDL joystick coordinates are positive down-right.  The arm seems to associate down-left with negative, so reverse one axis.
                        // The SDL value is a signed 16-bit, promoted in this code to handle 16-bit MIN_VALUE.
                        int32_t value = SDL_JoystickGetAxis(inputStick, inAxis);
                        if (mapping->reversed) {
                            value = -value;
                        }

                        std::cout << "axis " << inAxis << "->" << outAxis << " value " << value << std::endl;

                        if (value < -INPUT_VALUE_THRESHOLD) {
                            movement[outAxis] = ArmDevice::Motion::UP_RIGHT_CLOSE;
                        } else if (value > INPUT_VALUE_THRESHOLD) {
                            movement[outAxis] = ArmDevice::Motion::DOWN_LEFT_OPEN;
                        }
                    }

                    auto& buttonMapping = InputMapping::getButtonMappings();
                    for (auto mapping = buttonMapping.begin(); mapping != buttonMapping.end(); mapping++) {
                        const bool pressed = SDL_JoystickGetButton(inputStick, mapping->inButton);
                        if (pressed) {
                            movement[mapping->outAxis] = mapping->direction;
                        }
                    }

                    auto& lightButtons = InputMapping::getLightButtons();
                    bool led = false;
                    for (auto mapping = lightButtons.begin(); mapping != lightButtons.end(); mapping++) {
                        if (SDL_JoystickGetButton(inputStick, *mapping)) {
                            led = true;
                        }
                    }

                    device.motion(movement, led);
                }
            }
        }
    }

    SDL_Quit();
    return 0;
}

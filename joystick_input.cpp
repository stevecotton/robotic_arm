/**
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
#include <map>
#include <SDL/SDL.h>

#include "robotic_device.h"
#include "input_config.h"

namespace {
    /** How far the joystick has to be pushed to be registered as being moved */
    const Sint16 INPUT_VALUE_THRESHOLD=16192;
    /** Which button on the joystick controls the light (2nd right shoulder on my pad) */
    const int INPUT_BUTTON_LED = 7;
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
        if (numAxis < ArmDevice::NUMBER_OF_AXIS) {
            std::cout << "This joystick has less axis than the robot." << std::endl;
        }

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

                    for (int outAxis=0; outAxis < numAxis ; outAxis++) {
                        const int inAxis = InputMapping::getInputForOutput(outAxis);

                        // SDL joystick coordinates are positive down-right.  The arm seems to associate down-left with negative, so reverse one axis.
                        // The SDL value is a signed 16-bit, promoted in this code to handle 16-bit MIN_VALUE.
                        int32_t value = SDL_JoystickGetAxis(inputStick, inAxis);
                        if (0 == inAxis % 2) {
                            value = -value;
                        }

                        std::cout << "axis " << inAxis << "->" << outAxis << " value " << value << std::endl;

                        if (value < -INPUT_VALUE_THRESHOLD) {
                            movement[outAxis] = ArmDevice::Motion::UP_RIGHT_CLOSE;
                        } else if (value > INPUT_VALUE_THRESHOLD) {
                            movement[outAxis] = ArmDevice::Motion::DOWN_LEFT_OPEN;
                        }
                    }

                    bool led = SDL_JoystickGetButton(inputStick, INPUT_BUTTON_LED);

                    device.motion(movement, led);
                }
            }
        }
    }

    SDL_Quit();
    return 0;
}

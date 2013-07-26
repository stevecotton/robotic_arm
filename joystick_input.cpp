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
                case SDL_JOYAXISMOTION: {
                    std::cout << "Event " << (int)event.type << std::endl;
                    std::array<ArmDevice::Motion, ArmDevice::NUMBER_OF_AXIS> movement = {ArmDevice::Motion::STOP};

                    for (int i=0; i < numAxis ; i++) {
                        Sint16 value = SDL_JoystickGetAxis(inputStick, i);
                        if (value < -INPUT_VALUE_THRESHOLD) {
                            movement[i] = ArmDevice::Motion::RIMWARDS;
                        } else if (value > INPUT_VALUE_THRESHOLD) {
                            movement[i] = ArmDevice::Motion::WIDDERSHINS;
                        }
                    }

                    device.motion(movement);
                }
            }
        }
    }

    SDL_Quit();
    return 0;
}
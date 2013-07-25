/**
 * Read status of the joystick, and map that to movement of the arm
 */

#include <iostream>
#include <SDL.h>

/** How far the joystick has to be pushed to be registered as being moved */
namespace {
    const Sint16 INPUT_VALUE_THRESHOLD=16192;
}

int main(int argc, char** argv) {
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

                    for (int i=0; i < numAxis ; i++) {
                        Sint16 value = SDL_JoystickGetAxis(inputStick, i);
                        if (value < -INPUT_VALUE_THRESHOLD) {
                            std::cout << "axis " << i << " active negative" << std::endl;
                        } else if (value > INPUT_VALUE_THRESHOLD) {
                            std::cout << "axis " << i << " active positive" << std::endl;
                        }
                    }
                }
            }
        }
    }

    SDL_Quit();
    return 0;
}

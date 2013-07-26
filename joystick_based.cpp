/**
 * Read status of the joystick, and map that to movement of the arm
 */

#include <iostream>
#include <map>
#include <string>
#include <SDL/SDL.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

namespace {
    /** How far the joystick has to be pushed to be registered as being moved */
    const Sint16 INPUT_VALUE_THRESHOLD=16192;
    /** Endpoint to talk to the kernel driver */
    const char* DEVICE_FILENAME = "/dev/mrarm0";
}

class IOException {
};


namespace {
    static const int ARBITARY_IOCTL_NUM_STOP = 0xfedcba01;
    static const int ARBITARY_IOCTL_NUM_CLOSE = 0xfedcba02;
    static const int ARBITARY_IOCTL_NUM_OPEN = 0xfedcba03;
}

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

private:
    std::map<Motion, std::string> readableMotion {
        {Motion::STOP, {"STOP"}},
        {Motion::RIMWARDS, {"RIMWARDS"}},
        {Motion::WIDDERSHINS, {"WIDDERSHINS"}}
    };

    std::map<Motion, const int> kernelMotion {
        {Motion::STOP, ARBITARY_IOCTL_NUM_STOP},
        {Motion::RIMWARDS, ARBITARY_IOCTL_NUM_CLOSE},
        {Motion::WIDDERSHINS, ARBITARY_IOCTL_NUM_OPEN}
    };

public:
    ArmDevice() {
        deviceFd = open(DEVICE_FILENAME, O_WRONLY);
        if (0 > deviceFd) {
            std::cout << "Failed to access the robotic arm" << std::endl;
            throw new IOException();
        }
    }

    void motion(std::array<Motion, NUMBER_OF_AXIS> &movements) {
        for (int axis=0; axis < NUMBER_OF_AXIS; axis++) {
            Motion& motion = movements[axis];
            std::cout << "Movement on axis " << axis << " in direction " << readableMotion[motion] << std::endl;
        }

        int number_to_send_to_kernel = kernelMotion[movements[0]];
        if (0 != ioctl(deviceFd, number_to_send_to_kernel)) {
            std::cerr << "Error sending ioctl: " << errno << std::endl;
        }
    }

    ~ArmDevice() {
        if (0 <= deviceFd) {
            close(deviceFd);
        }
    }
};

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

CXXFLAGS += -std=c++11 -g -O3 -Wall
CXXFLAGS += `pkg-config --cflags sdl`
CXXFLAGS += `pkg-config --cflags libusb-1.0`
LDLIBS += `pkg-config --libs sdl`
LDLIBS += `pkg-config --libs libusb-1.0`

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(LDLIBS) -c -o $@ $^

joystick_arm_controller: joystick_input.o robotic_device.o input_config.o
	$(CXX) $(CXXFLAGS) $(LDLIBS) -o $@ $^

all: joystick_based

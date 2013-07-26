CXXFLAGS += -std=c++11 -g -O0
LDLIBS += -l SDL

joystick_arm_controller: joystick_input.o robotic_device.o
	$(CXX) $(CXXFLAGS) $(LDLIBS) -o $@ $^

all: joystick_based

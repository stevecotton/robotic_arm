CXXFLAGS += -std=c++11 -g -O0
LDLIBS += -l SDL

joystick_based: joystick_based.o
	$(CXX) $(CXXFLAGS) $(LDLIBS) -o $@ $^

all: joystick_based

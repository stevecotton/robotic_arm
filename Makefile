joystick_based: joystick_based.cpp
	g++ -std=c++11 -O0 -g -o joystick_based -I /usr/include/SDL -l SDL joystick_based.cpp

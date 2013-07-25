joystick_based: joystick_based.cpp
	g++ -O0 -g -o joystick_based -I /usr/include/SDL -l SDL joystick_based.cpp

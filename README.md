Joystick based controller for the OWI / Maplin Robotic Arm
==========================================================

The device is a toy robotic arm, with 5 motors and an LED light.

There are no sensors in the device, thus no way to
programmatically determine where the arm is.  Even when the
limits are reached, there's no feedback to the on-device chip to
turn off the motors.

There are mechanical limits that audibly click when the movement
has reached its limits, but the user must be able to see the
device and stop before those limits are reached.

It's sold as:
 * Maplin A37JN Robotic Arm
 * OWI Robotic Arm Edge

Status of this project
----------------------

This is a command-line only tool that assumes you have exactly
one joypad.  The code to talk to the device is complete, but
the joystick handling is minimal, and there's no UI.

Configuring for different joypad layouts currently requires
recompiling - the settings are at the top of `input_config.cpp`.

Dependencies:
 * SDL
 * libusb 1.0

History
-------

I wrote this to practise with an in-kernel driver, this project
simply being the client.

But there's almost no reason to need an in-kernel module rather
than using libusb.  So having implemented the kernel driver
driving one motor and the light, I continued with this project
using plain libusb.

Having it in the kernel does mean that a few edge cases can be
handled, but the one that I know of is very much an edge case.
Specifically hibernating your PC while the motors are running.

Copyright and licensing
-----------------------

Copyright 2013 Steve Cotton

Thanks to Vadim Zaliva for reverse-engineering the protocol.
http://notbrainsurgery.livejournal.com/38622.html

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; version 2 of the
License, or (at your option) any later version.

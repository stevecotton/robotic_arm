#!/usr/bin/python

# Read status of the joystick, and map that to movement of the arm

import pygame
import time

def main():
    try:
        pygame.init()

        pygame.joystick.init()
        stick = pygame.joystick.Joystick(0)
        stick.init()

        running = 1

        while running:
            event = pygame.event.wait()
            if event.type == pygame.QUIT:
                running = 0
            if event.type == pygame.JOYAXISMOTION:
                print ("%d : %f" % (event.axis, event.value));
                if event.value > 0.5 :
                    move_arm(event.axis, 1)
                if event.value < -0.5 :
                    move_arm(event.axis, -1)


    finally:
        pygame.quit()

def move_arm(axis, direction):
    print ("Would move arm axis %d in direction %d"
            % (axis, direction))

main()

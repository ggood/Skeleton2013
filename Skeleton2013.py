#!/usr/bin/env/python

import argparse
import collections
#import serial
import sys
import time

"""
"""


class _SkeletonBase(object):

    def __init__(self):
        pass

    def read_distance(self):
        raise NotImplementedError

    def shake(self):
        raise NotImplementedError

    def throb(self):
        raise NotImplementedError


class MockSkeleton(_SkeletonBase):

    def __init__(self):
        self._read_count = 0
        self._max = 50

    def read_distance(self):
        self._read_count += 1
        if self._read_count > self._max + 20:
            self._read_count = 0
        if self._read_count > self._max:
            return 10
        else:
            return 100;

    def shake(self):
        print "shaking"

    def throb(self):
        print "throbbing"


class Skeleton(_SkeletonBase):

    CMD_READ_DISTANCE = 'd'
    CMD_SHAKE = 's'
    CMD_THROB = 't'

    def __init__(self, tty, baud):
        self._port = serial.Serial(tty, baud)

    def read_distance(self):
        self._port.write(CMD_READ_DISTANCE)
        return int(port.read())

    def shake(self):
        print "shaking"

    def throb(self):
        print "throbbing"



ENTER_DISTANCE_THRESHOLD = 30
LEAVE_DISTANCE_THRESHOLD = 90
WAIT_ENTER = 1
WAIT_LEAVE = 2
SCREAM = "scream.wav"


def play_sound(filename):
    print "Playing %s" % filename


def event_loop(tty, baud):
    distances = collections.deque(maxlen=10)
    #skeleton = Skeleton(tty, baud)
    skeleton = MockSkeleton()
    last_shake_time = time.time()
    state = WAIT_ENTER
    while True:
        distance = skeleton.read_distance()
        print distance
        distances.append(distance)
        if state == WAIT_ENTER:
            if max(set(list(distances))) < ENTER_DISTANCE_THRESHOLD:
                state = WAIT_LEAVE
                print "Waiting for victim to leave"
                skeleton.shake()
                play_sound(SCREAM)
                skeleton.throb()
        elif state == WAIT_LEAVE:
            if max(set(list(distances))) > LEAVE_DISTANCE_THRESHOLD:
                # May also want a time delay
                state = WAIT_ENTER
                print "Waiting for a victim to enter"
        time.sleep(0.1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("tty", help="Serial port for skeleton controller")
    parser.add_argument("--baud", help="Serial port baud rate", default=9600)
    args = parser.parse_args()
    if not args.tty:
        parser.print_help()
        sys.exit(1)
    event_loop(args.tty, args.baud)

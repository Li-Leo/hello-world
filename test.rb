#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import time
import sys, os

def print_time():

    print(time.asctime())
    # print(time.localtime())
    print(time.ctime())
    print(time.time())

if __name__ == '__main__':
    print_time()
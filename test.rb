#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import time,sys, os
import  pathlib
from pathlib import Path
import string

def print_time():
    pass
    print(time.asctime())
    print(time.localtime())
    print(time.ctime())
    # print(time.time())

def print_files():
    p = pathlib.Path('.')
    M = list(p.glob('*.txt'))
    # L=[str(x) for x in p.iterdir()]
    # print(L)
    print(M)
    print(M[0].suffix)
    print(string.ascii_lowercase)


if __name__ == '__main__':
    print_time()
    print_files()
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

with open('A-PumpV1_8_master.txt', 'r') as f:
    n = 0;
    s = ''
    for line in f:
        if n < 10 and '@' not in line:
            s = s + line.replace("\n", ' ') 
            n += 1
        if '@' in line:
            print(int(line.strip('@'),16),line)
    t = s.split(" ")
    print(t)
    print(len(t))
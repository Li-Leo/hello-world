#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import sys

# print((sys.argv[1:]))
in_file = sys.argv[1]

with open(in_file, 'r') as f:
    line_num = 0
    s = ''
    L = []
    M = []
    for line in f:
        if '@' in line:
            L.append(line_num)
            M.append(line.strip('@').strip())
        line_num +=1


# 'A-PumpV1_8_master.txt'
with open(in_file, 'r') as f:
    
    line_num = 0
    SUM = []
    i = 0
    out = []
    out_list = []
    s =''

    for line in f:
        if i < len(L) - 1:
            if '@' not in line and line_num <= L[i+1]:
                s = s + line.replace("\n", ' ') 
        else:
            if '@' not in line and 'q' not in line and line_num < L[-1]:
                s = s + line.replace("\n", ' ')
                s = ''
        
        if "@" in line and line_num >= L[1]:
            i += 1
            s = s.split(' ')
            out = [j for j in s if j != '']
            out_list.append(out)
            # print(out)
            s =''
            
            SUM.append(len(out))

        if 'q' not in line and line_num > L[-1]:
            s = s + line.replace("\n", ' ') 

        line_num += 1
    s = s.split(' ')
    out = [j for j in s if j != '']
    out_list.append(out)
    # print(out)
    # print(t)
    SUM.append(len(out))

    print(SUM)
    print(L)
    # print(out_list)
    dic = dict(zip(M, SUM))
    dic1 = dict(zip(M, out_list))
    print(dic)
    print(out_list[1])
    print(dic1)

'''
with open('code.txt', 'r') as f:
    s = ''
    for line in f:
        s = s + line.replace("\n", ' ')

    s = s.split(' ')
    out = [j for j in s if j != '']

    print(len(out))
    start = 0x23B2A - 0x4400
    print(i)
    lenth = dic['23B2A']
    print(lenth, '\n')
    i = 0
    while i < lenth:
        print(out[i + start], end = " ")
        i += 1
'''
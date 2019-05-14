#!/usr/bin/env python3
# -*- coding: utf-8 -*-

with open('A-PumpV1_8_master.txt', 'r') as f:
    line_num = 0
    s = ''
    L = []
    M = []
    for line in f:
        if '@' in line:
            L.append(line_num)
            M.append(line.strip('@').strip())
        line_num +=1

with open('A-PumpV1_8_master.txt', 'r') as f:
    
    line_num = 0
    SUM = []
    i = 0
    out = []
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
            print(out)
            s =''
            
            SUM.append(len(out))

        if 'q' not in line and line_num > L[-1]:
            s = s + line.replace("\n", ' ') 

        line_num += 1
    s = s.split(' ')
    out = [j for j in s if j != '']
    print(out)
    # print(t)
    SUM.append(len(out))

    print(SUM)
    print(L)
    dic = dict(zip(M,SUM))
    print(dic)
    # t = s.split(" ")

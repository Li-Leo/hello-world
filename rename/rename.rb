#!/usr/bin/env python3import smtplib
# -*- coding: utf-8 -*-
from pathlib import Path
import keyboard, os
from time import sleep

# for f in (Path.cwd()/'en').glob('*'):
#     print(f.name)
#     # f.rename(str(f)+'.rb')#''.join(f.name.suffixs) + '.rb'
#     f.rename(f.with_suffix(''))

for f in (Path.cwd()/'en').glob('*'):
    cmd = 'subl.exe' + ' ' + str(f)
    os.system(cmd)
    sleep(2)
    keyboard.send('ctrl+shift+s')
    sleep(1)
    keyboard.write(f.name + '.rb')
    sleep(0.5)
    keyboard.send('enter')
    sleep(2)   
    keyboard.send('ctrl+shift+w')
    sleep(0.5)

# for f in (Path.cwd()/'en').glob('*'):
#     print(f.name)
#     # f.rename(str(f)+'.rb')#''.join(f.name.suffixs) + '.rb'
#     f.rename(f.with_suffix(''))

# for n in range(len(L)):
#     keyboard.send('ctrl+shift+s')
#     sleep(2)
#     keyboard.write(L[len(L)-n-1] + '.rb')
#     keyboard.send('enter')
#     sleep(2)   
#     keyboard.send('ctrl+w')
#     sleep(1)

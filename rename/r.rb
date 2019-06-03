#!/usr/bin/env python3import smtplib
# -*- coding: utf-8 -*-
from pathlib import Path
import keyboard, os
from time import sleep

# for f in (Path.cwd()/'en').glob('*.rb'):
#     print(f.name)
#     # f.rename(str(f)+'.rb')#''.join(f.name.suffixs) + '.rb'
#     f.rename(f.with_suffix('.abc'))

# for f in (Path.cwd()/'en').glob('*'):
# 	print(f)

lst =[]
for f in (Path("D:/hello-world/rename/en")).glob('*'):
	lst.append(f)
print(lst)

for f in lst:
    cmd = 'subl.exe' + ' ' + str(f)
    os.system(cmd)
    sleep(1)
    keyboard.send('ctrl+shift+s')
    sleep(2)
    keyboard.write(f.name + '.rb')
    sleep(1)
    keyboard.send('enter')
    sleep(3)   
    keyboard.send('ctrl+w')
    sleep(1)
    keyboard.send('alt+f4')
    # print(f)

# os.system('subl.exe')
# sleep(1)
# os.system('subl.exe')
# sleep(1)
# lst.reverse()
# print(lst)

# for f in lst:
#     keyboard.send('ctrl+shift+s')
#     sleep(1)
#     keyboard.write(f.name + '.rb')
#     sleep(1)
#     keyboard.send('enter')
#     sleep(3)   
#     keyboard.send('ctrl+w')
#     sleep(1)

# for f in (Path.cwd()/'en').glob('*'):
#     print(f.name)
#     f.rename(str(f)+'.rb')#''.join(f.name.suffixs) + '.rb'
# #     f.rename(f.with_suffix(''))

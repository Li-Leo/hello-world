#!/usr/bin/env python3import smtplib
# -*- coding: utf-8 -*-
from pathlib import Path
from subprocess import run
from time import sleep
import win32gui
import keyboard
import sys

work_path = Path.cwd()

def windowEnumerationHandler(hwnd, top_windows):
    top_windows.append((hwnd, win32gui.GetWindowText(hwnd)))

def set_focus(title):
    top_windows = []
    win32gui.EnumWindows(windowEnumerationHandler, top_windows)
        
    for i in top_windows:
        if title in i[1]:
            # print (i)
            win32gui.ShowWindow(i[0],5)
            win32gui.SetForegroundWindow(i[0])
            break

def save_as_rb():
    i = 0
    for f in work_path.glob('*'):
        if sys.argv[0] not in f.name and f.is_file():
            cmd = 'subl.exe' + ' ' + str(f)
            run(cmd)
            set_focus("Sublime Text")
            sleep(2)
            keyboard.send('ctrl+shift+s')
            sleep(1)
            keyboard.write(f.name + '.rb')
            sleep(1)
            keyboard.send('enter')
            sleep(2)
            set_focus("Sublime Text")
            keyboard.send('ctrl+w')
            sleep(1)
            i += 1
            print(f.name)
    return i

def move_and_rename():
    p = Path.cwd()/'output'
    
    if not p.exists():
        p.mkdir()
    else:
        for file in p.glob('*'):
            file.unlink()

    for f in work_path.glob('*.rb'):
        if sys.argv[0] not in f.name and f.is_file():
            f.rename(p/f.name)
    
    for f in p.glob('*'):
        f.rename(f.with_suffix(''))

if __name__ == "__main__":
    print(sys.argv[0])
    num = save_as_rb()
    move_and_rename()
    print(f'\n{num} files are decrypted!')
    set_focus('cmd.exe')

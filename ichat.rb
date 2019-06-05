#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import itchat
import os
HELP_MSG = r'''
availeble cmds:
    clac
    shutdown -xx(s)
    cancel
    notepad
'''
@itchat.msg_register(itchat.content.TEXT)
def music_player(msg):
    cmd = ''
    if msg['ToUserName'] != 'filehelper': return
    
    if msg['Text'] == 'calc':
        os.system("calc")
    elif 'shutdown' in msg['Text']:
        cmd = "shutdown /s /t " + (msg['Text'])[9:]
        os.system(cmd)
    elif msg['Text'] == 'cancel':
        os.system("shutdown /a")
    elif msg['Text'] == 'notepad':
        os.system("notepad")
    else:
        itchat.send(msg['Text'], 'filehelper')


itchat.auto_login(True)
itchat.send(HELP_MSG, 'filehelper') 
itchat.run()
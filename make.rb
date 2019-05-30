#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import subprocess
import re
import shutil
from os import environ, rename
from pathlib import Path

def get_name():
    name = ''
    with open(Path.cwd() / "Source/version.h", 'r') as f:
        name = (re.findall('\"[AE]-PumpV1_8_.{5,6}\"?', str(f.readlines())))[0]

    return name.replace('"','')

def get_version():
    v = []
    with open(Path.cwd() / "Source/version.h", 'r') as f:
        for line in f:
            line = line.strip('\n')
            if 'MAJOR_VERSION' in line:
                v.append(re.findall("\d+", line))
            elif 'MINOR_VERSION' in line:
                v.append(re.findall("\d+", line))
            elif 'REVISION_VERSION' in line:
                v.append(re.findall("\d+", line))
            elif 'INTERNAL_VERSION' in line:
                v.append(re.findall("\d+", line))
    return [j[0] for j in v]

def make_release(version):
    file_name = get_name()
    my_env= environ.copy()
    my_env["PATH"] = "C:\Program Files\IAR Systems\Embedded Workbench 7.0\common\\bin;" \
                     + my_env["PATH"]
    cmd = "iarbuild" + " IAR_project\\" + file_name + ".ewp" + " -make Release"

    print(cmd)

    proc = subprocess.run(cmd, shell = True, stdout = subprocess.PIPE, env = my_env)
    L = proc.stdout.decode('utf-8').split('\r\n')
    print(proc.stdout.decode('utf-8'))

    errors = re.findall("\d+", L[-3])

    if errors[0] == '0':
        print('Build success!')
        s = file_name + 'V' + version
        p = Path.cwd() / s
        if p.exists():
            for file in p.glob('*'):
                file.unlink()
        else:
            p.mkdir()
        in_file = Path.cwd() / ('IAR_project/Release/exe/' +file_name + '.txt')
        shutil.copy(in_file, p)
        rename(p / (file_name + '.txt'), p / (s+'.txt'))
    else:
        print('Build fail!')

if __name__ == "__main__":
    #print(get_name())
    v = '.'.join(c for c in get_version())
    print('building version %s...' % v)
    make_release(v)

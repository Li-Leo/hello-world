#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import subprocess
import re
import shutil
from os import environ, rename
from pathlib import Path

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
    my_env= environ.copy()
    my_env["PATH"] = "C:\Program Files\IAR Systems\Embedded Workbench 7.0\common\\bin;" \
                     + my_env["PATH"]

    proc = subprocess.run("iarbuild IAR_project\\a-pumpv1_8_master.ewp -make Release", 
                        shell = True, stdout = subprocess.PIPE, env = my_env)
    L = proc.stdout.decode('utf-8').split('\r\n')
    print(proc.stdout.decode('utf-8'))

    errors = re.findall("\d+", L[-3])

    if errors[0] == '0':
        print('Build success!')
        s = 'a_pumpV1.8_masterV' + version
        p = Path.cwd() / s
        if p.exists():
            for file in p.glob('*'):
                file.unlink()
        else:
            p.mkdir()
        in_file = Path.cwd() / 'IAR_project/Release/exe/A-PumpV1_8_master.txt'
        shutil.copy(in_file, p)
        rename(p / 'A-PumpV1_8_master.txt', p / (s+'.txt'))
    else:
        print('Build fail!')

if __name__ == "__main__":
    v = '.'.join(c for c in get_version())
    print('building version %s...' % v)
    make_release(v)

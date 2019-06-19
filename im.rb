#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from PIL import Image

im = Image.open('logo.bmp')

print(list(im.getdata()))

i =0
with open('im.txt','w') as f:
	for pl in list(im.getdata()):
		i += 1
		if pl:
			f.write('_')
		else:
			f.write('X')
		if i % im.width == 0:
			f.write('\n')
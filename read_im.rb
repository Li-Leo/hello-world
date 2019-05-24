#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from PIL import Image

im = Image.open("logo.bmp")
out = im.transpose(Image.FLIP_TOP_BOTTOM)
out.save('loge1.bmp')
# print(im.format, im.size, im.mode)
# im.show()N
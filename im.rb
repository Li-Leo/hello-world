#!/usr/bin/env python3
# -*- coding: utf-8 -*-from PIL import Image, ImageDraw, ImageFont
from PIL import Image
from PIL import ImageDraw, ImageFont
from PIL import ImageEnhance
def img_to_txt():
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

def generate_text_img(ch):
	img = Image.new('RGB', (40, 40), color = 'black')
	fnt = ImageFont.truetype('Deng.ttf', 40)
	d = ImageDraw.Draw(img)
	d.text((0,0), ch, font=fnt, fill=(255,255,255))
 
	img.save(ch + '.png')

if __name__ == '__main__':
	
	# text_str="和的有人我主产不为这"
	# for ch in text_str:
	# 	generate_text_img(ch)
	im = Image.open('lh.jpg')
	enhancer = ImageEnhance.Sharpness(im)

	for i in range(8):
		factor = i/4.0
		enhancer.enhance(factor).show("Sharpness %f" % factor)
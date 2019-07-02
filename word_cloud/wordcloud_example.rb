#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# pip install numpy matplotlib pillow wordcloud imageio jieba snownlp itchat -i https://pypi.tuna.tsinghua.edu.cn/simple

import wordcloud
import jieba
import imageio

import matplotlib.pyplot as plt
from wordcloud import WordCloud, ImageColorGenerator

def ci_yun():
	w = wordcloud.WordCloud(width=1000,
	                        height=700,
	                        background_color='white',
	                        font_path='msyh.ttf',
	                        scale=10)

	# 调用jieba的lcut()方法对原始文本进行中文分词，得到string
	txt = '同济大学（Tongji University），简称“同济”，是中华人民共和国教育部直属，由教育部、国家海洋局和上海市共建的全国重点大学，历史悠久、声誉卓著，是国家“双一流”、“211工程”、“985工程”重点建设高校，也是收生标准最严格的中国大学之一'
	txtlist = jieba.lcut(txt)
	string = " ".join(txtlist)

	# 将string变量传入w的generate()方法，给词云输入文字
	w.generate(string)

	# 将词云图片导出到当前文件夹
	w.to_file('2output-tongji.png')


def ci_yun_village():
	w = wordcloud.WordCloud(width=1000,
	                        height=700,
	                        background_color='white',
	                        font_path='msyh.ttf')

	# 对来自外部文件的文本进行中文分词，得到string
	txt = open('关于实施乡村振兴战略的意见.txt',encoding='utf-8').read()
	txtlist = jieba.lcut(txt)
	string = " ".join(txtlist)

	# 将string变量传入w的generate()方法，给词云输入文字
	w.generate(string)

	# 将词云图片导出到当前文件夹
	w.to_file('village1.png')

def ci_yun_village_wujiaoxing():
	mk = imageio.imread("wujiaoxing.png")
	w = wordcloud.WordCloud(mask=mk)

	# 构建并配置词云对象w，注意要加scale参数，提高清晰度
	w = wordcloud.WordCloud(width=1000,
	                        height=700,
	                        background_color='white',
	                        font_path='msyh.ttf',
	                        mask=mk,
	                        scale=15)

	# 对来自外部文件的文本进行中文分词，得到string
	txt = open('关于实施乡村振兴战略的意见.txt',encoding='utf-8').read()
	string = ' '.join(jieba.cut(txt))

	# 将string变量传入w的generate()方法，给词云输入文字
	w.generate(string)

	# 将词云图片导出到当前文件夹
	w.to_file('village_wujiaoxing.png')


def ci_yun_village_chinamap():
	
	mk = imageio.imread("chinamap.png")
	w = wordcloud.WordCloud(mask=mk)

	# 构建并配置词云对象w，注意要加scale参数，提高清晰度
	w = wordcloud.WordCloud(width=1000,
	                        height=700,
	                        background_color='white',
	                        font_path='msyh.ttf',
	                        mask=mk,
	                        scale=15)

	# 对来自外部文件的文本进行中文分词，得到string
	f = open('新时代中国特色社会主义.txt',encoding='utf-8')
	txt = f.read()
	txtlist = jieba.lcut(txt)
	string = " ".join(txtlist)

	# 将string变量传入w的generate()方法，给词云输入文字
	w.generate(string)

	# 将词云图片导出到当前文件夹
	w.to_file('1output7-chinamap.png')

def ci_yun_threekingdoms_chinamap():
	mk = imageio.imread("chinamap.png")

	# 构建并配置词云对象w，注意要加stopwords集合参数，将不想展示在词云中的词放在stopwords集合里，这里去掉“曹操”和“孔明”两个词
	w = wordcloud.WordCloud(width=1000,
	                        height=700,
	                        background_color='white',
	                        font_path='msyh.ttf',
	                        mask=mk,
	                        scale=15,
	                        stopwords={'曹操','孔明'})

	# 对来自外部文件的文本进行中文分词，得到string
	f = open('三国演义.txt',encoding='utf-8')
	txt = f.read()
	txtlist = jieba.lcut(txt)
	string = " ".join(txtlist)

	# 将string变量传入w的generate()方法，给词云输入文字
	w.generate(string)

	# 将词云图片导出到当前文件夹
	w.to_file('threekingdoms.png')

def ci_yun_recolor():
	text = open('alice.txt').read()
	mk = imageio.imread("alice_color.png")

	# 构建词云对象w
	wc = WordCloud(background_color="white",
	               mask=mk,)
	# 将text字符串变量传入w的generate()方法，给词云输入文字
	wc.generate(text)

	# 调用wordcloud库中的ImageColorGenerator()函数，提取模板图片各部分的颜色
	image_colors = ImageColorGenerator(mk)

	# 显示原生词云图、按模板图片颜色的词云图和模板图片，按左、中、右显示
	fig, axes = plt.subplots(1, 3)
	# 最左边的图片显示原生词云图
	axes[0].imshow(wc)
	# 中间的图片显示按模板图片颜色生成的词云图，采用双线性插值的方法显示颜色
	axes[1].imshow(wc.recolor(color_func=image_colors), interpolation="bilinear")
	# 右边的图片显示模板图片
	axes[2].imshow(mk, cmap=plt.cm.gray)
	for ax in axes:
	    ax.set_axis_off()
	plt.show()

	# 给词云对象按模板图片的颜色重新上色
	wc_color = wc.recolor(color_func=image_colors)
	# 将词云图片导出到当前文件夹
	wc_color.to_file('output10-alice.png')

def ci_yun_recolor_offical_example():
    #!/usr/bin/env python
    """
    Image-colored wordcloud
    =======================

    You can color a word-cloud by using an image-based coloring strategy
    implemented in ImageColorGenerator. It uses the average color of the region
    occupied by the word in a source image. You can combine this with masking -
    pure-white will be interpreted as 'don't occupy' by the WordCloud object when
    passed as mask.
    If you want white as a legal color, you can just pass a different image to
    "mask", but make sure the image shapes line up.
    """

    from os import path
    from PIL import Image
    import numpy as np
    import matplotlib.pyplot as plt
    import os

    from wordcloud import WordCloud, STOPWORDS, ImageColorGenerator

    # get data directory (using getcwd() is needed to support running example in generated IPython notebook)
    d = path.dirname(__file__) if "__file__" in locals() else os.getcwd()

    # Read the whole text.
    text = open(path.join(d, 'alice.txt')).read()

    # read the mask / color image taken from
    # http://jirkavinse.deviantart.com/art/quot-Real-Life-quot-Alice-282261010
    alice_coloring = np.array(Image.open(path.join(d, "alice_color.png")))
    stopwords = set(STOPWORDS)
    stopwords.add("said")

    wc = WordCloud(background_color="white", max_words=2000, mask=alice_coloring,
                   stopwords=stopwords, max_font_size=40, random_state=42)
    # generate word cloud
    wc.generate(text)

    # create coloring from image
    image_colors = ImageColorGenerator(alice_coloring)

    # show
    fig, axes = plt.subplots(1, 3)
    axes[0].imshow(wc, interpolation="bilinear")
    # recolor wordcloud and show
    # we could also give color_func=image_colors directly in the constructor
    axes[1].imshow(wc.recolor(color_func=image_colors), interpolation="bilinear")
    axes[2].imshow(alice_coloring, cmap=plt.cm.gray, interpolation="bilinear")
    for ax in axes:
        ax.set_axis_off()
    plt.show()

if __name__ == "__main__":
	# ci_yun()
	# ci_yun_village()
	# ci_yun_village_wujiaoxing()
	ci_yun_threekingdoms_chinamap()
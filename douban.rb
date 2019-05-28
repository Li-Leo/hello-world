#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from bs4 import BeautifulSoup
import re

soup = BeautifulSoup(open("douban.html",encoding='utf-8'), "html.parser", from_encoding="utf-8")#
L = soup.find_all("a", href=re.compile("\?start=(\d+).*filter="))
soup1 = soup.find_all('a')#, class_="title"

L = L[:-2]
# print(L)

# for a in L:
#     print(a['href'])
# print(soup1)
for a in soup1:
    print(a)
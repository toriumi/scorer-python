#http://www.books.or.jp/ResultList.aspx?searchtype=1&isbn=9784048926102&showcount=20&startindex=0

# -*- coding: utf-8 -*- 
import sys
import urllib.request
from bs4 import BeautifulSoup
print(sys.getdefaultencoding())
print(sys.stdout.encoding)
url = "http://www.books.or.jp/ResultList.aspx?searchtype=1&isbn=9784048926102&showcount=20&startindex=0"
#url = "http://www.google.co.jp"
html = urllib.request.urlopen(url).read()
soup = BeautifulSoup(html, "lxml")
soup.find_all('a')
for td in soup.find_all('td'):
    print(td.decode("ANSI_X3.4-1968").encode("utf-8")) 
content = soup.table
print(content)

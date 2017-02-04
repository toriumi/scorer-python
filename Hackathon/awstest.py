#https://runble1.com/python-amazon-product-adverising-api/ sudo apt-get install libxml2-dev libxslt-dev 
"""
import bottlenose
from bs4 import BeautifulSoup
  
ACCESS_KEY = ""
SECRET_ACCESS_KEY = ""
ASSOCIATE_TAG = "clonet-22"
 
amazon = bottlenose.Amazon(ACCESS_KEY, SECRET_ACCESS_KEY, ASSOCIATE_TAG, Region="JP")
response = amazon.ItemLookup(ItemId="4774142298", ResponseGroup="ItemAttributes",SearchIndex="Books", IdType="ISBN")
soup = BeautifulSoup(response,"lxml")
 
# 商品情報の部分のみ表示
print(soup.find('item').prettify())

"""
from amazon.api import AmazonAPI
 
ACCESS_KEY = ""
SECRET_ACCESS_KEY = ""
ASSOCIATE_TAG = "****"
 
amazon = AmazonAPI(ACCESS_KEY, SECRET_ACCESS_KEY, ASSOCIATE_TAG, region="JP")
try:
    product = amazon.lookup(ItemId="4839947597")
    print (product.title)
    print (product.price_and_currency)
except Exception as e:
    print (e)

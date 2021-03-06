#http://ytkyk.info/blog/2016/06/19/raspberry-pi%E3%81%A7128x64%E3%81%AEoled%E3%81%AB%E6%97%A5%E6%9C%AC%E8%AA%9E%E3%82%92%E8%A1%A8%E7%A4%BA%E7%BE%8E%E5%92%B2%E3%83%95%E3%82%A9%E3%83%B3%E3%83%88/
# -*- coding: utf-8 -*-

import Adafruit_GPIO.SPI as SPI
import Adafruit_SSD1306
 
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

# Raspberry Pi pin configuration
RST = 24

disp = Adafruit_SSD1306.SSD1306_128_64(rst=RST)

# Initialize library.
disp.begin()
 
# Clear display.
disp.clear()
disp.display()
 
# Create blank image for drawing.
# Make sure to create image with mode '1' for 1-bit color.
width = disp.width
height = disp.height
image = Image.new('1', (width, height))
 
# Get drawing object to draw on image.
draw = ImageDraw.Draw(image)
 
# Draw a black filled box to clear the image.
draw.rectangle((0,0,width,height), outline=0, fill=0)

# Misaki Font, awesome 8x8 pixel Japanese font, can be downloaded from the following URL.
# $ wget http://www.geocities.jp/littlimi/arc/misaki/misaki_ttf_2015-04-10.zip
font = ImageFont.truetype('font/misakifont/misaki_gothic.ttf', 16, encoding='unic')

# Un-comment out the following line if you want to use the default font instead of Misaki Font
# font = ImageFont.load_default()

# Write two lines of text.
x=0
y=0
for str in [ u'Future Standard', u'フューチャー', u'スタンダード', u'本郷' ]:
	draw.text((x,y), str, font=font, fill=255)
	y+=16

disp.image(image)
disp.display()
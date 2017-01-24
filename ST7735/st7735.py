#!/usr/bin/env python
# -*- coding: utf-8 -*-
 
import RPi.GPIO as GPIO
import spidev           # sudo pip install spidev
import time
import sys
from PIL import Image   # sudo pip install pillow
from PIL import ImageDraw
from PIL import ImageFont
import json
import urllib3
import urllib
import io
 
RSPort = 24
RstPort = 25
 
class ST7735:
    def __init__(self, rst, rs):
        self.spi = spidev.SpiDev()
        self.spi.open(0, 0)
        self.spi.max_speed_hz = 16000000
        self.rst = rst
        self.rs = rs
        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)
        GPIO.setup(self.rs, GPIO.OUT)
        GPIO.setup(self.rst, GPIO.OUT)
        self.reset()
        self.write_cmd(0x11)
        time.sleep(0.12)
        self.write((0xB1, 0x01, 0x2C, 0x2D))
        self.write((0xB2, 0x01, 0x2C, 0x2D))
        self.write((0xB3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D))
        self.write((0xB4, 0x07))
        self.write((0xC0, 0xA2, 0x02, 0x84))
        self.write((0xC1, 0xC5))
        self.write((0xC2, 0x0A, 0x00))
        self.write((0xC3, 0x8A, 0x2A))
        self.write((0xC4, 0x8A, 0xEE))
        self.write((0xC5, 0x0E))
        self.write((0x36, 0xC8))
        self.write((0xE0, 0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10))
        self.write((0xE1, 0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10))
        self.write((0x2A, 0x00, 0x02, 0x00, 0x81))
        self.write((0x2B, 0x00, 0x01, 0x00, 0xA0))
        self.write((0x3A, 0x05))
        self.write_cmd(0x29)
    def reset(self):
        GPIO.output(self.rst, False)
        time.sleep(0.1)
        GPIO.output(self.rst, True)
        time.sleep(0.1)
    def write_cmd(self, cmd):
        GPIO.output(self.rs, False)  # RS=0
        self.spi.xfer2([cmd])
    def write_data(self, data):
        GPIO.output(self.rs, True)   # RS=1
        self.spi.xfer2([data])
    def write(self, cmd):
        if len(cmd) == 0:
            return
        GPIO.output(self.rs, False)  # RS=0
        self.spi.xfer2([cmd[0]])
        GPIO.output(self.rs, True)   # RS=1
        self.spi.xfer2(list(cmd[1:]))
    def write_rgb(self, r, g, b):
        self.write_data(r & 0xF8 | g >> 5)
        self.write_data(g & 0xFC << 3 | b >> 3)
    def fill(self, r, g, b):
        self.write((0x2A, 0x00, 0x02, 0x00, 0x81))
        self.write((0x2B, 0x00, 0x01, 0x00, 0xA0))
        self.write_cmd(0x2C)
        GPIO.output(self.rs, True)   # RS=1
        hi = r & 0xF8 | g >> 5
        lo = g & 0xFC << 3 | b >> 3
        pixline = []
        for n in range(2048):
            pixline.append(hi)        
            pixline.append(lo)
        for n in range(10):
            self.spi.xfer2(pixline[0:])
    def image(self, imgfile):
        try:
            im = Image.open(imgfile).convert("RGB")
            im.thumbnail((128, 160))
            self.trans(im)
        except:
            pass
    def trans(self, im):
        pix = im.load()
        w = im.size[0]
        h = im.size[1]
        if w > 128:
            w = 128
        if h > 160:
            h = 160
        self.write((0x2A, 0x00, 0x02, 0x00, w + 1))
        self.write((0x2B, 0x00, 0x01, 0x00, h))
        self.write_cmd(0x2C)
        GPIO.output(self.rs, True)   # RS=1
        pixline = []
        px = 0
        py = 0
        for i in range(10):
            for n in range(2048):
                pixel = pix[px, py]
                hi = pixel[0] & 0xF8 | pixel[1] >> 5
                lo = pixel[1] & 0xFC << 3 | pixel[2] >> 3
                pixline.extend([hi, lo])
                px = px + 1
                if px >= w:
                    px = 0
                    py = py + 1
                    if py >= h:
                        break
            self.spi.xfer2(pixline)
            del pixline[:]
            if py >= h:
                break
    def blit(self, im, x1, y1, x2, y2):
        if x2 < x1:
            x1, x2 = x2, x1
        if y2 < y1:
            y1, y2 = y2, y1
        if x1 > 127 or x2 < 0:
            return
        if x1 < 0:
            x1 = 0
        if x2 > 127:
            x2 = 127
        if y1 > 159 or y2 < 0:
            return
        if y1 < 0:
            y1 = 0
        if y2 > 159:
            y2 = 159
        pix = im.load()
        self.write((0x2A, 0x00, x1 + 2, 0x00, x2 + 2))
        self.write((0x2B, 0x00, y1 + 1, 0x00, y2 + 1))
        self.write_cmd(0x2C)
        GPIO.output(self.rs, True)   # RS=1
        pixline = []
        px = x1
        py = y1
        for i in range(10):
            for n in range(2048):
                pixel = pix[px, py]
                hi = pixel[0] & 0xF8 | pixel[1] >> 5
                lo = pixel[1] & 0xFC << 3 | pixel[2] >> 3
                pixline.extend([hi, lo])
                px = px + 1
                if px > x2:
                    px = x1
                    py = py + 1
                    if py > y2:
                        break
            self.spi.xfer2(pixline)
            del pixline[:]
            if py > y2:
                break
 
class Weather:
    def __init__(self):
        self.http = urllib3.PoolManager()
    def get_data(self, location):
        try:
            url = 'http://weather.livedoor.com/forecast/webservice/json/v1?city={0:s}'.format(location)
            resp = self.http.request('GET', url)
            data = json.loads(resp.data.decode('utf-8'))
        except:
            data = []   # err
        return data
 
class Forecast:
    def __init__(self):
        self.weather = Weather()
        self.lcd = ST7735(RstPort, RSPort)
        self.im = Image.new('RGBA', (128, 160), (0, 0, 0))
        self.draw = ImageDraw.Draw(self.im)
        self.font = ImageFont.truetype("/usr/share/fonts/truetype/kochi/kochi-gothic.ttf", 18, encoding='unic')
    def show(self, location):
        data = self.weather.get_data(location)
        if len(data) > 0:
            self.draw.rectangle([0, 0, 127, 20], fill=(0x80, 0x00, 0x40))
            self.draw.rectangle([0, 21, 127, 159], fill=(0x40, 0x00, 0x80))
            str = u'{0:s}:{1:s}'.format(
                    data['location']['area'],
                    data['location']['city'])
            self.draw.text([2, 2], str, font=self.font, fill=(0xFF, 0xFF, 0xFF))
            for n in range(2):
                y = n * 65 + 5
                self.draw.text([54, y + 20], data['forecasts'][n + 1]['dateLabel'],
                    font=self.font, fill=(0xFF, 0xFF, 0x00))
                self.draw.text([54, y + 40], data['forecasts'][n + 1]['telop'],
                    font=self.font, fill=(0x00, 0xFF, 0x00))
                try:
                    icon_url = io.StringIO(
                                urllib.urlopen(data['forecasts'][n + 1]['image']['url']).read())
                    icon_w = data['forecasts'][n + 1]['image']['width']
                    icon_h = data['forecasts'][n + 1]['image']['height']
                    icon_im = Image.open(icon_url).convert("RGB")
                    self.im.paste(icon_im, (4, y + 22))
                except:
                    pass
                try:
                    t_min = data['forecasts'][n + 1]['temperature']['min']['celsius']
                    t_max = data['forecasts'][n + 1]['temperature']['max']['celsius']
                    str = u'{0:s}℃～{1:s}℃'.format(t_min, t_max)
                except:
                    str = u'xx℃～xx℃'.format(t_min, t_max)
                self.draw.text([20, y + 60], str, font=self.font, fill=(0x00, 0xFF, 0xFF))
            self.draw.rectangle([0, 20, 127, 22], fill=(0x40, 0x00, 0x00))
            self.draw.rectangle([0, 85, 127, 87], fill=(0x40, 0x00, 0x00))
            self.lcd.trans(self.im)
 
if __name__ == "__main__":
    fst = Forecast()
    try:
        while True:
            print('\n002_')
            fst.show('016010')  # 札幌
            time.sleep(10)    
            fst.show('040010')  # 仙台
            time.sleep(10)    
            fst.show('130010')  # 東京
            time.sleep(10)
            fst.show('230010')  # 名古屋
            time.sleep(10)
            fst.show('270000')  # 大阪
            time.sleep(10)
            fst.show('400010')  # 福岡
            time.sleep(10)
            fst.show('471010')  # 那覇
            time.sleep(10)
            
    except KeyboardInterrupt:
        print('\nbreak')
    GPIO.cleanup()
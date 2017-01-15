#!/usr/bin/python
# -*- coding: utf-8 -*-
import cv2
import zbar
import PIL.Image


#initialize QR_Code
scanner = zbar.ImageScanner()
scanner.parse_config('enable')



img = cv2.imread("sdkqr.jpg", 1)

#grayscale
img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

#Binarization
tresh = 100
max_pixel = 255
ret, img = cv2.threshold(img, tresh, max_pixel, cv2.THRESH_BINARY)

#picture change PIL
pil_img = PIL.Image.fromarray(img)
width, height = pil_img.size
raw = pil_img.tostring()
image = zbar.Image(width, height, 'Y800', raw)

#result
scanner.scan(image)
for symbol in image:
    print(symbol.data)




#https://www.blog.umentu.work/python3-opencv3%E3%82%92%E4%BD%BF%E3%81%A3%E3%81%A6%E7%89%A9%E4%BD%93%E6%A4%9C%E5%87%BA/


import zmq
import cv2
import sys
from scorer import Scorer

print("Waiting Start...")

###
### Scorer Python SDK Test for Web
###


scorer=Scorer("Task")

fgbg = cv2.createBackgroundSubtractorMOG2(50,16,0)

average_square = (25, 25)
sigma_x = 1
cv2.namedWindow('test')
while True:

    if scorer.poll() == False:
        continue

    frame = scorer.get_frame()
    if frame == None:
       continue

    bgr = frame.get_bgr()
    gray = frame.get_gray()
    cv2.imshow("test",gray)
    
    #gray = cv2.GaussianBlur(gray, average_square, sigma_x)
    thresh = 220
    max_pixel = 255
    ret, gray = cv2.threshold(gray, thresh, max_pixel, cv2.THRESH_BINARY)
    #gray = fgbg.apply(gray)
    #gray = cv2.adaptiveThreshold(gray,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,13,9)
    #cv2.imshow("test",gray)
    #scorer.web_show(gray, 1)
    #scorer.web_show(bgr, 2)


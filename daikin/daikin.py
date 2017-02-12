#https://www.blog.umentu.work/python3-opencv3%E3%82%92%E4%BD%BF%E3%81%A3%E3%81%A6%E7%89%A9%E4%BD%93%E6%A4%9C%E5%87%BA/


import zmq
import cv2
import sys
import scorer

print("Waiting Start...")

###
### Scorer Python SDK Test for Web
###

cap = scorer.VideoCapture(0)
fgbg = cv2.createBackgroundSubtractorMOG2(100,16,0)
average_square = (25, 25)
sigma_x = 1
maxarea=1000
minarea=15
while True:

    frame = cap.read()
    if frame == None:
       continue

    bgr = frame.get_bgr()
    gray = frame.get_gray()
    """
    #gray = cv2.GaussianBlur(gray, average_square, sigma_x)
    gray = cv2.GaussianBlur(gray, average_square, sigma_x)
    thresh = 232
    max_pixel = 255
    #gray = cv2.adaptiveThreshold(gray,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,15,7)
    ret, gray = cv2.threshold(gray, thresh, max_pixel, cv2.THRESH_BINARY)
    gray = fgbg.apply(gray)
    im1, contours1, hierarchy1 = cv2.findContours(gray,cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    for i, cnt in enumerate(contours1):
        x,y,w,h = cv2.boundingRect(cnt)
        if w*h > maxarea:
            continue
        elif w*h > minarea:
            cv2.rectangle(bgr,(x,y),(x+w,y+h),(0,255,0),2)
    """

    
    #cv2.imshow("test",gray)
    #scorer.web_show(gray, 1)
    #scorer.web_show(bgr, 2)
    cv2.imshow("org",bgr)
    cv2.imshow("test",gray)
    k = cv2.waitKey(1) # 1msec待つ
    if k == 27: # ESCキーで終了
        break
    


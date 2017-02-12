#https://www.blog.umentu.work/python3-opencv3%E3%82%92%E4%BD%BF%E3%81%A3%E3%81%A6%E7%89%A9%E4%BD%93%E6%A4%9C%E5%87%BA/


import zmq
import cv2
import sys
from datetime import datetime
import scorer
# import module for ROIStrage
sys.path.append("/opt/scorer/home/dev/scorer-sdk-python/samples/lib")
import scorer_util

print("Waiting Start...")

###
### Scorer Python SDK Test for Web
###

cap = scorer.VideoCapture(0)
# Read ROI data
roi = scorer_util.ROIStorage()
x1,y1,x2,y2 = roi.get_roi_rect_by_index(0)
#fgbg = cv2.createBackgroundSubtractorMOG2(50,15,0)

average_square = (25, 25)
sigma_x = 1

while True:

    frame = cap.read()
    if frame == None:
       continue

    bgr = frame.get_bgr()
    gray = frame.get_gray()
    cv2.imshow("test",gray)
    gray=gray[y1:y2,x1:x2]
    #gray = cv2.GaussianBlur(gray, average_square, sigma_x)
    thresh = 210
    max_pixel = 255
    ret, gray = cv2.threshold(gray, thresh, max_pixel, cv2.THRESH_BINARY)
    
    
    #gray = fgbg.apply(gray)
    #gray = cv2.adaptiveThreshold(gray,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,13,9)
    #print("x="+str((x1-x2)/2)+"y="+str((y1-y2)/2))
    nLabels, labelImage, contours, GoCs = cv2.connectedComponentsWithStats(gray)
    for j in range(1, nLabels):
        x,y = GoCs[j]
        print("unixtime = "+str(datetime.now().strftime('%s'))+" id = "+str(j)+" x = "+str(x)+" y = "+str(y))
    cv2.imshow("test2",gray)
    #scorer.web_show(gray, 1)
    #scorer.web_show(bgr, 2)
    k = cv2.waitKey(1) # 1msec待つ
    if k == 27: # ESCキーで終了
        break

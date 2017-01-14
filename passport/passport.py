import zmq
import cv2
import sys
import numpy as np
import time
from scorer import Scorer


count=0
print("Waiting Start...")

scorer=Scorer("Task")
args = sys.argv

thresh = 170
minarea = 600
maxarea = 1500

if len(args) > 3:
	thresh = int(args[1])
	minarea = int(args[2])
	maxarea = int(args[3])
elif len(args) >1:
	thresh = int(args[1])
	
img = cv2.imread("trimmed.jpg",1)
img = cv2.blur(img, ksize=(5,5))
bgr = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
#bgr = cv2.cvtColor(bgr, cv2.COLOR_GRAY2BGR)
binary_img4 = cv2.threshold(bgr, thresh, 255, cv2.THRESH_BINARY)
th3 = cv2.adaptiveThreshold(bgr,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,15,4)

"""
th3 = cv2.cvtColor(th3, cv2.COLOR_GRAY2BGR)
th3 = cv2.blur(th3, ksize=(5,5))
th3 = cv2.cvtColor(th3, cv2.COLOR_BGR2GRAY)て
"""
#binary_img4 = cv2.cvtColor(th3, cv2.COLOR_GRAY2BGR)
im4, contours4, hierarchy4 = cv2.findContours(th3,cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

countcnt=0;

roi_y,roi_x,roi_w,roi_h = -1,-1,-1,-1
th3 = cv2.cvtColor(th3, cv2.COLOR_GRAY2BGR)
for i, cnt4 in enumerate(contours4):
    x,y,w,h = cv2.boundingRect(cnt4)
    flag = "false"
    if w/h >0.7:
        if w/h<1.4:
            #cv2.rectangle(bgr,(x,y),(x+w,y+h),(0,255,0),2)
            if w*h > maxarea:
                continue
            elif w*h > minarea:
                cv2.rectangle(img,(x,y),(x+w,y+h),(0,255,0),2)	
                print("in  x ", x, " y ", y, " w ",  w, " h ", h)
                if roi_x == -1:
                    roi_x,roi_y,roi_w,roi_h = x,y,w,h
                else:
                    x2,y2,w2,h2 = roi_x,roi_y,roi_w,roi_h
                    roi_x = min([x, x2])
                    roi_y = min([y, y2])
                    roi_w = max([x+w, x2+w2]) - roi_x
                    roi_h = max([y+h, y2+h2]) - roi_y
                    print("out x ", roi_x, " y ", roi_y, " w ",  roi_w, " h ", roi_h)
            else:
                flag="true"
        elif w*h < minarea:
            flag="true"
    elif w*h < minarea:
        flag="true"
            
    if flag == "true":
        cv2.drawContours( th3, contours4, i, (255,255,255), -1)
        #x0,y0,w0,h0 = cv2.boundingRect(cnt4)
        #cv2.rectangle(th3,(x0,y0),(x0+w0,y0+h0),(255,255,255), -1)
                

cv2.rectangle(img,(roi_x,roi_y),(roi_x+roi_w,roi_y+roi_h),(0,127,255),3)

th3 = th3[roi_y:roi_y+roi_h, roi_x:roi_x+roi_w]
#th3 = cv2.resize(th3,(int(roi_w/2),int(roi_h/2)))
cv2.imwrite("result.jpg",img)
cv2.imwrite("binary.jpg",th3)

print("Ended")
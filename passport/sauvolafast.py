#http://schima.hatenablog.com/entry/2013/10/25/202418
import zmq
import cv2
import sys
import numpy as np
import time
import math
from scorer import Scorer


count=0
print("Waiting Start...")

scorer=Scorer("Task")
args = sys.argv

thresh = 170
minarea = 600
maxarea = 5000

if len(args) > 3:
	thresh = int(args[1])
	minarea = int(args[2])
	maxarea = int(args[3])
elif len(args) >1:
	thresh = int(args[1])
	
img = cv2.imread("trimmed.jpg",1)
bgr = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
#bgr = cv2.cvtColor(bgr, cv2.COLOR_GRAY2BGR)
binary_img4 = cv2.threshold(bgr, thresh, 255, cv2.THRESH_BINARY)
th3 = cv2.adaptiveThreshold(bgr,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,13,7)

#binary_img4 = cv2.cvtColor(th3, cv2.COLOR_GRAY2BGR)
im4, contours4, hierarchy4 = cv2.findContours(th3,cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

countcnt=0;

roi_y,roi_x,roi_w,roi_h = -1,-1,-1,-1

for h, cnt4 in enumerate(contours4):
    x,y,w,h = cv2.boundingRect(cnt4)
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
#gray = gray[roi_y:roi_y+roi_h, roi_x:roi_x+roi_w]
#gray = cv2.resize(gray,(int(roi_w/2),int(roi_h/2)))


kernelSize = 11
k = 0.15
r = 50
height, width = gray.shape[:2]
dst = gray.copy()
borderSize = int(kernelSize / 2 + 1)
kernelPixels = int(kernelSize * kernelSize)
srcWithBorder = cv2.copyMakeBorder(gray, borderSize, borderSize, borderSize, borderSize, cv2.BORDER_REPLICATE)
Sum, sqSum = cv2.integral2(srcWithBorder)
for y in range(0, height-1):
    print(str(y)+"-")
    for x in range(0, width-1):
        #print(str(y)+"-"+str(x))
        kx = int(x + kernelSize)
        ky = int(y + kernelSize)
        sumVal = Sum[ky, kx] - Sum[ky, x] - Sum[y, kx] + Sum[y, x]
        sqSumVal = sqSum[ky, kx] - sqSum[ky, x] - sqSum[y, kx] + sqSum[y, x]
        mean = sumVal / kernelPixels
        var = (sqSumVal / kernelPixels) - (mean * mean)
        if var < 0.0: 
            var = 0.0
        stddev = math.sqrt(var)
        threshold = mean * (1 + k * (stddev / r - 1))
        if gray[y, x] < threshold:
            dst[y, x] = 0
        else:
            dst[y, x] = 255


cv2.rectangle(img,(roi_x,roi_y),(roi_x+roi_w,roi_y+roi_h),(0,127,255),3)

cv2.imwrite("result.jpg",img)
cv2.imwrite("binary.jpg",th3)
cv2.imwrite("dst.jpg",dst)
cv2.imwrite("gray.jpg",gray)
print("Ended")
"""
void sauvolaFast(const cv::Mat &src, cv::Mat &dst, int kernelSize, double k, double r)
{
    dst.create(src.size(), src.type());

    cv::Mat srcWithBorder;
    int borderSize = kernelSize / 2 + 1;
    int kernelPixels = kernelSize * kernelSize;
    cv::copyMakeBorder(src, srcWithBorder, borderSize, borderSize, 
                       borderSize, borderSize, cv::BORDER_REPLICATE);

    cv::Mat sum, sqSum;
    cv::integral(srcWithBorder, sum, sqSum);	
    for(int y = 0; y < src.rows; y++)
    {
        for(int x = 0; x < src.cols; x++)
        {
            int kx = x + kernelSize;
            int ky = y + kernelSize;
            double sumVal = sum.at<int>(ky, kx)
                          - sum.at<int>(ky, x)
                          - sum.at<int>(y, kx)
                          + sum.at<int>(y, x);
            double sqSumVal = sqSum.at<double>(ky, kx)
                            - sqSum.at<double>(ky, x)
                            - sqSum.at<double>(y, kx)
                            + sqSum.at<double>(y, x);

            double mean = sumVal / kernelPixels;
            double var = (sqSumVal / kernelPixels) - (mean * mean);
            if (var < 0.0) 
                var = 0.0;
            double stddev = sqrt(var);
            double threshold = mean * (1 + k * (stddev / r - 1));

            if (src.at<uchar>(y, x) < threshold)
                dst.at<uchar>(y, x) = 0;
            else
                dst.at<uchar>(y, x) = 255;
        }
    }
}
"""
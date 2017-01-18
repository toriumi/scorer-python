# tebure hosei http://www.learnopencv.com/image-alignment-ecc-in-opencv-c-python/
# http://tokyo.supersoftware.co.jp/code/3242
#https://github.com/atinfinity/lab/wiki/OpenCV%E3%82%92%E4%BD%BF%E3%81%A3%E3%81%9F%E3%83%A9%E3%83%99%E3%83%AA%E3%83%B3%E3%82%B0
# -*- coding: UTF-8 -*- 

import cv2
import math 
import numpy as np 
import os
center = [120,120]
rangeval = [100, 1000]
if __name__ == '__main__':
    
    # 画像の読み込み
    img_src1 = cv2.imread("meter_0.png", 0)
    img_src2 = cv2.imread("meter_60.png", 0)
    height, width = img_src1.shape[:2]
    # 背景画像との差分を算出
    img_diff = cv2.absdiff(img_src2, img_src1)
    img_src1 = cv2.cvtColor(img_src1, cv2.COLOR_GRAY2BGR)

    # 差分を二値化
    img_diffm = cv2.threshold(img_diff, 20, 255, cv2.THRESH_BINARY)[1]
    
    
    # 膨張処理、収縮処理を施してマスク画像を生成
    operator = np.ones((3, 3), np.uint8)
    img_dilate = cv2.dilate(img_diffm, operator, iterations=4)
    img_mask = cv2.erode(img_dilate, operator, iterations=4)

    # マスク画像を使って対象を切り出す
    img_dst = cv2.bitwise_and(img_src2, img_mask)
    
    img_diffm = cv2.circle(img_diffm, (center[0],center[1]),20, (0,0,0), -1)
    nLabels, labelImage, contours, GoCs = cv2.connectedComponentsWithStats(img_diffm)
    colors = []
    dst = cv2.cvtColor(img_diff, cv2.COLOR_GRAY2BGR)
    for i in range(1, nLabels + 1):
        colors.append(np.array([np.random.randint(0, 255), np.random.randint(0, 255), np.random.randint(0, 255)]))

    for y in range(0, height):
        for x in range(0, width):
            if labelImage[y, x] > 0:
                dst[y, x] = colors[labelImage[y, x]]
            else:
                dst[y, x] = [0, 0, 0]
    thetadiff=[180,-180]
    for j in range(1, nLabels + 1):
        x,y,w,h,size = contours[j-1]
        if rangeval[0] < size < rangeval[1]:
            x,y = GoCs[j-1]
            theta = int(math.atan2(int(x)-center[0],-int(y)+center[1])*180/math.pi)
            if theta > thetadiff[1]:
                thetadiff[1]=theta
            if theta < thetadiff[0]:
                thetadiff[0]=theta
            print("theta = "+str(theta))
            dst = cv2.circle(dst, (int(x),int(y)), 2, (0,0,255), -1)
            img_src1=cv2.line(img_src1, (center[0],center[1]),(int(x),int(y)), (255,0,0), 5)

    # 表示
    print("diff = "+str(thetadiff[1]-thetadiff[0]))
    cv2.imwrite("imgdist.png", dst)
    cv2.imwrite("imgorg1.png", img_src1)
    cv2.imwrite("result.png", img_diffm)
    cv2.imwrite("resultdiff.png", img_diff)
   
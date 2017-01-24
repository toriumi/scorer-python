# -*- coding: utf-8 -*-

import cv2
import numpy as np
import LPRecogLib3 as LPRecogLib

lpr = LPRecogLib.LPRecog()
ret = lpr.initialize("../Conf/cascade_lbp.xml", "../Conf/plate_shape.dat",\
                     "../Conf/local_name_filter.txt", "../Conf/local_name_id.csv",\
                     "../Conf/class_number_filter.txt", "../Conf/class_number_id.csv",\
                     "../Conf/hiragana_filter.txt", "../Conf/hiragana_id.csv",\
                     "../Conf/car_number_filter.txt", "../Conf/car_number_id.csv")

if(ret < 0):
    print("Fail to initialize")
else:
    img = cv2.imread("test.JPG", 0)
    npimg = np.asarray(img)
    results = lpr.Recog(npimg, 500, 1000, 1, 0.2)
    for plate in results:
        print(plate[0])
        print(plate[1])
        print(plate[2])
        print(plate[3])
        print(plate[4])
        print('---')
    print("Done")


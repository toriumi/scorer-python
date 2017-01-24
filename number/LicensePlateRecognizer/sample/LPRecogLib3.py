# -*- coding: utf-8 -*-

import ctypes
import numpy as np
import cv2

class LPRecog:
    u"""
    ナンバープレート認識プログラム
    """
    __lpr_class = ctypes.c_void_p
    __lpr_dll = ctypes.cdll.LoadLibrary('/opt/scorer/home/dev/scorer-python/number/LicensePlateRecognizer/src/libLPRecogLib.so')
    __lpr_dll.new_LPRecog.restype = ctypes.c_void_p
    #__lpr_dll.new_LPRecog.restype = ctypes.POINTER(ctypes.c_long)
    __lpr_dll.GetLocalName.restype = ctypes.c_char_p
    __lpr_dll.GetCarNumber.restype = ctypes.c_char_p
    __lpr_dll.GetHiragana.restype = ctypes.c_char_p
    __lpr_dll.GetClassNumber.restype = ctypes.c_char_p
    #__lpr_dll.GetPlateCorners.restype = ctypes.POINTER(ctypes.c_float * 8)

    def __init__(self):
        self.__lpr_class = self.__lpr_dll.new_LPRecog()

    def __del__(self):
        self.__lpr_class = self.__lpr_dll.delete_LPRecog(self.__lpr_class)


    def initialize(self, detector_file, shape_file, local_name, local_name_id,\
                   class_number, class_number_id, hiragana, hiragana_id,\
                   car_number, car_number_id):
        u"""
        初期化ファイルの設定

        detector_file = プレート検出器の学習済みファイル
        shape_file = プレート形状推定器の学習済みファイル
        local_name = 地名認識の学習済みファイル
        local_name_id = IDと地名の紐づけ用CSVファイル
        class_number = 分類番号の学習済みファイル
        class_number_id = IDと分類番号の紐づけ用CSVファイル
        hiragana = ひらがな認識の学習済みファイル
        hiragana_id = IDとひらがなの紐づけ用CSVファイル
        car_number = 車両番号の学習済みファイル
        car_number_id = IDと車両番号の紐づけ用CSVファイル
        """
        return self.__lpr_dll.initialize(self.__lpr_class, ctypes.c_char_p(detector_file.encode()),\
                                  ctypes.c_char_p(shape_file.encode()), ctypes.c_char_p(local_name.encode()),\
                                  ctypes.c_char_p(local_name_id.encode()), ctypes.c_char_p(class_number.encode()),\
                                  ctypes.c_char_p(class_number_id.encode()), ctypes.c_char_p(hiragana.encode()),\
                                  ctypes.c_char_p(hiragana_id.encode()), ctypes.c_char_p(car_number.encode()),\
                                  ctypes.c_char_p(car_number_id.encode()))


    def Recog(self, image, min_plate_width, max_plate_width, num_plates, threshold):
        u"""
        画像内のナンバープレートを認識

        image = 画像(numpy array）
        min_plate_width = 最小検出プレート幅(pixel)
        max_plate_width = 最大検出プレート幅(pixel)
        num_plates = 最大検出プレート数
        threshold = 閾値
        """
        if image.dtype != 'uint8':
            print("Error: image dtype must be 'uint8'")
            return
        img_type = None
        if image.ndim == 2:
            img_type = cv2.CV_8UC1
        elif image.ndim == 3 and image.shape[2] == 3 :
            img_type = cv2.CV_8UC3
        else:
            print("Error: image channel must be 1 or 3")
            return
        image_p = image.ctypes.data_as(ctypes.POINTER(ctypes.c_int8))
        plate_infos = self.__lpr_dll.Recog(self.__lpr_class, image_p, ctypes.c_int(image.shape[1]), ctypes.c_int(image.shape[0]), ctypes.c_int(img_type), \
                                           ctypes.c_int(min_plate_width), ctypes.c_int(max_plate_width), ctypes.c_int(num_plates), ctypes.c_float(threshold))
        num_detected = self.__lpr_dll.NumDetected(plate_infos)
        results = []
        for i in range(num_detected):
            plate_info_array = []
            local_name = self.__lpr_dll.GetLocalName(plate_infos, ctypes.c_int(i))
            plate_info_array.append(str(local_name, 'shift_jis'))

            class_number = self.__lpr_dll.GetClassNumber(plate_infos, ctypes.c_int(i))
            plate_info_array.append(str(class_number, 'shift_jis').replace('\r',''))

            hiragana = self.__lpr_dll.GetHiragana(plate_infos, ctypes.c_int(i))
            plate_info_array.append(str(hiragana, 'shift_jis'))

            car_number = self.__lpr_dll.GetCarNumber(plate_infos, ctypes.c_int(i))
            plate_info_array.append(str(car_number, 'shift_jis').replace('\r',''))

            plate_corners = (ctypes.c_float * 8)()
            plate_corners_p = ctypes.cast(plate_corners, ctypes.POINTER(ctypes.c_float * 8))
            self.__lpr_dll.GetPlateCorners(plate_infos, ctypes.c_int(i), plate_corners_p)
            #print isinstance(plate_corners[0], float)
            #print [plate_corners[n] for n in range(8)]
            plate_corner_array = np.array(plate_corners)
            plate_corner_array = plate_corner_array.reshape(4,2)
            #print plate_corner_array
            plate_info_array.append(plate_corner_array)

            results.append(plate_info_array)
            
        self.__lpr_dll.delete_PlateInfos(plate_infos)
        return results
        

import zbarimg
import cv2

testpic = cv2.imread('sdkqrmulti.jpg', cv2.IMREAD_GRAYSCALE)
bartype, bardata = zbarimg.passzbar(testpic)
print(bardata[2].decode('utf-8'))
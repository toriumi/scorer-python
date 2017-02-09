"""
 zbar.py

 Copyright (c) 2017 Future Standard Co., Ltd.

 This software is released under the MIT License.
 http://opensource.org/licenses/mit-license.php
"""
#import scorer
from scorer import Scorer
import cv2
import subprocess
from time import sleep
import pycurl

def read_barcode(image):
    cv2.imwrite("tmp.jpg", image)
    proc = subprocess.Popen("./readAR intrinsics.yml tmp.jpg", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    dataline = proc.stdout.readline()
    dataline = proc.stdout.readline()

    dataset=[]
    while True:
        dataline = proc.stdout.readline()
        if not dataline and proc.poll() is not None:
            break

        dataset.append(dataline)
    
    return dataset



# Setup VideoCaputre Object
scorer=Scorer("Task")
print("waiting...")
while True:
    if scorer.poll() == False:
       continue
    # Read Frame from Camera
    frame = scorer.get_frame()
    if frame == None:
       continue

    # Convert the Frame to GRAY
    gray = frame.get_gray()

    # Show camera image to web 
    

    
    # Read Barcode
    bardata = read_barcode(gray)
    
    barcodelist=""
    # Print the result
    for data in bardata:
        barcodelist = barcodelist + "-" + data.decode('utf-8')
        print(data.decode('utf-8'))
    
    if barcodelist!="":
        buffer=barcodelist
        curl = pycurl.Curl()
        curl.setopt(pycurl.URL, 'https://notify-api.line.me/api/notify')
        curl.setopt(pycurl.HTTPHEADER, ['Authorization: Bearer AhfffLL3f1BYUJMnBnV8HZFAuvFS3ztJ65ZQv9UPo5j'])
        #curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8')),('imageFile','@lena.png'),('stickerPackageId', '1'), ('stickerId', '112')])
        #curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8')),('imageFile', (curl.FORM_FILE, 'lena.png'))])
        curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8'))])
        curl.perform()

        sleep(10)

"""
# Setup VideoCaputre Object
cap = scorer.VideoCapture(0)
print("waiting...")
while True:
    # Read Frame from Camera
    frame = cap.read()
    if frame == None:
       continue

    # Convert the Frame to GRAY
    gray = frame.get_gray()

    # Show camera image to web 
    scorer.imshow(1, gray)

    
    # Read Barcode
    bartype, bardata = read_barcode(gray)
    
    barcodelist=""
    # Print the result
    for data in bardata:
        barcodelist = barcodelist + "-" + data.decode('utf-8')
        print(data.decode('utf-8'))
    
    if barcodelist!="":
        buffer=barcodelist
        curl = pycurl.Curl()
        curl.setopt(pycurl.URL, 'https://notify-api.line.me/api/notify')
        curl.setopt(pycurl.HTTPHEADER, ['Authorization: Bearer AhfffLL3f1BYUJMnBnV8HZFAuvFS3ztJ65ZQv9UPo5j'])
        #curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8')),('imageFile','@lena.png'),('stickerPackageId', '1'), ('stickerId', '112')])
        #curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8')),('imageFile', (curl.FORM_FILE, 'lena.png'))])
        curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8'))])
        curl.perform()

        sleep(10)
"""
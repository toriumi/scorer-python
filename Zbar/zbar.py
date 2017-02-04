"""
 zbar.py

 Copyright (c) 2017 Future Standard Co., Ltd.

 This software is released under the MIT License.
 http://opensource.org/licenses/mit-license.php
"""
import scorer
import cv2
import subprocess
from time import sleep
import pycurl

def read_barcode(image):
    # conbert gray image to bitmap
    retval, bmp = cv2.imencode('.bmp', image)
    if retval == False:
        print("Failed to write bitmap file")
        exit(1)

    # convert bmp date to bytes
    binbmp = bmp.tostring()

    args = [ 'zbarimg', ':-', '-q' ] 
    p = subprocess.Popen( args,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        shell=False
        )

    stdout, stderr = p.communicate(input=binbmp)
    if len(stderr) == 0:
        bindata = stdout
    else:
        print('ERR:\n' + stderr.decode('utf-8'))
        exit(1)

    datalines = bindata.splitlines()
    datatype=[]
    dataset=[]
    for dataline in datalines:
        try:
            type, data = dataline.split(b":", 1)
        except ValueError:
            continue
        datatype.append(type)
        dataset.append(data)
    return datatype, dataset


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

#http://nixeneko.hatenablog.com/entry/2016/01/25/023703
import numpy as np
import cv2
import subprocess


# zbar command
ZBARIMG = 'zbarimg'
    
def passzbar(image): 
    # convert to bmp binary so that zbar can handle it
    retval, buf = cv2.imencode('.bmp', image)
    if retval == False:
        raise ValueError('The Given image could not be converted to BMP binary data')
    # convert buf from numpy.ndarray to bytes
    binbmp = buf.tostring()
    optionargs = []
    
    args = [
        ZBARIMG,
        ':-', '-q'
    ] + optionargs
    
    p = subprocess.Popen(
        args,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        shell=False
        )
        
    stdout, stderr = p.communicate(input=binbmp)
    if len(stderr) == 0:
        bindata = stdout
    else:
        raise RuntimeError('ZBar threw error:\n' + stderr.decode('utf-8'))
    
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
    
def main():
    testpic = cv2.imread('sdkqrmulti.jpg', cv2.IMREAD_GRAYSCALE)
    bartype, bardata = passzbar(testpic)
    print(bardata[2].decode('utf-8'))

if __name__ == '__main__':
    main()
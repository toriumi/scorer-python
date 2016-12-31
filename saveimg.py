import cv2
import sys
import scorer_sdk.Scorer
import pycurl

def send2line_on():
    buffer='on.png saved'
    curl = pycurl.Curl()
    curl.setopt(pycurl.URL, 'https://notify-api.line.me/api/notify')
    curl.setopt(pycurl.HTTPHEADER, ['Authorization: Bearer AhfffLL3f1BYUJMnBnV8HZFAuvFS3ztJ65ZQv9UPo5j'])
    curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8')),('imageFile', (curl.FORM_FILE, '/opt/scorer/home/dev/SDK/roi/on.png'))])
    curl.perform()

def send2line_off():
    buffer='off.png saved'
    curl = pycurl.Curl()
    curl.setopt(pycurl.URL, 'https://notify-api.line.me/api/notify')
    curl.setopt(pycurl.HTTPHEADER, ['Authorization: Bearer AhfffLL3f1BYUJMnBnV8HZFAuvFS3ztJ65ZQv9UPo5j'])
    curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8')),('imageFile', (curl.FORM_FILE, '/opt/scorer/home/dev/SDK/roi/off.png'))])
    curl.perform()

args = sys.argv
scorer=scorer_sdk.Scorer.Scorer("Task")
x1,y1,x2,y2 = scorer.get_roi_rect_by_index(0)


if len(args) >1:
    if scorer.poll() == False:
        sys.exit()

    frame = scorer.get_frame()
    
    if frame == None:
        sys.exit()
    
    bgr = frame.get_bgr()
    rect = bgr[y1:y2, x1:x2]

    if args[1] == "on":
        #cv2.imwrite("/opt/scorer/home/dev/scorer-python/on.png",rect)
        cv2.imwrite("/opt/scorer/home/dev/SDK/roi/on.png",rect)
        send2line_on()
        print("on saved")
    
    elif args[1] == "off":
        cv2.imwrite("/opt/scorer/home/dev/SDK/roi/off.png",rect)
        send2line_off()
        print("off saved")


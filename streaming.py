import zmq
import cv2
import sys
import scorer_sdk.Scorer
import pycurl

cnt=0
print("Waiting Start...")

args = sys.argv
scorer=scorer_sdk.Scorer.Scorer("Task")
x1,y1,x2,y2 = scorer.get_roi_rect_by_index(0)



while True:

    if scorer.poll() == False:
        continue

    frame = scorer.get_frame()
    if frame == None:
        continue

    bgr = frame.get_bgr()
    scorer.web_show(bgr, 2)
    bgr = cv2.rectangle(bgr, (x1,y1),(x2,y2), (0,255,0), 5)
    scorer.web_show(bgr, 1)


    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

def send2line():
    buffer='saved'
    curl = pycurl.Curl()
    curl.setopt(pycurl.URL, 'https://notify-api.line.me/api/notify')
    curl.setopt(pycurl.HTTPHEADER, ['Authorization: Bearer AhfffLL3f1BYUJMnBnV8HZFAuvFS3ztJ65ZQv9UPo5j'])
    curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8')),('imageFile', (curl.FORM_FILE, 'lena.png'))])
    curl.perform()

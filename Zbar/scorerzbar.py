import zmq
import cv2
import sys
from scorer import Scorer
import zbarimg

cnt=0
print("Waiting Start...")

###
### Scorer Python SDK Test for Web
###

my_dict =  {"data":"hogehoge", "comment":"this is a comment"}
my_str  = "logstring-test"
my_list  = [100,200,300]

img_list = []

scorer=Scorer("Task")

while True:

    if scorer.poll() == False:
        continue

    frame = scorer.get_frame()
    if frame == None:
       continue

    gray = frame.get_gray()
    scorer.web_show(gray, 1)
    bartype, bardata = zbarimg.passzbar(gray)
    for data in bardata:
        print(data.decode('utf-8'))

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

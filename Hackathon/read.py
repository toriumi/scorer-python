import zmq
import cv2
import sys
from scorer import Scorer
import zbarimg
from time import sleep
import subprocess
from datetime import datetime
import RPi.GPIO as GPIO  
from paramiko import SSHClient,AutoAddPolicy
from scp import SCPClient
HOST = "210.224.185.213"
PORT = 22
USER = "shintashinta"
PSWD = "4fv93gnb4f"

cnt=0
print("Waiting Start...")
GPIO.setmode(GPIO.BCM)  
GPIO.setup(24, GPIO.OUT) 
###
### Scorer Python SDK Test for Web
###

my_dict =  {"data":"hogehoge", "comment":"this is a comment"}
my_str  = "logstring-test"
my_list  = [100,200,300]

img_list = []
date_str="0"
date_num=0
barcode=""
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
    sutterflg=0
    firstcode=""
    secondcode=""
    for data in bardata:
        string=data.decode('utf-8')
        if string[0:3]=="978":
            firstcode=data.decode('utf-8')
            print("firstcode:"+data.decode('utf-8'))
        if string[0:3]=="192":
            secondcode=data.decode('utf-8')
            print("secondcode:"+data.decode('utf-8'))
        
    date = datetime.now()    
    if firstcode!="" and secondcode!="" and date.timestamp()-date_num>5 and barcode!=firstcode+"_"+secondcode:
        GPIO.output(24, 1)
        barcode=firstcode+"_"+secondcode
        print("shutter countdoun")
        print(date.timestamp()-date_num)
        sleep(3.5)
        GPIO.output(24, 0)
        sleep(0.5)
        GPIO.output(24, 1)
        sleep(0.5)
        GPIO.output(24, 0)
        sleep(0.5)
        GPIO.output(24, 1)
        sleep(0.5)
        GPIO.output(24, 0)
        sleep(2)
        date_num = date.timestamp()
        date_str = date.strftime("%Y%m%d%H%M%S")
        subprocess.check_call("fswebcam -i 0 -d /dev/video1 --no-banner -r 1280x720 ./image/"+date_str+"_"+firstcode+"_"+secondcode+"_.jpg", shell=True)
        sleep(5)
        
        #subprocess.check_call("scp -r ~/scorer-python/Hackathon/image book.com:/home/shintashinta/www/image", shell=True)
        ssh = SSHClient()
        ssh.set_missing_host_key_policy(AutoAddPolicy())
        ssh.connect(HOST, port=PORT, username=USER, password=PSWD)

        scp = SCPClient(ssh.get_transport())
        sftp = ssh.open_sftp()
        sftp.put("./image/"+date_str+"_"+firstcode+"_"+secondcode+"_.jpg", "/home/shintashinta/www/image/"+date_str+"_"+firstcode+"_"+secondcode+"_.jpg")
        sftp.close()
        ssh.close()
        subprocess.check_call("mv ./image/"+date_str+"_"+firstcode+"_"+secondcode+"_.jpg ./image/done", shell=True)
        continue
    if date.timestamp()-date_num>30:
        barcode=""
    
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
import scorer
import cv2
import pycurl
from time import sleep
from datetime import datetime, timedelta


#HAAR分類器の顔検出用の特徴量
cascade_path = "haarcascade_frontalface_alt.xml"
color = (255, 255, 255) #白
cascade = cv2.CascadeClassifier(cascade_path)

#通知しすぎないように最低通知間隔と、顔の大きさの最低値（px）を設定
check=datetime.now()
faceminwidth=100

cap = scorer.VideoCapture(0)

print("waiting...")
while True:
    # Read Frame from Camera
    frame = cap.read()
    if frame == None:
       continue

    #グレースケールで顔検知し、通知用の画像はカラーを使う
    image = frame.get_bgr()
    gray = frame.get_gray()

    #確認用画面 
    scorer.imshow(1, gray)
    #顔検知実行
    facerect = cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=1, minSize=(1, 1))    
    #顔が存在したら顔を検知して通知
    if len(facerect) > 0 and frame.datetime-check > timedelta(seconds=5):#5秒以内の検知は無視
        check=frame.datetime
        #検出した顔を囲む矩形の作成
        for rect in facerect:
            cv2.rectangle(image, tuple(rect[0:2]),tuple(rect[0:2]+rect[2:4]), color, thickness=2)
            if rect[2]>faceminwidth:
                #認識結果の保存
                cv2.imwrite("detected.jpg", image)
                #LINE Notifyへ通知
                buffer=str(frame.datetime)+"顔発見！！"
                curl = pycurl.Curl()
                curl.setopt(pycurl.URL, 'https://notify-api.line.me/api/notify')
                curl.setopt(pycurl.HTTPHEADER, ['Authorization: Bearer WgwVWFgG5EAJwDbRFLQ4037JeOVmx8Q0fVRgQZPIkIS'])
                curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8')),('imageFile', (curl.FORM_FILE, 'detected.jpg'))])
                curl.perform()


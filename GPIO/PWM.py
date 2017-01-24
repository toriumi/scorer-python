import time
import os
import RPi.GPIO as GPIO            # import RPi.GPIO module  
GPIO.setmode(GPIO.BCM)             # choose BCM or BOARD  
GPIO.setup(24, GPIO.OUT)           # set GPIO24 as an output   
p24 = GPIO.PWM(24, 60)
p24.start(0)
try:
    while True:
        # 徐々に明るく
        for dc in range(0, 101, 20):
            p24.ChangeDutyCycle(dc)
            time.sleep(0.1)


        # 徐々に暗く
        for dc in range(100, -1, -20):
            p24.ChangeDutyCycle(dc)
            time.sleep(0.1)

except KeyboardInterrupt:          # trap a CTRL+C keyboard interrupt  
    p24.stop()
    GPIO.cleanup()  
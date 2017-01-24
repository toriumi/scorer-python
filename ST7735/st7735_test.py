#!/usr/bin/env python
# -*- coding: utf-8 -*-
 
import RPi.GPIO as GPIO
import spidev           # sudo pip install spidev
import time
 
class ST7735:
    def __init__(self):
        self.spi = spidev.SpiDev()
        self.spi.open(0, 0)
        self.spi.max_speed_hz = 16000000
        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)
        GPIO.setup(24, GPIO.OUT)
        GPIO.setup(25, GPIO.OUT)
        self.reset()
        self.write_cmd(0x11)
        time.sleep(0.12)
        self.write((0xB1, 0x01, 0x2C, 0x2D))
        self.write((0xB2, 0x01, 0x2C, 0x2D))
        self.write((0xB3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D))
        self.write((0xB4, 0x07))
        self.write((0xC0, 0xA2, 0x02, 0x84))
        self.write((0xC1, 0xC5))
        self.write((0xC2, 0x0A, 0x00))
        self.write((0xC3, 0x8A, 0x2A))
        self.write((0xC4, 0x8A, 0xEE))
        self.write((0xC5, 0x0E))
        self.write((0x36, 0xC8))
        self.write((0xE0, 0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10))
        self.write((0xE1, 0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10))
        self.write((0x2A, 0x00, 0x02, 0x00, 0x81))
        self.write((0x2B, 0x00, 0x01, 0x00, 0xA0))
        self.write((0x3A, 0x05))
        self.write_cmd(0x29)
    def reset(self):
        GPIO.output(25, False)
        time.sleep(0.1)
        GPIO.output(25, True)
        time.sleep(0.1)
    def write_cmd(self, cmd):
        GPIO.output(24, False)  # RS=0
        self.spi.xfer2([cmd])
    def write_data(self, data):
        GPIO.output(24, True)   # RS=1
        self.spi.xfer2([data])
    def write(self, cmd):
        if len(cmd) == 0:
            return
        GPIO.output(24, False)  # RS=0
        self.spi.xfer2([cmd[0]])
        GPIO.output(24, True)   # RS=1
        self.spi.xfer2(list(cmd[1:]))
    def write_rgb(self, r, g, b):
        self.write_data(r & 0xF8 | g >> 5)
        self.write_data(g & 0xFC << 3 | b >> 3)
    def fill(self, r, g, b):
        self.write((0x2A, 0x00, 0x02, 0x00, 0x81))
        self.write((0x2B, 0x00, 0x01, 0x00, 0xA0))
        self.write_cmd(0x2C)
        for i in range(128):
            for n in range(128):
                self.write_rgb(r, g, b)
         
if __name__ == "__main__":
    lcd = ST7735()
    lcd.write_cmd(0x2C)
    print('\n001')
    try:
        while True:
            lcd.fill(255, 255, 255)   # Fill Black
            for i in range(128):
                print('\n002_'+str(i))
                for n in range(128):
                    
                    if i < 22:
                        lcd.write_rgb(0xFF, 0xFF, 0xFF) # White
                    elif i < 44:
                        lcd.write_rgb(0xFF, 0xFF, 0x00) # Yellow
                    elif i < 66:
                        lcd.write_rgb(0x00, 0xFF, 0xFF) # Cyan
                    elif i < 88:
                        lcd.write_rgb(0x00, 0xFF, 0x00) # Green
                    elif i < 110:
                        lcd.write_rgb(0xFF, 0x00, 0xFF) # Magenta
                    elif i < 132:
                        lcd.write_rgb(0xFF, 0x00, 0x00) # Red
                    else:
                        lcd.write_rgb(0x00, 0x00, 0xFF) # Blue
            time.sleep(3)
            
    except KeyboardInterrupt:
        print('\nbreak')
    GPIO.cleanup()
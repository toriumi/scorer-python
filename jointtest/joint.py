import zmq
import cv2
import numpy as np
import time
from datetime import datetime

web_sock1      = "ipc://@/scorer/web_sock1"

ctx = zmq.Context()
#
self.poller = zmq.Poller()
self.poller.register(self.img_sock, zmq.POLLIN)
#
self.web_sock1 = ctx.socket(zmq.PUB)
self.web_sock1.setsockopt(zmq.SNDHWM, 10000)

serialized = pickle.dumps(cvdata, protocol=4)
self.web_sock1.bind(web_sock1)
self.web_sock1.send(serialized)
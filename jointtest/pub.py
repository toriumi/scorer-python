#https://librabuch.jp/blog/2015/09/mosquiito_paho_python_mqtt/
from time import sleep
import paho.mqtt.client as mqtt

host = '127.0.0.1'
port = 1884
topic = 'iktakahiro/a'

# インスタンス作成時に protocol v3.1.1 を指定します
client = mqtt.Client(protocol=mqtt.MQTTv311)

client.connect(host, port=port, keepalive=60)

for i in range(3):
    client.publish(topic, 'ham')
    sleep(0.2)

client.disconnect()
import sys
import os
import argparse
import time
import ntpath
import paho.mqtt.client as mqtt
from time import sleep

topic = '13113F/audio'

def sendIR(ir):
    print(ir)
    client.publish(topic, ir)
    client.loop()
    return

client = mqtt.Client()

client.username_pw_set("ESP", "1234")
client.connect("192.168.0.100", 1883, 60)


prefix = '08E7'

sendIR("08E750AF")
sleep(0.5)
sendIR("08E750AF")
sleep(1)
sendIR("08E750AF")

exit(0)
for type in range(0,15):
   for code in range(0,15):
       a = code
       b = type
       c = ~a & 0xF
       d = (a ^ b) ^ c
       ir = prefix + format(a, 'X') + format(b, 'X') + format(c, 'X') + format(d, 'X')

       if ir == "08E750AF":
           print("Skipping 08E750AF")
       else:
           sendIR(ir)
       sleep(0.5)

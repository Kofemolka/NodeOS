import sys
import os
import argparse
import time
import ntpath
import paho.mqtt.client as mqtt

if __name__ == '__main__':
    # parse arguments or use defaults
    parser = argparse.ArgumentParser(description='NodeOS Uploader.')
    parser.add_argument('-host',    required=True,  help='MQTT Host')
    parser.add_argument('-port',    required=True,  help='MQTT Port')
    parser.add_argument('-user',                    help='MQTT User')
    parser.add_argument('-pwd',                     help='MQTT Password')
    parser.add_argument('-id',      required=True,  help='Device ID')
    parser.add_argument('-file',                    help='File to upload')
    parser.add_argument('-r',                       help='Reboot device after upload', action="store_true"  )

    args = parser.parse_args()

topic = args.id + '/dev/ip'

def upload(ip, fileName):
    os.system("luatool.py --ip " + ip + " -f " + fileName + " -t " + ntpath.basename(fileName))
    return

def reboot(ip):
    print("Rebooting " + ip)
    client.publish(args.id + "/dev/reset", "do")
    client.loop()
    return

def on_connect(client, userdata, flags, rc):
    client.subscribe(topic)
    return

done = False

def on_publish(mosq, obj, mid):
    return

def on_message(client, userdata, msg):
    if msg.topic == topic:
        print(str(msg.payload))
        ip = msg.payload

        if args.file:
            upload(ip, args.file)

        if args.r:
            reboot(ip)

        done = True
        client.disconnect()
    return

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.on_publish = on_publish
client.username_pw_set(args.user, args.pwd)
client.connect(args.host, args.port, 60)

client.publish(args.id + "/dev/telnet", "on")

#client.loop_start()

rc = 0
while rc == 0 and done == False:
   rc = client.loop()

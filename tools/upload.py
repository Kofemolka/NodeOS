import sys
import os
import argparse
import ntpath
import paho.mqtt.client as mqtt

if __name__ == '__main__':
    # parse arguments or use defaults
    parser = argparse.ArgumentParser(description='NodeOS IP discovery.')
    parser.add_argument('-host',    required=True,  help='MQTT Host')
    parser.add_argument('-port',    required=True,  help='MQTT Port')
    parser.add_argument('-user',                    help='MQTT User')
    parser.add_argument('-pwd',                     help='MQTT Password')
    parser.add_argument('-id',      required=True,  help='Device ID')
    parser.add_argument('-file',    required=True,  help='File to upload')
    parser.add_argument('-r',                       help='Reboot device after upload')

    args = parser.parse_args()

topic = args.id + '/dev/ip'

def upload(ip, fileName):
    os.system("luatool.py --ip " + ip + " -f " + fileName + " -t " + ntpath.basename(fileName))
    return

def on_connect(client, userdata, flags, rc):
    client.subscribe(topic)
    return

gotIp = False

def on_message(client, userdata, msg):
    if msg.topic == topic:
        gotIp = True
        print(str(msg.payload))
        upload(msg.payload, args.file)
        
        sys.exit(0)
    return

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(args.user, args.pwd)
client.connect(args.host, args.port, 60)

client.publish(args.id + "/dev/telnet", "on")

while gotIp == False:
    client.loop(2)

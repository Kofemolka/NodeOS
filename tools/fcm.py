# Send to single device.
import argparse
import pyfcm

if __name__ == '__main__':
    # parse arguments or use defaults
    parser = argparse.ArgumentParser(description='OpenHAB Push Notifier')
    parser.add_argument('-topic',    required=True,  help='Topic')
    parser.add_argument('-msg',    required=True,  help='Message')

    args = parser.parse_args()

    push_service = pyfcm.FCMNotification(api_key="AAAAryqDsSM:APA91bGIv1tk_ilQPP3Srhh96k1QDIZcbvpE5VtTXrdOxd7dOnbG1cYHcCqC-u3mUB6vLXjwaPg-BJYu3YCtLVne8fd8AWT20taskHT7Icfk4UKW6epFunvhTBC4vqptO0uO7lffuLnI")

    registration_id = "cg3ysVO1BAY:APA91bFXJaO8pPehkP2jxTBLebF3fem92kHpNkkDYDt6ju-q9Jw63IBu-EYdDXd2UMvcfXvjxLxNyoMmxC-z6HRluiUyccgGlqWbOCL42mUPZCG2ExOwT5r9wo5J5W_vxK1ff2s9D8_I"
    message_title = "OpenHAB Event"
    message_body = "PYFCM test message"
    #push_service.notify_topic_subscribers(topic_name="global", message_body=message_body)
    result = push_service.notify_single_device(registration_id=registration_id, message_title=args.topic, message_body=args.msg)

    print result

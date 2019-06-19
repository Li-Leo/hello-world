#!/usr/bin/env python3import smtplib
# -*- coding: utf-8 -*-

import time
from sinchsms import SinchSMS

number = '+8615102113002'
message = 'I love SMS!'

client = SinchSMS("4209bed0-a77b-4231-9cb4-b71a1fb0aaea", 'UehqPl/PDUOIljdAqgUlug==')

print("Sending '%s' to %s" % (message, number))
response = client.send_message(number, message)
message_id = response['messageId']

response = client.check_status(message_id)
while response['status'] != 'Successful':
    print(response['status'])
    time.sleep(1)
    response = client.check_status(message_id)
    print(response['status'])
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import smtplib
import sys
from email.mime.text import MIMEText

def mail():
    body = '''
    今日天气不错，天天天蓝！
    谢谢！

    Best wishes,
    Leo
    '''
    msg = MIMEText(body,'plain','utf-8')

    sender = 'ljl_leo@163.com'
    password = 'aloe707thee037'

    recipient = ['oleoleo037@163.com'] #'jlli@microport.com',

    smtp_server = 'smtp.163.com'
    msg['From'] = sender
    msg['To'] = ';'.join(recipient) + ';'
    msg['Subject'] = '今日天气'
    
    try:
        server = smtplib.SMTP(smtp_server, 25)
        server.login(sender, password)
        try:
            server.set_debuglevel(1)
            server.sendmail(sender, recipient, msg.as_string())
        finally:
            server.quit()
            server.close()
    except Exception as e:
        print(e) # give an error message

if __name__ == "__main__":
    mail()

''' send mail with attachment
import smtplib
import os.path as op
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email.mime.text import MIMEText
from email.utils import COMMASPACE, formatdate
from email import encoders

def send_mail(send_from, send_to, subject, message, files=[],
              server="localhost", port=587, username='', password='',
              use_tls=True):
    """Compose and send email with provided info and attachments.

    Args:
        send_from (str): from name
        send_to (str): to name
        subject (str): message title
        message (str): message body
        files (list[str]): list of file paths to be attached to email
        server (str): mail server host name
        port (int): port number
        username (str): server auth username
        password (str): server auth password
        use_tls (bool): use TLS mode
    """
    msg = MIMEMultipart()
    msg['From'] = send_from
    msg['To'] = COMMASPACE.join(send_to)
    msg['Date'] = formatdate(localtime=True)
    msg['Subject'] = subject

    msg.attach(MIMEText(message))

    for path in files:
        part = MIMEBase('application', "octet-stream")
        with open(path, 'rb') as file:
            part.set_payload(file.read())
        encoders.encode_base64(part)
        part.add_header('Content-Disposition',
                        'attachment; filename="{}"'.format(op.basename(path)))
        msg.attach(part)

    smtp = smtplib.SMTP(server, port)
    if use_tls:
        smtp.starttls()
    smtp.login(username, password)
    smtp.sendmail(send_from, send_to, msg.as_string())
    smtp.quit()
'''
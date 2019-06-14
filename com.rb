#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import time
import sys
import serial
from datetime import datetime
import keyboard
import re

port_name = sys.argv[1]

#open com
com = serial.Serial(
    port=port_name,
    baudrate=115200,
)

com.write('givemecmd\n'.encode('utf-8'))
time.sleep(0.2)
com.reset_input_buffer()

def interact():
    print("Enter your commands below or 'exit' to leave the application.")   
    while True:
        cmd = input("Please input command: ")

        if cmd == 'exit':
            com.close()
            exit()
        elif not cmd:
            print("no command input!")
        else:
            com.write((cmd + '\n').encode('utf-8'))
            serial_in = ''
            # let's wait some second before reading serial_input
            if 'log' in cmd:
                time.sleep(10)
            else:
                time.sleep(0.5)

            while com.in_waiting > 0:
                serial_in += (com.readline().decode('utf-8'))
                
            if serial_in:
                serial_in = serial_in[len(cmd):]
                print(serial_in)

                if 'log' in cmd or cmd == 'data':
                    with open(cmd +'.txt', 'w') as f:
                        f.write(serial_in)
                        f.write('\ncreated time: ' + datetime.now().strftime('%Y-%m-%d %H:%M:%S'))

def send_repeat_cmd(cmd):
    with open(datetime.now().strftime('%Y-%m-%d %H-%M-%S ')
                + cmd + '.txt', 'w') as f:
        while True:
            com.write((cmd + '\n').encode('utf-8'))
            serial_in = ''
            time.sleep(0.2)

            while com.in_waiting > 0:
                serial_in += (com.read().decode('utf-8'))
                    
            if serial_in:
                serial_in = serial_in[len(cmd):]
                f.write(serial_in)
                print(serial_in)

            if keyboard.is_pressed('esc'):
                com.close()
                break

def get_factory_data():
    with open('data.txt') as f:
        for line in f:
            if 'S/N' in line:
                SN = line[5:].strip()
            elif 'EncoderCountEachML' in line:
                encoder_count_each_ml = re.findall('\d+', line)[0]
            elif 'Bubble alarm' in line:
                bubble_alarm_status = re.findall('\d+', line)[0]
            elif 'pressure threshold' in line:
                pressure_threshold = re.findall('\d+', line)[0]
            elif 'precision factor' in line:
                precision_factor = re.findall('\d+', line)[0]

        # print(SN,encoder_count_each_ml,bubble_alarm_status,pressure_threshold,precision_factor)
        return (SN, encoder_count_each_ml, bubble_alarm_status,
                pressure_threshold, precision_factor)

def log_data():
    with open('log.txt') as f, open('log1.txt','w') as f1:
        for line in f:
            if '发生报警' in line:
                f1.write(line)

if __name__ == "__main__":
    # print("Please select mode: 1 -- interactive mode, 2 -- repeat mode.")   
    # mode = input("Please input mode: ")
    # if mode == '2':
    #     cmd = input("Please input cmd: ")
    #     print("Press 'Esc' to exit repeat mode")
    #     time.sleep(1)
    #     send_repeat_cmd(cmd)
    # elif mode == '1':
    #     interact()
    # else:
    #     exit()
    # get_factory_data()
    log_data()



'''  pyserial module man
pip install pyserial

class serial.Serial
__init__(port=None, baudrate=9600, bytesize=EIGHTBITS, parity=PARITY_NONE, 
            stopbits=STOPBITS_ONE, timeout=None, xonxoff=False, rtscts=False,
            write_timeout=None, dsrdtr=False, 
            inter_byte_timeout=None, exclusive=None)
Parameters: 
port – Device name or None.
baudrate (int) – Baud rate such as 9600 or 115200 etc.
bytesize – Number of data bits. Possible values: FIVEBITS, SIXBITS, SEVENBITS, EIGHTBITS
parity – Enable parity checking. Possible values: PARITY_NONE, PARITY_EVEN, PARITY_ODD PARITY_MARK, PARITY_SPACE
stopbits – Number of stop bits. Possible values: STOPBITS_ONE, STOPBITS_ONE_POINT_FIVE, STOPBITS_TWO
timeout (float) – Set a read timeout value.
xonxoff (bool) – Enable software flow control.
rtscts (bool) – Enable hardware (RTS/CTS) flow control.
dsrdtr (bool) – Enable hardware (DSR/DTR) flow control.
write_timeout (float) – Set a write timeout value.
inter_byte_timeout (float) – Inter-character timeout, None to disable (default).


open()
close()
in_waiting:  Get the number of bytes in the receive buffer.
out_waiting :Get the number of bytes in the output buffer
reset_input_buffer()
reset_output_buffer()

readline(size=-1)
readlines(hint=-1)
writelines(lines)
get_settings()
apply_settings(d)  d (dict) – a dictionary with port settings.

write(data)
Parameters: data – Data to send.
Returns:    Number of bytes written.
Return type:    int
Raises: SerialTimeoutException – In case a write timeout is configured for the 
port and the time is exceeded.
Write the bytes data to the port. This should be of type bytes (or compatible 
such as bytearray or memoryview). Unicode strings must be encoded 
(e.g. 'hello'.encode('utf-8').

read(size=1)
Parameters: size – Number of bytes to read.
Returns:    Bytes read from the port.
Return type:    bytes
Read size bytes from the serial port. If a timeout is set it may return 
less characters as requested. With no timeout it will block until the 
requested number of bytes is read.

read_until(expected=LF, size=None)
Parameters: 
    expected – The byte string to search for.
    size – Number of bytes to read.
Returns:    
    Bytes read from the port.

Return type:  bytes

Read until an expected sequence is found (‘\n’ by default), the size is 
exceeded or until timeout occurs. If a timeout is set it may return less 
characters as requested. With no timeout it will block until the requested
number of bytes is read.

'''
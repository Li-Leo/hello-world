#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import time
import sys
import serial
import serial.tools.list_ports
import keyboard
import re
from   subprocess import run
from   datetime   import datetime
from   pathlib import Path

def open_available_port():
    print("all available ports:")
    all_ports = serial.tools.list_ports.comports()
    ports =sorted([str(port).lower() for port in all_ports])
    for port in ports: print(port)
    port_name = input("\nplease select the correct port: ")
    is_exist = False
    for port in ports:
        if port_name.lower() in port and port_name.lower().startswith('com'):
            is_exist = True
    
    if not is_exist:
        print("port doesn't exists!")
        exit()

    try:
        com = serial.Serial(port=port_name, baudrate=115200,timeout=0.1)
        com.write('givemecmd\n'.encode('utf-8'))
        time.sleep(0.2)
        com.reset_input_buffer()
    except Exception as e:
        print('failed to open the port!')
        print(e)
        exit()
    return com

def interact(com):
    print("enter your commands below or 'exit' to leave the application.")   
    while True:
        cmd = input("please input command: ")

        if cmd == 'exit':
            print("bye!")
            com.close()
            exit()
        elif not cmd:
            print("no command input!")
        else:
            com.write((cmd + '\n').encode('utf-8'))
            serial_in = ''
            # let's wait some seconds before reading serial_input
            if 'log' in cmd:
                time.sleep(1)
                print('reading log...')
            else:
                time.sleep(0.1)

            while com.in_waiting:
                read_in = (com.read(100)).decode('utf-8','ignore')#
                serial_in += read_in
                print(read_in, end='')
                time.sleep(0.01)
                # print(f'{com.in_waiting}')
                
            if serial_in:
                # print(serial_in)
                # print(len(serial_in))

                if 'log' in cmd or cmd == 'data' or cmd == 'read_data':
                    file_name = cmd.replace(' ','_')
                    start_cmd = 'start ' + file_name + '.txt'
                    with open(file_name +'.txt', 'w') as f:
                        f.write(serial_in)
                        f.write('\ncreated time: ' + 
                                datetime.now().strftime('%Y-%m-%d %H:%M:%S'))

                    if cmd == 'data' or 'log' in cmd:
                        name = ''
                        with open(file_name +'.txt') as f:
                            for line in f:
                                if "S/N" in line:
                                    name = line[4:].strip()
                                    break
                        if name:
                            name_path = Path(file_name +'.txt')
                            if not Path(file_name + '_' + name + '.txt').exists(): 
                                name_path.rename(file_name + '_' + name + '.txt')
                                file_name = file_name + '_' + name + '.txt'
                                start_cmd = 'start ' + file_name
                            else:
                                print(f"\n{file_name}_{name}.txt already exists")

                    run(start_cmd, shell=True)

def send_repeat_cmd(cmd, com):
    with open(datetime.now().strftime('%Y-%m-%d %H-%M-%S ')
                + cmd + '.txt', 'w') as f:
        while True:
            com.write((cmd + '\n').encode('utf-8'))
            serial_in = ''
            time.sleep(0.2)

            while com.in_waiting > 0:
                serial_in += (com.read().decode('utf-8'))
                    
            if serial_in:
                f.write(serial_in)
                print(serial_in)

            if keyboard.is_pressed('esc'):
                print("bye!")
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
    com = open_available_port()
    print("please select mode: 1--interactive mode, 2--repeat mode.")   
    mode = input("please input mode: ")
    if mode == '2':
        cmd = input("please input cmd: ")
        print("press 'esc' to exit repeat mode")
        time.sleep(1)
        send_repeat_cmd(cmd, com)
    elif mode == '1':
        interact(com)
    else:
        print("bye!")
        exit()
    # get_factory_data()
    # log_data()



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
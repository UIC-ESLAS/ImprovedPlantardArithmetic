#!/usr/bin/env python3
import serial
import serial.tools.list_ports
import sys
import platform
import os

# ports = serial.tools.list_ports.comports()
# for port in ports:
#     print(port.device, port.description)
#     if(len(sys.argv) == 2 and (sys.argv[1] == "M4")):
#         if "UART" in port.description:
#             dev = serial.Serial(port.device, 115200)
#     else:
#         if "STM32" in port.description:
#             dev=serial.Serial(port.device, 115200)
          
dev = serial.Serial("/dev/ttyUSB0", 115200)

command = "st-flash --flash=128k --reset write " + \
    str(sys.argv[1]) + " 0x8000000 " + ">/dev/null 2>&1"
print(command)
os.system(command)

print("--------------------Start Reading: "+str(sys.argv[1])+"-----------------------")

while True:
    x = dev.read()
    try:
        if x.decode('utf-8')=='#':
            print("\n")
            break
        sys.stdout.buffer.write(x)
    except:
        pass    
sys.stdout.flush()

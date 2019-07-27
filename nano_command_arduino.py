""" LIBRARIES & DEPEENDENCIES HERE """

import serial
import time

""" END OF LIBRARY & DEPENDENCY """




""" VARIABLES CONFIGURATION """

## ---- Serial Configuration ---- ##
baudrate = 115200
port = "/dev/ttyUSB0"
time_out = 0.1
arduino = serial.Serial(port, baudrate, timeout=time_out)
time.sleep(1)                                   # allow time for nano to sync with arduino
## ---- End of Serial Configuration ---- ##

""" END OF VARIABLE CONFIG """



""" ADD YOUR API HERE """

## ---- Send motor control commands to arduino ---- ##
"""
    Send commands to arduino to controL:
        1. Travel Direction
        2. PWM Speed
        3. Servo Angle
    
    Direction:
        +: forward
        -: backward
        
    Limitation of PWM Speed:
        0 - 255 
        
    Physical limitation of servo angles:
        20 - 100
        STRAIGHT == 60
        20 < LEFT < 60
        60 < RIGHT < 100
        
    Command template:
        "<speed,angle>"
        speed include "-" for bacward & "" for forward
"""
def send_command(speed=0, angle=60):
    command = "<{},{}>".format(str(speed), str(angle))
    print(command)                                     # for debugging purpose only
    arduino.write(command.encode())                     # send as byte to arduino via USB Serial
## ---- End of sending motor control commands---- ##

""" END OF API """



""" DEBUGGING CODE HERE """
send_command(-1000,200)
""" END OF DEGUGGING """



""" MAIN CODE RUNS HERE """

""" END OF MAIN CODE """
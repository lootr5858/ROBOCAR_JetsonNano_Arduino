""" LIBRARIES & DEPEENDENCIES HERE """

import serial
import time
import random

""" END OF LIBRARY & DEPENDENCY """




""" VARIABLES CONFIGURATION """

## ---- Serial Configuration ---- ##
baudrate = 115200
port = "/dev/ttyUSB0"
time_out = 0.1
arduino = serial.Serial(port, baudrate, timeout=time_out)
time.sleep(1)                                               # allow time for nano to sync with arduino
wait_for_reply = False                                       # change state between sending command and wait for arduino
arduino_ready = False
## ---- End of Serial Configuration ---- ##

""" END OF VARIABLE CONFIG """



""" ADD YOUR API HERE """

## ---- Check if arduino is ready ---- ##
"""
    Wait for arduino to fully configure
    Ready command" "Arduino is ready!"    
"""
def check_arduino_ready():
    time.sleep(1)
    ready = arduino.read_all()
    ready = ready.decode('utf-8')
    print(ready)
    if "Arduino is ready!" in ready:
        print("Robocar is ready!")
        return True

    else:
        print(ready)
        return False

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
    print("Send to arduino: {}".format(command))
    arduino.write(command.encode())                     # send as byte to arduino via USB Serial
    return command, True
## ---- End of sending motor control commands---- ##


## ---- Receive reply from arduino ---- ##
"""
    Arduino reply template: "<speed: {}, angle: {}>".format(speed, angle)
    pyserial reads byte, decode bytes to string with 'utf-8'
    verify reply msg in correct format
"""
def receive_verify_reply():
    reply = arduino.read_all()
    reply = reply.decode('utf-8')
    print(reply)

    if "<" in reply and ">" in reply:
        if "speed:" in reply and "angle:" in reply:
            print("Reply from arduino verified!\n")
            return False

        else:
            print("Waiting for reply...")
            return True

    else:
        print("Waiting for reply...")
        return True
## ---- End of Receive reply from arduino ---- ##

""" END OF API """



""" DEBUGGING CODE HERE """
while not arduino_ready:
    arduino_ready = check_arduino_ready()

while True:
    if wait_for_reply:
        wait_for_reply = receive_verify_reply()
        time.sleep(0.5)

    else:
        rand_speed = random.randint(-500, 500)
        rand_angle = random.randint(0, 100)
        wait_for_reply = send_command(rand_speed, rand_angle)
""" END OF DEGUGGING """



""" MAIN CODE RUNS HERE """

""" END OF MAIN CODE """
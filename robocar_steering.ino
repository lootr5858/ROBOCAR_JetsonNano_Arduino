
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*  
 *   Robocar Specification
 *   Motor shield: RobotDyn L298P motor shield (5-36V, 2A per motor)
 *   SINGLE motor config
 *   RWD using motor 1 (pins 8, 9 & 10 for logic OFF, pins 9 & 10 for logic ON)
 *   Front wheel steering using servomotor (pin 4)
 *   For this script, set logic to OFF
 */

/* 
 *  RobotDyn L298P motor shield
 *                      Logic ON                Logic OFF   
 *  Motor   Pin Forward Backward Stop   Forward Backward  Stop  Brake     Comments
 *  1(E1)   10     1        1     0        1       1        0     1       PWM         (Analog for variable speed, 0-255. Digital for full speed.)
 *  1(In1)  9      0        1     0/1      1       0       0/1    1       Direction 1 (Digital)
 *  1(In2)  8   ---------NA----------      0       1       0/1    1       Direction 2 (Digital)
 *  
 *  2(E2)   11     1        1     0        1       1        0     1       PWM         (Analog for variable speed, 0-255. Digital for full speed.)
 *  2(In3)  13     0        1    0/1       0       1       0/1    1       Direction 1 (Digital)
 *  2(In4)  12  ----------NA----------     1       0       0/1    1       Direction 2 (Digital)
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int E1 = 11;
const int In1 = 13;
const int In2 = 12;
const int speed_value = 255;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Logic OFF
// STOP w/ brake
void stop_brake()
{
  analogWrite(E1, speed_value);
  digitalWrite(In1, HIGH);
  digitalWrite(In2, HIGH);
}

// STOP w/o brake
void stop_nobrake()
{
  analogWrite(E1, 0);
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);
}

// RWD forward at half speed
void forward()
{
  analogWrite(E1, speed_value);
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
}

// RWD backward at half speed
void backward()
{
  analogWrite(E1, speed_value);
  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*  
 *   Servo motor control
 *  using library: Servo.h
 *  @setup: set servo position to 0
 *  left: 0 degree
 *  straight: 45 degree
 *  right: 90 degree
 */

Servo myservo;

// steer LEFT. set servo angle to 20 degree
void steer_left()
{
  myservo.write(20);
}

// steer STRAIGHT. set servo angle to 60 degree
void steer_straight()
{
  myservo.write(60);
}

// steer RIGHT. set servo angle to 100 degree
void steer_right()
{
  myservo.write(100);
}

// find back position 0 on servo
void servo_cal()
{
  myservo.write(0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * Single run code for hardware setup
 * set the motor pins to their respective modes
 * ensure that all motor are STOP
 * align servo to STRAIGHT
 */
void setup()
{
  // motor setup
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);

  // STOP all motors
  stop_nobrake();

  // servo setup (servo pin set to 4)
  // align to STRAIGHT
  myservo.attach(4);
  steer_straight();
}

void loop()
{
  // turn in circles in left forward direction for 10s
  steer_left();
  delay(1000);
  forward();
  delay(10000);

  // brake for 0.5s
  stop_brake();
  delay(5000);
  steer_straight();
  delay(1000);
  

  // turn in circles in right backward direction for 10s
  steer_right();
  delay(1000);
  backward();
  delay(10000);

  // brake for 0.5s
  stop_brake();
  delay(5000);
  steer_straight();
  delay(1000);
  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

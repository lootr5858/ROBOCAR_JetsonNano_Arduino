//////////////////////////////////////////
// --------  SUMMARY HERE !!! -------- //
////////////////////////////////////////
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


/*
 *  Arduino-Jetson Nano USB Serials Communication
 *  Baud rate  115200
 *  Jetson Nano -> Arduino Command Template: <PWM Speed Value, Steering Angle Value>
 *  Arduino -> Jetson Nano Reply Temaplate: <Speed: PWM Speed Value, Steering Angle: Steering Angle Value>
 */ 

 /*
  *   SAFETY MECHANISMS
  *   1. Nano-Arduino out-of-SYNC
  *      - if serial NOT AVAILABLE for more than 5s/1000 counts
  *      - apply BRAKE and STOP robocar
  *   
  *   2. PWM value exceeds 255
  *      - cap PWM value at max speed (255)
  *   
  *   3. Wheels steer beyond physical limit
  *      - LEFT < 20 -> change angle to 20
  *      - RIGHT > 100 -> change angle to 100
  */
///////////////////////////////////////////
// --------   END OF SUMMARY   -------- //
/////////////////////////////////////////



///////////////////////////////////////////
// -------- ADD LIBRARIES HERE -------- //
/////////////////////////////////////////

#include <Servo.h>

///////////////////////////////////////////
// --------  END OF LIBARARIES -------- //
/////////////////////////////////////////




///////////////////////////////////////////
// -------- ADD VARIABLES HERE -------- //
/////////////////////////////////////////

//----- Serial Config -----//
const byte buffer_size = 64;                // define max char for input_buffer
char input_buffer[buffer_size];             // store received data
const char start_marker = '<';              // denotes the start of a data package
const char end_marker = '>';                // denotes the end of a data package
byte bytes_received = 0;                    // count the no of bytes received
boolean read_in_progress = false;           // STATE of reading data
boolean new_data_from_nano = false;         // STATE of replying to Nano
char message_from_nano[buffer_size] = {0};
int no_serial = 0;                            // count for duration where arduino and nano are not in sync
//----- End of Serial Config -----//


// ---- Motor Control Var ---- //
int sp;     // PWM Speed control value (0 - 255)
int angle;  // Steering control angle (20 - 100)
// ---- End of Motor Control Var ----//


// ---- L293D Motor Controller Pins ----//
const int E1 = 11;  // PWM Speed Control pin
const int In1 = 13; // Logic Control 1 pin
const int In2 = 12; // Logic Control 2 pin
// ---- End of L293 Motor Controller Pins ----//


// ---- Servo Motor Configuration ----- //
Servo steer_control;            // Create object for Servo motor
const int servo_pin = 4;  // Use arduino D4 pin for servo control
// ---- End of Servo Configuration ---- //

///////////////////////////////////////////
// --------  END OF VARIABLES  -------- //
/////////////////////////////////////////



///////////////////////////////////////////
// --------    ADD API HERE    -------- //
/////////////////////////////////////////

// ---- Receive data from Jetson Nano ---- //
void receive()
{
  if (Serial.available() > 0)
  {
    char x = Serial.read(); // temp store of received bit
    no_serial = 0;          // reset counter as arduino and nano are in sync
    
    // !!! the order of these IF clauses is significant !!! //
    
    // -- Stop reading from serial when ">" package end marker received -- //
    if (x == end_marker)
    {
      read_in_progress = false;         // STATE change: stop reading
      new_data_from_nano = true;        // STATE change: ready to receive data from Nano
      input_buffer[bytes_received] = 0;
      parse_data();                     // call FUNCTION to analyse received data
    }

    // -- Reading data package received -- //
    if (read_in_progress)
    {
      input_buffer[bytes_received] = x;   
      bytes_received ++;                  // increase counter to prep for writing into next byte
      if (bytes_received == buffer_size)  // actions to do when buffer is full
      {
        bytes_received = buffer_size - 1;
      }
    }
    
    // -- Initiate reading -- //
    if (x == start_marker)
    {
      bytes_received = 0;       
      read_in_progress = true; // STATE change: start reading data package received
    }
  }

  if (Serial.available() < 0)
  {
    /*
     *  Arduino-Nano out-of-SYNC / lost connection
     *  When arduino NOT receiving packages from Nano
     *  Limit for no-serial counter before STOPPING robocar: 1000
     */
    no_serial ++;
    Serial.print("!!! Arduino-Nano OUT-OF-SYNC !!! No_serial count: ");
    Serial.println(no_serial);

    if (no_serial > 1000)
    {
      brake();
      Serial.println("!!! Arduino-Nano connection LOST !!! STOPING robocar!");
    }
  }
}
// ---- End of data receive from Nano ---- //


// ---- Analysing received data ---- //
void parse_data()
{
  char * strtok_index;  // Used by strtok() as index
  strtok_index = strtok(input_buffer, ",");

  int count = 0;
  
  // Extracting received data into useful data (PWM Speed & Steering angle)
  while (strtok_index != NULL)
  {
    count++;
    
    if (count == 1)
    {
      sp = atof(strtok_index);
    }

    else if (count == 2)
    {
      angle = atof(strtok_index);
    }

    strtok_index = strtok(NULL, ",");
  }
}
// ---- End of Analysing received data ----//


// ---- Feedback to Nano ----//
void reply_nano()
{
  if (new_data_from_nano)
  {
    new_data_from_nano = false;
    Serial.print("<speed: ");
    Serial.print(sp);
    Serial.print(". Steer angle: ");
    Serial.print(angle);
    Serial.println(">");

    next_action();
  }
}
// ---- End of Feedback to Nano ---- //


// ---- Carry out Nano's commands ---- //
void next_action()
{
  // -- PWM Speed Control -- //
  /*
   *  E1 PWM Speed Control pin 11
   *  PWM range 0 - 255
   *  +ve values ONLY
   *  use Logic controls In1 and In2 to reserve directions
   *  make sure to absolute "sp" value before passing into PWM control
   *  
   *  sp value > 0: forward direction
   *  sp value < 0: backward direction
   *  sp value = 0: brake
   */
   if (sp == 0)
   {
    // Nano commands for BRAKE
    brake();
   }
   
   else if (sp > 0)
   {
    // Nano commands for forward movement
    sp = abs(sp); // ensure that sp value is +ve
    forward(sp);  //  move forward
   }

   else if (sp < 0)
   {
    // Nano commands for backward movement
    sp = abs(sp); // ensures that sp value is +ve
    backward(sp); // move backward
   }
   // -- End of PWM Speed Control -- //

   // -- Steer angle control -- //
   /*
    *   Using Servo.h library
    *   Servo control pin 4
    *   Physcial limit for servo angle 20 - 100
    *   Steer STRAIGHT 60
    *   Steer LEFT 20
    *   Steer RIGHT 100
    */
    steer(angle);
    // -- End of Steer Angle Control -- //   
}
// ---- End of Carry out Nano's command ---- //


// ---- PWM Speed & Direction control ---- //
void brake()
{
  /*
   *  Always BRAKE at FULL power
   *  pwm = 255
   *  Logic control for brake:
   *  In1: HIGH
   *  In2: HIGH
   */
   analogWrite(E1, 255);
   digitalWrite(In1, HIGH);
   digitalWrite(In2, HIGH);

   // for debugging purpose only
   Serial.println("Braking....");
}

/*
 *  E1 PWM Speed COntrol pin 11
 *  PWM range 0 - 255
 *  if "sp" range exceeds PWM range, cap out at max/min
 */
void forward(int pwm)
{
  if (pwm > 255)
  {
    pwm = 255;
    Serial.print("Speed limit exceeded! Capped at ");
    Serial.print(pwm);
    Serial.println("!");
  }

  // Motor Control using ardiuno pins (DIGITAL & PWM)
  /*
   *  Logic control for forward:
   *  In1: HIGH
   *  In2: LOW
   */
  analogWrite(E1, pwm);
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);

  // for debugging purpose only
  Serial.print("Moving FORWARD at (PWM) speed: ");
  Serial.println(pwm);
}

void backward(int pwm)
{
  if (pwm > 255)
  {
    pwm = 255;
    Serial.print("Speed limit exceeded! Capped at ");
    Serial.print(pwm);
    Serial.println("!");
  }

  // Motor Control using ardiuno pins (DIGITAL & PWM)
  /*
   *  Logic control for forward:
   *  In1: LOW
   *  In2: HIGH
   */
  analogWrite(E1, pwm);
  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);

  // for debugging purpose only
  Serial.print("Moving BACKWARD at (PWM) speed: ");
  Serial.println(pwm);
}

// ---- End of PWM Speed & Direction control ---- //


// ---- Steering Control (Servo) ---- //
void steer(int ang)
{
  /*
   *  Physical limitation of wheels angle: 20 - 100
   *  Ensure that ang falls within this range
   *  
   *  STRAIGHT = 60
   *  20 < LEFT < 60
   *  60 < RIGHT< 100
   */
   if (ang < 20)
   {
    ang = 20;
    Serial.print("LEFT steering exceeded! Max.: ");
    Serial.println(ang);
   }


   else if (ang > 100)
   {
    ang = 100;
    Serial.print("RIGHT steering exceeded! Max.: ");
    Serial.println(ang);
   }

   steer_control.write(ang);

   // for debugging purpose only
   Serial.print("Steering: ");
   Serial.println(ang);
}

// -- Calibrating the servo motor on startup -- //
void calibrate()
{
  // set servo to 0 first
  steer_control.write(0);
  delay(1000);

  // reset wheel to STRAIGHT
  steer_control.write(60);
  delay(1000);
}
// ---- End of Steering Control (Servo) ---- //

///////////////////////////////////////////
// --------     END OF API     -------- //
/////////////////////////////////////////



///////////////////////////////////////////
// -------- SETUP ARDUINO HERE -------- //
/////////////////////////////////////////

void setup()
{
  // -- Serial Setup -- //
  Serial.begin(115200);
  Serial.println("Serial comms ready!");
  // -- End of Serial Setup -- //

  // -- Motor setup -- //
  Serial.println("Warming up motors ...");
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  brake();
  // -- End of Motor Setup -- //

  // -- Servo setup -- //
  steer_control.attach(servo_pin);
  Serial.println("Servo motor exercising ...");
  calibrate();
  Serial.println("Servo motor is sufficiently BUFF!");
  // -- End of Servo setup -- //
  

  Serial.println("Arduino is ready!");
}

///////////////////////////////////////////
// --------    END OF SETUP    -------- //
/////////////////////////////////////////




///////////////////////////////////////////
// -------- ARDUINO LOOP HERE  -------- //
/////////////////////////////////////////

void loop()
{
  receive();
  reply_nano();
}

///////////////////////////////////////////
// --------    END OF LOOP     -------- //
/////////////////////////////////////////

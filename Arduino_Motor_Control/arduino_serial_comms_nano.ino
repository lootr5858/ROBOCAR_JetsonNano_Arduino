//----- Serial Config -----//
const byte buffer_size = 64;        // define max char for input_buffer
char input_buffer[buffer_size];     // store received data
const char start_marker = '<';      // denotes the start of a data package
const char end_marker = '>';        // denotes the end of a data package
byte bytes_received = 0;            // count the no of bytes received
boolean read_in_progress = false;   // STATE of reading data
boolean new_data_from_nano = false; // STATE of replying to Nano
char message_from_nano[buffer_size] = {0};
//----- End of Serial Config -----//

// ---- Motor Control Var ---- //
int sp;     // PWM Speed control value (0 - 255)
int angle;  // Steering control angle (20 - 100)
// ---- End of Motor Control Var ----//

// ---- Receive data from Jetson Nano ---- //
void receive()
{
  if (Serial.available() > 0)
  {
    char x = Serial.read(); // temp store of received bit
    
    // the order of these IF clauses is significant
    // -- 
    if (x == end_marker)
    {
      read_in_progress = false;
      new_data_from_nano = true;
      input_buffer[bytes_received] = 0;
      parse_data();
    }

    
    if (read_in_progress)
    {
      input_buffer[bytes_received] = x;
      bytes_received ++;
      if (bytes_received == buffer_size)
      {
        bytes_received = buffer_size - 1;
      }
    }
    
    
    if (x == start_marker)
    {
      bytes_received = 0;
      read_in_progress = true;
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
  }
}
// ---- End of Feedback to Nano ---- //

void setup()
{
  // -- Serial Setup -- //
  Serial.begin(115200);
  Serial.println("Serial comms ready!");
  // -- End of Serial Setup -- //

  Serial.println("Arduino is ready!");
}

void loop()
{
  receive();
  reply_nano();

}

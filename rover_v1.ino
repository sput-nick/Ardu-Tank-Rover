// Rover v1
// Obstacle avoidance
// All ranges are in centimeters

#include <Servo.h>

// Pin definitions
const int PAN_PIN = 9;
const int TILT_PIN = 10;
const int PING_PIN = 7;
const int motorA = 3;
const int motorB = 11;
const int dirA = 12;
const int dirB = 13;

// Directions
const int EDGE = 0;
const int FRONT = 1;
const int LEFT = 2;
const int RIGHT = 3;
const int BACK = 4;
int ALT_DIR = 4; 

// Create servo objects
Servo pan;
Servo tilt;

// Servo directions
const int pan_forward = 110;
const int pan_left = 20;
const int pan_right = 180;
const int tilt_level = 130;
const int tilt_down = 40;

// delay for servo to be in position in milliseconds
const int servo_wait = 500;

// range variables
long edgeRange, frontRange, leftRange, rightRange;

// max range for EDGE, values larger than this indicate a drop, i.e. stairs
const long edgeMAX = 15;
// range values farther than this indicate clear path for full speed
const long clearRange = 100;
// minimum range, range less than this value indicate an obstacle
const long minRange = 15;

void setup()
{
  Serial.begin(9600);
  
  // pin setup
  pan.attach(PAN_PIN);
  tilt.attach(TILT_PIN);
  pinMode(motorA, OUTPUT);
  pinMode(dirA, OUTPUT);
  pinMode(motorB, OUTPUT);
  pinMode(dirB, OUTPUT);
  
  // point servo FRONT
  pan.write(pan_forward);
  tilt.write(tilt_level);
}

void loop()
{
  // Check for obstacles, avoid them
  edgeRange = getRange(EDGE);
  frontRange = getRange(FRONT);
  //leftRange = getRange(LEFT);
  //rightRange = getRange(RIGHT);
  
  // ping sensor sometimes returns 0, if that is the case
  // stop and range again
  if ( (edgeRange == 0) || (frontRange == 0) )
  {
    throttleStop();
    edgeRange = getRange(EDGE);
    frontRange = getRange(FRONT);
  }
  
  if (edgeRange > edgeMAX)
  {
    // at an edge, stop and find another path
    ALT_DIR = altPath();
    if (ALT_DIR == BACK)
    backward(128);
    else turn(ALT_DIR);
  }
  
  if (frontRange > clearRange)
  forward(255);
  
  if (frontRange <= clearRange && frontRange > minRange)
  forward(128);
  
  if (frontRange < minRange)
  {
    // at an obstacle, stop and find another path
    ALT_DIR = altPath();
    if (ALT_DIR == BACK)
    backward(128);
    else turn(ALT_DIR);
  }

}

int altPath()
{
  // check left/right, return the one with farther range
  throttleStop();
  leftRange = getRange(LEFT);
  rightRange = getRange(RIGHT);
  
  if (leftRange > rightRange && leftRange > minRange)
  return LEFT;
  
  else if (rightRange >= leftRange && rightRange > minRange)
  return RIGHT;
  
  else
  return BACK;
}

void throttleStop()
{
  // Stop moving
  Serial.println("Stopping");
  analogWrite(motorA, 0);
  analogWrite(motorB, 0);
}

void forward(int throttle)
{
  // move forward
  Serial.print("Moving forward at ");
  Serial.print(throttle/255);
  Serial.println("%");
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);
  analogWrite(motorA, throttle);
  analogWrite(motorB, throttle);
}

void backward(int throttle)
{
  // move backward
  Serial.print("Moving backward at ");
  Serial.print(throttle/255);
  Serial.println("%");
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite(motorA, throttle);
  analogWrite(motorB, throttle);
  
}

void turn(int dir)
{
  if (dir == RIGHT)
  {
    // turn right
    Serial.println("Turning right");
    analogWrite(motorA, 128);
    analogWrite(motorB, 128);
    digitalWrite(dirA, HIGH);
    digitalWrite(dirB, LOW);
    delay(500);
  }
  
  if (dir == LEFT)
  {
    // turn left
    Serial.println("Turning left");
    analogWrite(motorA, 128);
    analogWrite(motorB, 128);
    digitalWrite(dirA, LOW);
    digitalWrite(dirB, HIGH);
    delay(500);
  }  
}

long getRange(int dir)
{  
  long duration, cm;
  
  if (dir == EDGE)
  {
    // move sensor to edge
    pan.write(pan_forward);
    tilt.write(tilt_down);
  }
  
  if (dir == FRONT)
  {
    // move sensor to forward
    pan.write(pan_forward);
    tilt.write(tilt_level);
  }
  
  if (dir == LEFT)
  {
    // move sensor to left
    pan.write(pan_left);
    tilt.write(tilt_level);
  }
  
  if (dir == RIGHT)
  {
    // move sensor to right
    pan.write(pan_right);
    tilt.write(tilt_level);
  }
  
  // wait for servo to be in position
  delay(servo_wait);
  
  // Trigger the ping sensor, return distance in CM
  pinMode(PING_PIN, OUTPUT);
  digitalWrite(PING_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(PING_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(PING_PIN, LOW);
  
  // Get the delay value from sensor
  pinMode(PING_PIN, INPUT);
  duration = pulseIn(PING_PIN, HIGH);
  Serial.print("Duration: ");
  Serial.println(duration);
  
  // convert duration to centimeters
  cm = (duration / 29 / 2);
  Serial.print(dir);
  Serial.print(" range = ");
  Serial.println(cm);
  
  // move sensor to front/level
  pan.write(pan_forward);
  tilt.write(tilt_level);
  
  return cm;
  
}

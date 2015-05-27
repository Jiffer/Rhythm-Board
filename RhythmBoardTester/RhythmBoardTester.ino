/* Sweep
 by BARRAGAN <http://barraganstudio.com> 
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://arduino.cc/en/Tutorial/Sweep
*/ 

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
Servo servo2;                // twelve servo objects can be created on most boards
 
int pos = 0;    // variable to store the servo position 
 
void setup() 
{ 
  myservo.attach(10);  // attaches the servo on pin 9 to the servo object 
  servo2.attach(11);
  pinMode(9, OUTPUT);
  
    // enable internal pullups for buttons
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);

  // LEDs
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
} 
 
void loop() 
{ 
  digitalWrite(0, digitalRead(5));
  digitalWrite(1, digitalRead(6));
  digitalWrite(2, digitalRead(7));
  digitalWrite(3, digitalRead(8));
  digitalWrite(4, digitalRead(12));
   
  int delTime = map(analogRead(A0), 0, 1023, 1, 10);
  for(pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    servo2.write(pos);
    delay(2);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {
    servo2.write(pos);    
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(2);                       // waits 15ms for the servo to reach the position 
  } 
  digitalWrite(9, HIGH);
  delay(20);
  digitalWrite(9, LOW);
  delay(50);
} 

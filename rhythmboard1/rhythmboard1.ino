/*
Top to bottom on the board:
 10 -- servo1
 11 -- servo2
 9  -- solenoid
 
 A1 -- sensor1
 A2 -- sensor2
 A3 -- sensor3
 
 2  -- led3
 1  -- led1
 0  -- led2
 
 7  --  s3
 6  --  s4
 5  --  s5
 
 // left to right buttons and LEDs
 12  -- s1
 8  --  s2
 
 4 -- led4
 3 -- led5
 
 13 -- toggle-switch
 A0 -- knob
 
 */

#include <Servo.h>
#include "buttonStruct.h"
#include "blinkStruct.h"
#include "sensorStruct.h"

const int debounceTime = 30; // button debounce time

buttonStruct playButton = { 
  12, LOW, LOW, 0, 0};
boolean play; // playing or paused
int playLED = 4;

blinkStruct tempoLED = {
  3, 0, 45, 0};

// when was the last beat?
unsigned long lastHit = 0;
unsigned long count = 0; // keep track of what beat
int tempo = 0; // how many ms between beats (reads from knob on A0)

// solenoid, button and LED
buttonStruct solb = {
  5, LOW, LOW, 0, 0};
blinkStruct sol = {
  9, 0, 50, 0}; // (pin, lastHitTime, onTime (ms), current state)
blinkStruct solLED = {
  0, 0, 30, 0};
sensorStruct sols = {
  A3, 1, 1023, 0}; // (pin , mappedValue, min, max)

////////////////
// SERVO stuff:
// servos, buttons and LEDs
Servo servo1;
Servo servo2;
// current state
boolean servo1State = false; // left or right
boolean servo2State = false; // left or right
// servo buttons
buttonStruct servo1b = {
  7, LOW, LOW, 0, 0};
buttonStruct servo2b = {
  6, LOW, LOW, 0, 0};
// servo LEDs
blinkStruct servo1LED = {
  2, 0, 30, 0};
blinkStruct servo2LED = {
  1, 0, 30, 0};
// servo sensors
sensorStruct servo1s = {
  A1, 1, 1023, 0}; // (pin , mappedValue, min, max)
sensorStruct servo2s = {
  A2, 1, 1023, 0}; // (pin , mappedValue, min, max)

/////////////////////////////////////////////////
// initialization
/////////////////////////////////////////////////
void setup() {
  // attach servo pins to servo objects
  servo1.attach(10);
  servo2.attach(11);

  // paused at startup
  play = false;

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

  pinMode(9, OUTPUT);
}

////////////////////////////////////////////////////////////
//////////////////////// MAIN LOOP /////////////////////////
////////////////////////////////////////////////////////////
void loop() {
  // update sensors
  sols = readSensor(sols);
  servo1s = readSensor(servo1s);
  servo2s = readSensor(servo2s);

  // check if play button has been pressed
  // if so toggle state
  playButton = readDebounce(playButton);
  if (playButton.bang) {
    play = !play;
    digitalWrite(playLED, play );

  }

  // if in play mode: calculate tempo, read the channel buttons
  if (play) {
    // calculate tempo
    tempo = map(analogRead(A0), 0, 1023, 1500, 150);
    // check if any channel buttons are pressed
    solb = readDebounce(solb);
    servo1b = readDebounce(servo1b);
    servo2b = readDebounce(servo2b);
    
    // if so update sensor reading
    if (solb.bang == true) { // read sensor
      sols = setSensor(sols);
    }
    // servo1
    if (servo1b.bang == true) { // read sensor
      servo1s = setSensor(servo1s);
    }
    // servo2
    if (servo2b.bang == true) { // read sensor
      servo2s = setSensor(servo2s);
    }

    // if in play mode and we're on a beat...
    if ((millis() - lastHit) > tempo) {
      // if true then its a beat - figure out which motors to move
      count++;
      lastHit = millis();

      // blink tempo light
      tempoLED = startBlink(tempoLED);

      // trigger servo1
      if (count % servo1s.mappedVal == 0) {
        //    if (count % 3 == 0) {
        servo1LED = startBlink(servo1LED);
        if (servo1State) {
          servo1.write(135);
        }
        else
        {
          servo1.write(45);
        }
        servo1State = !servo1State;
      }

      // trigger servo2
      if (count % servo2s.mappedVal == 0) {
        //    if (count % 2 == 0) {
        // blink the light
        servo2LED = startBlink(servo2LED);
        if (servo2State) {
          servo2.write(45);
        }
        else
        {
          servo2.write(135);
        }
        servo2State = !servo2State;
      }

      if (count % sols.mappedVal == 0){
        // trigger solenoid
        sol = startBlink(sol);
        solLED = startBlink(solLED);
      }


    }
  }

  // turn off LEDs and solenoids if enough time has passed
  tempoLED = checkBlink(tempoLED);

  servo1LED = checkBlink(servo1LED);
  servo2LED = checkBlink(servo2LED);

  sol = checkBlink(sol);
  solLED = checkBlink(solLED);
}

////////////////////////////////////////////////////////////
////////////////////// END MAIN LOOP ///////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// debounce buttons
////////////////////////////////////////////////////////////
buttonStruct readDebounce(buttonStruct button) {

  int reading = digitalRead(button.pin);
  button.bang = false;

  if (reading != button.lastState) {
    // reset timer
    button.lastDebounceTime = millis();
  }

  if ((millis() - button.lastDebounceTime) > debounceTime) {
    {
      if (reading != button.state) {
        button.state = reading;
        if ( button.state == LOW) {
          button.bang = true;
        }
      }
    }
  }
  button.lastState = reading;
  return button;
}

////////////////////////////////////////////////////////////
// LED and solenoid blink functions
// startBlink
////////////////////////////////////////////////////////////
blinkStruct startBlink(blinkStruct led) {
  // set pin HIGH
  digitalWrite(led.pin, HIGH);
  // set state to true
  led.state = true;
  led.lastHit = millis();

  // return updated struct
  return led;
}

////////////////////////////////////////////////////////////
// LED and solenoid blink functions
// checkBlink
// check if its time to turn off
////////////////////////////////////////////////////////////
blinkStruct checkBlink(blinkStruct led) {
  if (led.state && (millis() - led.lastHit > led.onTime))
  {
    led.state = false;
    digitalWrite(led.pin, LOW);
  }

  return led;
}

////////////////////////////////////////////////////////////
// read sensor
////////////////////////////////////////////////////////////
sensorStruct readSensor(sensorStruct s) {
  int val = analogRead(s.pin);

  if (val < s.minVal)
    s.minVal = val;

  if (val > s.maxVal)
    s.maxVal = val;

  return s;
}

sensorStruct setSensor(sensorStruct s) {
  int val = analogRead(s.pin);
  val = map(val, s.minVal, s.maxVal, 0, 8); // map a little beyond the edges and constrain
  s.mappedVal = constrain(val, 1, 8); // just to make sure
  return s;
}











/*  AccelStepper Library
   http://www.airspayce.com/mikem/arduino/AccelStepper/
   http://www.airspayce.com/mikem/arduino/AccelStepper/AccelStepper-1.56.zip


   x -> Horizontal Movement
   y -> Vertical Movement

    __________________
   | 1  2  ....... 20 |
   |                  |
   | Lx Lx ....... Lx |
   | Ly Ly ....... Ly |
   |                  |
   | Rx Rx ....... Rx |
   | Ry Ry ....... Ry |
   |__________________|
            |
            |
    ________|_________
   |                  |
   |  Control Room    |
   |__________________|

  //Todo
  -Linkage Serial To Here
  -Test Actual code with 4 motor and switches
*/
//================ Config ================
#include "PinAssignment.h" //Pin Config
#include "config.h" //Pin Config
//================ Serial ================

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

// ============ STEPPER ================
#include <AccelStepper.h>

long positionArray[numOfStepper];

//float SPEED = 1000;
//float ACCELARATION = 1000;
float SPEED = 500000;
float ACCELARATION = 50000;
float MOVETO = 50;

AccelStepper stepperLx (AccelStepper::DRIVER, lxStep, lxDir);
AccelStepper stepperLy (AccelStepper::DRIVER, lyStep, lyDir);
AccelStepper stepperRx (AccelStepper::DRIVER, rxStep, ryDir);
AccelStepper stepperRy (AccelStepper::DRIVER, ryStep, ryDir);

AccelStepper* steppers[numOfStepper] = { &stepperLx, &stepperLy, &stepperRx, &stepperRy};

// ============ ENCODER ================
#include <Encoder.h>

Encoder encoderLx(encoderLxA, encoderLxB);
Encoder encoderLy(encoderLyA, encoderLyB);
Encoder encoderRx(encoderRxA, encoderRxB);
Encoder encoderRy(encoderRyA, encoderRyB);

Encoder* encoder[numOfStepper] = {&encoderLx, &encoderLy, &encoderRx, &encoderLy};

// ============ LIMIT SWITCH ================
const byte limitSwitch[4]  = {LimitSwitchLx, LimitSwitchLy, LimitSwitchRx, LimitSwitchRy};

//TODO
int val = 0;

// ============ ============ ================
// ============ ====SETUP=== ================
// ============ ============ ================

void setup() {


  // ============ STEPPER ================

  for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
    steppers[stepperNumber]->setMaxSpeed(SPEED);
    steppers[stepperNumber]->setAcceleration(ACCELARATION);
    steppers[stepperNumber]->moveTo(MOVETO);

    //steppers[stepperNumber].setPinsInverted(true, false, false); //(directionInvert,stepInvert,enableInvert)
    // ============ LIMIT SWITCH ================
    pinMode(limitSwitch[stepperNumber], INPUT_PULLUP);
  }

  // ============ SERIAL ================
  Serial.begin(BAUD);

  // ============  ================
   //pinMode(34, INPUT);


}


// ============ ============ ================
// ============ ====LOOP==== ================
// ============ ============ ================

void loop() {


  // ============ SERIAL ================
  serial_decode();
  check_update();


  // ============ STEPPER ================

  for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
    if (steppers[stepperNumber]->distanceToGo() == 0) {

      steppers[stepperNumber]->moveTo(-steppers[stepperNumber]->currentPosition());
      //steppers[stepperNumber]->moveTo(positionArray[stepperNumber]);

    }
  }

  for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
    steppers[stepperNumber]->run();
  }
  // ============ ENCODER ================


  for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
    encoder[stepperNumber]->read(); //TODO, read to sth?
  }

  // ============  ================
 /* unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    }
    else {
      ledState = LOW;
      Serial.println("C");
    }
    // set the LED with the ledState of the variable:

  }
*/

}



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
  -Test Actual code with 4 motor and switches

*/
//================ Config ================
#include "PinAssignment.h" //Pin Config
#include "config.h" //Pin Config
//================ Serial ================

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

long input_value[Input_size];

// ============ STEPPER ================
#include <AccelStepper.h>


long stepperSpeed[numOfStepper]  = {0, 0, 0, 0};
long stepperAccel[numOfStepper]  = {0, 0, 0, 0};
long stepperPos[numOfStepper]  = {0, 0, 0, 0};

AccelStepper stepperLx (AccelStepper::DRIVER, lxStep, lxDir);
AccelStepper stepperLy (AccelStepper::DRIVER, lyStep, lyDir);
AccelStepper stepperRx (AccelStepper::DRIVER, rxStep, ryDir);
AccelStepper stepperRy (AccelStepper::DRIVER, ryStep, ryDir);

AccelStepper* steppers[numOfStepper] = { &stepperLx, &stepperLy, &stepperRx, &stepperRy};

bool isEmergencyStop = false;

//variables
long inverseDir[numOfStepper]  = {0, 0, 0, 0};
long maxPos[numOfStepper]  = {0, 0, 0, 0};
long maxSpeed[numOfStepper] = {0, 0, 0, 0};
long maxAccel[numOfStepper] = {0, 0, 0, 0};



// ============ ENCODER ================
#include <Encoder.h>

Encoder encoderLx(encoderLxA, encoderLxB);
Encoder encoderLy(encoderLyA, encoderLyB);
Encoder encoderRx(encoderRxA, encoderRxB);
Encoder encoderRy(encoderRyA, encoderRyB);

Encoder* encoder[numOfStepper] = {&encoderLx, &encoderLy, &encoderRx, &encoderLy};

// ============ LIMIT SWITCH ================
const byte limitSwitch[numOfStepper]  = {LimitSwitchLx, LimitSwitchLy, LimitSwitchRx, LimitSwitchRy};

//================ Style ================
int style = 0; //0 command to go


//TODO
int val = 0;

// ============ ============ ================
// ============ ====SETUP=== ================
// ============ ============ ================

void setup() {


  // ============ STEPPER ================

  for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
    steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
    steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
    steppers[stepperNumber]->moveTo(stepperPos[stepperNumber]);

    //steppers[stepperNumber].setPinsInverted(true, false, false); //(directionInvert,stepInvert,enableInvert)
    Load_Flash();
    Load_To_Variables();
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
    // == == == == == == SERIAL == == == == == == == ==
    serial_decode();
    check_update();
  
  if(isEmergencyStop){
    //TODO -> check brake
  }
  else if (GO_HOME) { //TODO
    /*
      for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      if (steppers[stepperNumber]->distanceToGo() == 0) {
      steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]); //TODO Change to HOME VAL
      steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);  //TODO Change to HOME VAL
      steppers[stepperNumber]->moveTo(stepperPos[stepperNumber]); //TODO NOT MOVE TO BUT SET TO 0

      //steppers[stepperNumber]->moveTo(-steppers[stepperNumber]->currentPosition());
      //steppers[stepperNumber]->moveTo(positionArray[stepperNumber]);

      }
      }
      }


      for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      steppers[stepperNumber]->run();
      }

    */

    GO_HOME = false;
  } else {



    // ============ STEPPER ================

    stepper_style();
    
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


}


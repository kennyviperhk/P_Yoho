#include <AccelStepper.h>

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


// ============ STEPPER ================

const int stepperAmount = 4;

AccelStepper stepperLx (AccelStepper::DRIVER, 2, 3);
AccelStepper stepperLy (AccelStepper::DRIVER, 5, 6);
AccelStepper stepperRx (AccelStepper::DRIVER, 38, 39);
AccelStepper stepperRy (AccelStepper::DRIVER, 41, 42);


long positionArray[stepperAmount];

float SPEED = 1000;
float ACCELARATION = 1000;


AccelStepper* steppers[stepperAmount] = {
  &stepperLx,
  &stepperLy,
  &stepperRx,
  &stepperRy,
};

//TODO
int val = 0;

void setup() {

  // ============ STEPPER ================

  for (int stepperNumber = 0; stepperNumber < stepperAmount; stepperNumber++) {
    steppers[stepperNumber]->setMaxSpeed(SPEED);
    steppers[stepperNumber]->setAcceleration(ACCELARATION);
  }
}

void loop() {

  // ============ STEPPER ================

  for (int stepperNumber = 0; stepperNumber < stepperAmount; stepperNumber++) {
    steppers[stepperNumber]->moveTo(positionArray[stepperNumber]);
  }

  for (int stepperNumber = 0; stepperNumber < stepperAmount; stepperNumber++) {
    steppers[stepperNumber]->run();
  }

//TODO
  val=20;
}


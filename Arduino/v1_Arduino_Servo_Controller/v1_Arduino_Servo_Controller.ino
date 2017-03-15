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
AccelStepper stepperLy (AccelStepper::DRIVER, 4, 5);
AccelStepper stepperRx (AccelStepper::DRIVER, 6, 7);
AccelStepper stepperRy (AccelStepper::DRIVER, 8, 9);

long positionArray[stepperAmount];

//float SPEED = 1000;
//float ACCELARATION = 1000;
float SPEED = 500000;
float ACCELARATION = 50000;
float MOVETO = 50;

AccelStepper* steppers[stepperAmount] = {
  &stepperLx,
  &stepperLy,
  &stepperRx,
  &stepperRy,
};

// ============ ENCODER ================
#include <Encoder.h>
Encoder enc(22, 23);

Encoder encoderLx(22, 23);
Encoder encoderLy(25, 26);
Encoder encoderRx(28, 29);
Encoder encoderRy(31, 32);

Encoder* encoder[stepperAmount] = {
  &encoderLx,
  &encoderLy,
  &encoderRx,
  &encoderLy,
};

//TODO
int val = 0;

void setup() {


  // ============ STEPPER ================

  for (int stepperNumber = 0; stepperNumber < stepperAmount; stepperNumber++) {
    steppers[stepperNumber]->setMaxSpeed(SPEED);
    steppers[stepperNumber]->setAcceleration(ACCELARATION);
    steppers[stepperNumber]->moveTo(MOVETO);
  }
  //pinMode(34, OUTPUT);

  // ============ SERIAL ================
  Serial.begin(9600);


  pinMode(34, INPUT);
}

void loop() {


  // ============ SERIAL ================
  serial_decode();
  check_update();


  // ============ STEPPER ================

  for (int stepperNumber = 0; stepperNumber < stepperAmount; stepperNumber++) {
    if (steppers[stepperNumber]->distanceToGo() == 0) {

      steppers[stepperNumber]->moveTo(-steppers[stepperNumber]->currentPosition());
      //steppers[stepperNumber]->moveTo(positionArray[stepperNumber]);

    }
  }

  for (int stepperNumber = 0; stepperNumber < stepperAmount; stepperNumber++) {
    steppers[stepperNumber]->run();
  }
  // ============ ENCODER ================

  long newPosition = enc.read();
  for (int stepperNumber = 0; stepperNumber < stepperAmount; stepperNumber++) {
    encoder[stepperNumber]->read(); //TODO, read to sth?
  }


  //TODO
  val = 20;
  //buttonState =
  if (digitalRead(34) == HIGH) {
    Serial.println("Y");
  } else {
    Serial.println("N");
  }
}


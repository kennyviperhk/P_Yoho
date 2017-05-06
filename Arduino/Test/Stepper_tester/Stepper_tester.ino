/*

*/

const int buttonPin = 12;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin


//========= STEPPER ========
#include <AccelStepper.h>
int st = 6;
int dir = 7;
AccelStepper stepper (AccelStepper::DRIVER, st, dir);

long currPos = 0;
long maxPos = 6300;

bool goHome = true;


long homeSpeed = -250;
long homeMaxSpeed = 250;

bool triggerState = true;
bool finish = false;

void setup() {
  //========= Serial ========
  Serial.begin(9600);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  //========
  Serial.println("=== UReady to go home === ");
}

void loop() {

  if (goHome) {
    int reading = digitalRead(buttonPin);
    buttonState = reading;
    digitalWrite(ledPin, buttonState);
    stepper.setMaxSpeed(homeMaxSpeed);
    stepper.setSpeed(homeSpeed);

    if (buttonState) {

      Serial.println("Reached Home");
      stepper.setCurrentPosition(0);
      goHome = false;
    }
    stepper.runSpeed();
  } else {



    if (stepper.distanceToGo() == 0) {
      stepper.setMaxSpeed(350);
      stepper.setAcceleration(350);

      stepper.moveTo(currPos);
      Serial.print("MoveTo : ");
      Serial.println(currPos);


    }
    stepper.run();

  }
}


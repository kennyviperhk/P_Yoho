/*

*/

const int buttonPin = A2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin


//========= STEPPER ========
#include <AccelStepper.h>
int st = 7;
int dir = 6;
AccelStepper stepper (AccelStepper::DRIVER, st, dir);


bool goHome = true;


long homeSpeed = -250;
long homeMaxSpeed = 250;

bool triggerState = true;
bool finish = false;

bool manualMode = true;
bool isLeft = true;
int moveToPos;

void setup() {
  //========= Serial ========
  Serial.begin(57600);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  //========
  Serial.println("=== UReady to go home === ");
}

void loop() {


  if (manualMode) {
    int reading = digitalRead(buttonPin);
    buttonState = reading;
    digitalWrite(ledPin, buttonState);
    if (stepper.distanceToGo() == 0) {

      stepper.setMaxSpeed(200);
      stepper.setAcceleration(200);
      if (isLeft) {
        stepper.moveTo(-moveToPos);
      } else {
        stepper.moveTo(moveToPos);
      }
    
    }
    if (buttonState) {
      stepper.setCurrentPosition(0);

      Serial.println("Reached Home");
         
    }else{
       stepper.run();
      }
    
  }
  else {
    if (goHome) {
      int reading = digitalRead(buttonPin);
      buttonState = reading;
      digitalWrite(ledPin, buttonState);
      stepper.setMaxSpeed(homeMaxSpeed);
      if(isLeft){
      stepper.setSpeed(-homeSpeed);
      }
      else{
         stepper.setSpeed(homeSpeed);
        }
      if (buttonState) {

        Serial.println("Reached Home");
        stepper.setCurrentPosition(0);
        goHome = false;
      }
      stepper.runSpeed();
    }
    else
    {
    }
  }
  while (Serial.available() > 0)
  {
    int inChar = Serial.read();
    int inChar2 = Serial.parseInt();
    if (inChar == 'l') {
      isLeft = true;
      Serial.print("Left : ");
      moveToPos = inChar2;
      Serial.println(moveToPos);
    }
    if (inChar == 'r') {
      isLeft = false;
      Serial.print("Right : ");
      moveToPos = inChar2;
      Serial.println(moveToPos);
    }
    if (inChar == 'm') {
      manualMode = !manualMode;
      Serial.print("Manual Mode : ");
      Serial.println(manualMode);
      stepper.setCurrentPosition(0);
      moveToPos = 0;
    }
  }
}


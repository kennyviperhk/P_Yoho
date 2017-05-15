/*

*/


#define DI1_SERVO_ON_lx 14
#define DI1_SERVO_ON_ly 15
#define DI1_SERVO_ON_rx 16
#define DI1_SERVO_ON_ry 17

#define DI2_ALARM_RESET_lx 50
#define DI2_ALARM_RESET_ly 51
#define DI2_ALARM_RESET_rx 52
#define DI2_ALARM_RESET_ry 53

#define DO1_SERVO_READY_lx 34
#define DO1_SERVO_READY_ly 35
#define DO1_SERVO_READY_rx 36
#define DO1_SERVO_READY_ry 37

#define BrakeLx A4
#define BrakeLy A5
#define BrakeRx A6
#define BrakeRy A7

const int buttonPin0 = A0;     // the number of the pushbutton pin
const int buttonPin1 = A1;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin

int prevReading0 = 0;
int prevReading1 = 0;
//========= STEPPER ========
#include <AccelStepper.h>
int st = 3;
int dir = 2;
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

  pinMode(buttonPin0, INPUT);
  pinMode(buttonPin1, INPUT);
  pinMode(ledPin, OUTPUT);




  //

  pinMode(DI1_SERVO_ON_lx, OUTPUT);
  pinMode(DI1_SERVO_ON_ly, OUTPUT);
  pinMode(DI1_SERVO_ON_rx, OUTPUT);
  pinMode(DI1_SERVO_ON_ry, OUTPUT);

  digitalWrite(DI1_SERVO_ON_lx, HIGH);
  digitalWrite(DI1_SERVO_ON_ly, HIGH);
  digitalWrite(DI1_SERVO_ON_rx, HIGH);
  digitalWrite(DI1_SERVO_ON_ry, HIGH);

  digitalWrite(BrakeLx, LOW);
  digitalWrite(BrakeLy, LOW);
  digitalWrite(BrakeRx, LOW);
  digitalWrite(BrakeRy, LOW);


  //========
  Serial.println("=== UReady to go home === ");
}

void loop() {


  if (manualMode) {


    //int reading = digitalRead(buttonPin);
    int reading0 = analogRead(buttonPin0);
    int reading1 = analogRead(buttonPin1);
    if (reading0 > prevReading0) {
      Serial.print("A0: ");
      Serial.println(reading0);
      prevReading0 = reading0;
    }
        if (reading1 > prevReading1) {
      Serial.print("A1: ");
      Serial.println(reading1);
      prevReading1 = reading1;
    }
    // buttonState = reading;
    digitalWrite(ledPin, buttonState);
    if (stepper.distanceToGo() == 0) {

      stepper.setMaxSpeed(200);
      stepper.setAcceleration(800);
      if (isLeft) {
        stepper.moveTo(-moveToPos);
      } else {
        stepper.moveTo(moveToPos);
      }

    }
    /*
    if (reading0 > 500) {
      stepper.setCurrentPosition(0);
      Serial.println(reading0);
      moveToPos = 0;
      Serial.println("Reached Home A0");

    } else 
    */
    if (reading1 > 500) {
      stepper.setCurrentPosition(0);
      Serial.println(reading1);
      moveToPos = 0;
      Serial.println("Reached Home A1");

    }
    else {
      stepper.run();
    }

  }
  else {
    if (goHome) {
      int reading = digitalRead(buttonPin0);
      buttonState = reading;
      digitalWrite(ledPin, buttonState);
      stepper.setMaxSpeed(homeMaxSpeed);
      if (isLeft) {
        stepper.setSpeed(-homeSpeed);
      }
      else {
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

    digitalWrite(BrakeLx, HIGH);
    digitalWrite(BrakeLy, HIGH);
    digitalWrite(BrakeRx, HIGH);
    digitalWrite(BrakeRy, HIGH);


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


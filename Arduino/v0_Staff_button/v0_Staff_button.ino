
#include "Keyboard.h"
// this constant won't change:
const int buttonPin[] = {2, 3, 4, 5, 6}; // the pin that the pushbutton is attached to
int buttonState[] = {0, 0, 0, 0, 0};
int lastButtonState[] = {0, 0, 0, 0, 0}; // previous state of the button

const int ledPin = 13;       // the pin that the LED is attached to

void setup() {
  // initialize the button pin as a input:

  for (int i = 0 ; i < 5; i++) {
    pinMode(buttonPin[i], INPUT);
  }
  // initialize the LED as an output:

  pinMode(ledPin, OUTPUT);


  // initialize serial communication:
  Serial.begin(9600);
}


void loop() {
  // read the pushbutton input pin:
  for (int i = 0 ; i < 5; i++) {
    buttonState[i] = digitalRead(buttonPin[i]);

    if (buttonState[i] != lastButtonState[i]) {
      if (buttonState[i] == HIGH) {

       // Serial.println("on");
        if (buttonState[i]) {
          char a = 53 + i;
          Keyboard.write(a);
          
        }
      } else {
       // Serial.println("off");
      }

      delay(50);
    }
    lastButtonState[i] = buttonState[i];



  }



}

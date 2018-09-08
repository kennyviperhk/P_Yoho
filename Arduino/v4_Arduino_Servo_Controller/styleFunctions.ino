void runTestStyle(int whichMotor, int XSpeedAccel, int YSpeedAccel, int maxXPos, int maxYPos) {

  if (whichMotor >= 0 && whichMotor < numOfStepper) {

    if (steppers[whichMotor]->distanceToGo() == 0) {

      if (whichMotor == 0 || whichMotor == 2) { //x
        stepperSpeed[whichMotor] = XSpeedAccel;
        stepperAccel[whichMotor] = XSpeedAccel;
        stepperPos[whichMotor] = random(0,  maxXPos);
        Serial.print("Xmdone");
        Serial.print(whichMotor);
        Serial.print("-");
      } else { //Y
        Serial.print("Ymdone");
        Serial.print(whichMotor);
        Serial.print("-");
        stepperSpeed[whichMotor] = YSpeedAccel;
        stepperAccel[whichMotor] = YSpeedAccel;
        stepperPos[whichMotor] = random(0, maxYPos);
      }

      if (stepperPos[whichMotor] >= 0 && stepperAccel[whichMotor] > 0 && stepperSpeed[whichMotor] > 0) {
        steppers[whichMotor]->setMaxSpeed(stepperSpeed[whichMotor]);
        steppers[whichMotor]->setAcceleration(stepperAccel[whichMotor]);
        steppers[whichMotor]->moveTo((inverseDir[whichMotor])*stepperPos[whichMotor]);
      }
    }



  } else {
    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      Serial.print(steppers[stepperNumber]->distanceToGo());
      Serial.print("-");
      if (steppers[stepperNumber]->distanceToGo() == 0) {


        if (stepperNumber == 0 || stepperNumber == 2) { //x
          Serial.print("Xmdone");
          Serial.print(stepperNumber);
          Serial.print("-");
          stepperSpeed[stepperNumber] = XSpeedAccel;
          stepperAccel[stepperNumber] = XSpeedAccel;
          stepperPos[stepperNumber] = random(0,  maxXPos);
        } else { //Y
          Serial.print("Ymdone");
          Serial.print(stepperNumber);
          Serial.print("-");
          stepperSpeed[stepperNumber] = YSpeedAccel;
          stepperAccel[stepperNumber] = YSpeedAccel;
          stepperPos[stepperNumber] = random(0, maxYPos);
        }

        if (stepperPos[stepperNumber] >= 0 && stepperAccel[stepperNumber] > 0 && stepperSpeed[stepperNumber] > 0) {
          steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
          steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
          steppers[stepperNumber]->moveTo((inverseDir[stepperNumber])*stepperPos[stepperNumber]);
          Serial.print("Here");
          Serial.print(stepperNumber);
          Serial.print("-");

          Serial.print(steppers[stepperNumber]->distanceToGo());
          Serial.print("-");
        }
      }
    }
  }

}

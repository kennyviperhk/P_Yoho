void stepper_style() {
  //style 5 //AUTOMATIC TEST RUN FOR ALL FOUR MOTORS
  //style 6 //AUTOMATIC TEST RUN FOR ALL FOUR MOTORS (Half Speed, Half Dist)
  //style 7 //AUTOMATIC TEST RUN FOR Lx
  //style 8 //AUTOMATIC TEST RUN FOR Ly
  //style 9 //AUTOMATIC TEST RUN FOR Rx
  //style 10 //AUTOMATIC TEST RUN FOR Ry
  style = input_value[0];
  if (style == 5) {
    runTestStyle(-1, 2000, 2000, 5000, 10000); // int XSpeedAccel,int YSpeedAccel,int maxXPos,int maxYPos

  }
  if (style == 6) {
    runTestStyle(-1, 2000, 2000, 2500, 5000); // int XSpeedAccel,int YSpeedAccel,int maxXPos,int maxYPos
  }
  if (style == 7) {
    runTestStyle(0, 400, 1000, 2500, 5000); // int XSpeedAccel,int YSpeedAccel,int maxXPos,int maxYPos
  }
  if (style == 8) {
    runTestStyle(1, 400, 1000, 2500, 5000); // int XSpeedAccel,int YSpeedAccel,int maxXPos,int maxYPos
  }
  if (style == 9) {
    runTestStyle(2, 400, 1000, 2500, 5000); // int XSpeedAccel,int YSpeedAccel,int maxXPos,int maxYPos
  }
  if (style == 10) {
    runTestStyle(3, 400, 1000, 2500, 5000); // int XSpeedAccel,int YSpeedAccel,int maxXPos,int maxYPos
  }
  if (style == 1) {
    //STYLE - MOTOR - SPEED - ACCEL - POS
    int stepperNumber = input_value[1];
    stepperSpeed[stepperNumber] = input_value[2];
    stepperAccel[stepperNumber] = input_value[3];
    stepperPos[stepperNumber] = input_value[4];

    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      if (steppers[stepperNumber]->distanceToGo() == 0) {
        steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
        steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
        steppers[stepperNumber]->moveTo((inverseDir[stepperNumber])*stepperPos[stepperNumber]);
      }
    }
  }
  if (style == 2) {
    //STYLE - SPEED_X - ACCEL_X - SPEED_Y - ACCEL_Y - POS1_LX - POS2_LX - POS1_LY - POS2_LY - POS1_RX - POS2_RX - POS1_RY - POS2_RY

    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      if (stepperNumber == 0 || stepperNumber == 2) {
        stepperSpeed[stepperNumber] = input_value[1];
        stepperAccel[stepperNumber] = input_value[2];
      } else {
        stepperSpeed[stepperNumber] = input_value[3];
        stepperAccel[stepperNumber] = input_value[4];
      }
      //abPos[stepperNumber] = true;
    }

    stepperPos1[0] = input_value[5];
    stepperPos2[0] = input_value[6];

    stepperPos1[1] = input_value[7];
    stepperPos2[1] = input_value[8];

    stepperPos1[2] = input_value[9];
    stepperPos2[2] = input_value[10];

    stepperPos1[3] = input_value[11];
    stepperPos2[3] = input_value[12];


    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      if (steppers[stepperNumber]->distanceToGo() == 0) {
        if (stepperPos1[stepperNumber] >= 0 && stepperPos2[stepperNumber] >= 0 && stepperAccel[stepperNumber] > 0 && stepperSpeed[stepperNumber] > 0) {
          steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
          steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
          if (abPos[stepperNumber]) {
            //   Serial.print(stepperNumber);
            //  Serial.println("A");
            steppers[stepperNumber]->moveTo((inverseDir[stepperNumber])*stepperPos1[stepperNumber]);
          } else {
            //           Serial.print(stepperNumber);
            //   Serial.println("B");
            steppers[stepperNumber]->moveTo((inverseDir[stepperNumber])*stepperPos2[stepperNumber]);
          }
          abPos[stepperNumber] = !abPos[stepperNumber];
        }
      }
    }
  }
  if (style == 11) {
    //STYLE - SPEED - ACCEL - POS
    stepperSpeed[0] = input_value[1];
    stepperAccel[0] = input_value[2];
    stepperPos[0] = input_value[3];

    stepperSpeed[1] = input_value[4];
    stepperAccel[1] = input_value[5];
    stepperPos[1] = input_value[6];

    stepperSpeed[2] = input_value[7];
    stepperAccel[2] = input_value[8];
    stepperPos[2] = input_value[9];

    stepperSpeed[3] = input_value[10];
    stepperAccel[3] = input_value[11];
    stepperPos[3] = input_value[12];

    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      if (steppers[stepperNumber]->distanceToGo() == 0) {
        if (stepperPos[stepperNumber] >= 0 && stepperAccel[stepperNumber] > 0 && stepperSpeed[stepperNumber] > 0) {
            /* Serial.print("mdone");
            Serial.print(stepperNumber);
            Serial.print("-");*/
          steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
          steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
          steppers[stepperNumber]->moveTo((inverseDir[stepperNumber])*stepperPos[stepperNumber]);
        }
      }
    }
  }
  if (style == 12) { // Time Specified Movement Controllers


    //STYLE - POS - TIME - POS - TIME - POS - TIME - POS - TIME

    stepperPos[0] = input_value[1];
    stepperTime[0] = input_value[2] / 1000.0;

    stepperPos[1] = input_value[3];
    stepperTime[1] = input_value[4] / 1000.0;

    stepperPos[2] = input_value[5];
    stepperTime[2] = input_value[6] / 1000.0;

    stepperPos[3] = input_value[7];
    stepperTime[3] = input_value[8] / 1000.0;

    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      if (stepperPos[stepperNumber] >= 0 && stepperTime[stepperNumber] > 0) {

        if (steppers[stepperNumber]->isCompleteTotDist()) {
          if (currentMillis - prevStepperMillis[stepperNumber] > stepperTime[stepperNumber]) {
            Serial.print("mdone");
            Serial.print(stepperNumber);
            Serial.print("-");
          }
          prevStepperMillis[stepperNumber] = currentMillis;
          steppers[stepperNumber]->reset((inverseDir[stepperNumber])*stepperPos[stepperNumber], stepperTime[stepperNumber]);
        }
      }
    }
  }
}

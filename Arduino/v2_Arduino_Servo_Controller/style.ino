void stepper_style() {
  style = input_value[0];
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
    //STYLE - MOTOR - SPEED - ACCEL - POS1 - POS2
    int stepperNumber = input_value[1];
    stepperSpeed[stepperNumber] = input_value[2];
    stepperAccel[stepperNumber] = input_value[3];
    long stepperPos1[stepperNumber];
    long stepperPos2[stepperNumber];
    stepperPos1[stepperNumber] = input_value[4];
    stepperPos2[stepperNumber] = input_value[5];

    bool abPos[stepperNumber];
    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      if (steppers[stepperNumber]->distanceToGo() == 0) {
        steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
        steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
        if (abPos[stepperNumber]) {
          steppers[stepperNumber]->moveTo((inverseDir[stepperNumber])*stepperPos1[stepperNumber]);
        } else {
          steppers[stepperNumber]->moveTo((inverseDir[stepperNumber])*stepperPos2[stepperNumber]);
        }
        abPos[stepperNumber] = !abPos[stepperNumber];
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
        steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
        steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
        steppers[stepperNumber]->moveTo((inverseDir[stepperNumber])*stepperPos[stepperNumber]);
        if (stepperNumber == 0) {
          Serial.print("\t invers : ");
          Serial.print((inverseDir[stepperNumber]));
          Serial.print("\t Pos : ");
          Serial.print(stepperPos[stepperNumber]);
          Serial.print("\t A*B : ");
          Serial.println((inverseDir[stepperNumber])*stepperPos[stepperNumber]);
        }
      }
    }
  }
}

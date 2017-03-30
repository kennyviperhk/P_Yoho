void stepper_style(){
    style = input_value[0];
    if (style == 1) {
    //STYLE - MOTOR - SPEED - ACCEL - POS
      int stepperNumber = input_value[1];
      stepperSpeed[stepperNumber] = input_value[2];
      stepperAccel[stepperNumber] = input_value[3];
      stepperMoveTo[stepperNumber] = input_value[4];

      for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
        if (steppers[stepperNumber]->distanceToGo() == 0) {
          steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
          steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
          steppers[stepperNumber]->moveTo(stepperMoveTo[stepperNumber]);
        }
      }
    }
    if (style == 2) {
    //STYLE - MOTOR - SPEED - ACCEL - POS1 - POS2
      int stepperNumber = input_value[1];
      stepperSpeed[stepperNumber] = input_value[2];
      stepperAccel[stepperNumber] = input_value[3];
      long stepperMoveTo1[stepperNumber];
      long stepperMoveTo2[stepperNumber];
      stepperMoveTo1[stepperNumber] = input_value[4];
      stepperMoveTo2[stepperNumber] = input_value[5];

      bool abPos[stepperNumber];
      for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
        if (steppers[stepperNumber]->distanceToGo() == 0) {
          steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
          steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
          if(abPos[stepperNumber]){
            steppers[stepperNumber]->moveTo(stepperMoveTo1[stepperNumber]);
          }else{
            steppers[stepperNumber]->moveTo(stepperMoveTo2[stepperNumber]);
           }
           abPos[stepperNumber] = !abPos[stepperNumber];
        }
      }
    }
    if (style == 11) {
    //STYLE - SPEED - ACCEL - POS
      stepperSpeed[0] = input_value[1];
      stepperAccel[0] = input_value[2];
      stepperMoveTo[0] = input_value[3];

      stepperSpeed[1] = input_value[4];
      stepperAccel[1] = input_value[5];
      stepperMoveTo[1] = input_value[6];

      stepperSpeed[2] = input_value[7];
      stepperAccel[2] = input_value[8];
      stepperMoveTo[2] = input_value[9];

      stepperSpeed[3] = input_value[10];
      stepperAccel[3] = input_value[11];
      stepperMoveTo[3] = input_value[12];

      for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
        if (steppers[stepperNumber]->distanceToGo() == 0) {
          steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
          steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
          steppers[stepperNumber]->moveTo(stepperMoveTo[stepperNumber]);
        }
      }
    }
}

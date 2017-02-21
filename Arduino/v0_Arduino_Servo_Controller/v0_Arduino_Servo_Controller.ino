// Random.pde
// -*- mode: C++ -*-
//
// Make a single stepper perform random changes in speed, position and acceleration
//
// Copyright (C) 2009 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

// Define a stepper and the pins it will use
//AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepper(AccelStepper::DRIVER, 5, 6);
void setup()
{  
  pinMode(4, OUTPUT);
  digitalWrite(4,HIGH);

    stepper.setMaxSpeed(100000000*100000000);
  stepper.setAcceleration(50*500);
  stepper.moveTo(500*1000);
}

void loop()
{
  /*
    if (stepper.distanceToGo() == 0)
    {
	// Random change to speed, position and acceleration
	// Make sure we dont get 0 speed or accelerations
 
	delay(1000);
	stepper.moveTo(rand() % (random(20000000000,10000000000)));
	stepper.setMaxSpeed((rand() % (random(20000000000,10000000000)))*50 + 1);
	stepper.setAcceleration((rand() % (random(20000000000,10000000000)))*50 + 1);
    }
    stepper.run();
    */

   if (stepper.distanceToGo() == 0)
      stepper.moveTo(-stepper.currentPosition());

    stepper.run();
}

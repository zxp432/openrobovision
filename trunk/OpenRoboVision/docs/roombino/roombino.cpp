/*
  roombino.h - ORCP library
  Copyright (C) 2012 Vladimir Savinov.  All rights reserved.
  
 roombino - arduino implementation of ROI
  
  http://robocraft.ru
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/


//******************************************************************************
//* Includes
//******************************************************************************

#include "roombino.h"
#include "HardwareSerial.h"

extern "C" {
#include <string.h>
#include <stdlib.h>
}

//=============================================================
// Constructors
//=============================================================

RoombinoClass::RoombinoClass(Stream &s):
_serial(s)
{
  
}

//=============================================================
// Public Methods
//=============================================================

/* begin method for overriding default serial bitrate */
void RoombinoClass::begin(void)
{
  begin(ROOMBINO_DEFAULT_RATE);
}

/* begin method for overriding default serial bitrate */
void RoombinoClass::begin(long speed)
{
  Serial.begin(speed);
  _serial = Serial;
}

void RoombinoClass::begin(Stream &s)
{
  _serial = s;
}

//=============================================================
// Serial Receive Handling

int RoombinoClass::available(void)
{
  return _serial.available();
}

void RoombinoClass::processInput(void)
{
  int inputData = _serial.read(); // this is 'int' to handle -1 when no data
  int command;
  
  // TODO: make sure it handles -1 properly
  
}

//=============================================================
// Serial Send Handling


// make one instance for the user to use
ORCPClass ORCP(Serial);

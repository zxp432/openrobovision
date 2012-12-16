/*
  roombino.h - arduino implementation of  iRobot Roomba ROI 
  Copyright (C) 2012 Vladimir Savinov.  All rights reserved.
  
 roombino - arduino implementation of ROI
  
  http://robocraft.ru
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef _ROOMBINO_h_
#define _ROOMBINO_h_

#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// default rate
#define ROOMBINO_DEFAULT_RATE 57600

class RoombinoClass
{
public:
	RoombinoClass(Stream &s);
    void begin();
    void begin(long);
    void begin(Stream &s);

    // serial receive handling
    int available(void);
    void processInput(void);

private:
    Stream &_serial;

};

extern RoombinoClass Roombino;

#endif // #ifndef _ROOMBINO_h_

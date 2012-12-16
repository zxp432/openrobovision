/*
 * ORCP - ORV Communication Protocol
 *
 * ORCP is Point-to-Point protocol for interaction with  
 * Arduino/CraftDuino.
 *
 * example of working with servo
 *
 * http://robocraft.ru
 *
 */
 
#include <Servo.h>
#include <orcp.h>

Servo servos[MAX_SERVOS];

void analogWriteCallback(byte pin, int value)
{  
  servos[pin].write(value);
}

void setup() 
{
  byte pin;

  ORCP.attach(CP_DUINO_ANALOGWRITE, analogWriteCallback);

  for (pin=2; pin < 12; pin++)
  {
	servos[pin].attach(pin);
  }
  
  ORCP.begin(57600);
}

void loop() 
{
  while(ORCP.available())
    ORCP.processInput();
}

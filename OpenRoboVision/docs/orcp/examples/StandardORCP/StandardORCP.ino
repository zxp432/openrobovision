/*
 * ORCP - ORV Communication Protocol
 *
 * ORCP is Point-to-Point protocol for interaction with  
 * Arduino/CraftDuino.
 *
 * example of working with MCU
 *
 * http://robocraft.ru
 *
 */
 
#include <Servo.h>
#include <orcp.h>

Servo servos[MAX_SERVOS];

// ======================================================
// CALLBACKS
// ======================================================

// sets the pin mode
void setPinModeCallback(byte pin, int mode)
{
  switch(mode)
  {
  case CP_MODE_ANALOG:
    pinMode(pin, INPUT); // disable output driver
    digitalWrite(pin, LOW); // disable internal pull-ups
    break;
  case CP_MODE_INPUT:
      pinMode(pin, INPUT); // disable output driver
      digitalWrite(pin, LOW); // disable internal pull-ups
    break;
  case CP_MODE_OUTPUT:
      digitalWrite(pin, LOW); // disable PWM
      pinMode(pin, OUTPUT);
    break;
  case CP_MODE_PWM:
      pinMode(pin, OUTPUT);
      analogWrite(pin, 0);
    break;
  case CP_MODE_SERVO:
    if (!servos[pin].attached())
      servos[pin].attach(pin);
    break;
  default:
    break;
  }
} 

void analogWriteCallback(byte pin, int value)
{
  if(servos[pin].attached())
    servos[pin].write(value);
  else
    analogWrite(pin, value);
}

void digitalWriteCallback(byte pin, int value)
{
  digitalWrite(pin, value);
}

void reportAnalogCallback(byte analogPin, int value)
{
  ORCP.sendAnalog(analogPin, analogRead(analogPin));
}

void reportDigitalCallback(byte pin, int value)
{
  ORCP.sendDigital(pin, digitalRead(pin));
}

void systemResetCallback()
{
}

// ======================================================
// SETUP()
// ======================================================

void setup() 
{

  ORCP.attach(CP_DUINO_PINMODE, setPinModeCallback);
  ORCP.attach(CP_DUINO_DIGITALREAD, reportDigitalCallback);
  ORCP.attach(CP_DUINO_ANALOGREAD, reportAnalogCallback);
  ORCP.attach(CP_DUINO_DIGITALWRITE, digitalWriteCallback);
  ORCP.attach(CP_DUINO_ANALOGWRITE, analogWriteCallback);
  ORCP.attach(CP_DUINO_RESET, systemResetCallback);

  ORCP.begin(57600);
  systemResetCallback();  // reset to default config
}

// ======================================================
// LOOP()
// ======================================================
void loop() 
{

  // processing incoming messagse
  while(ORCP.available())
    ORCP.processInput();
}

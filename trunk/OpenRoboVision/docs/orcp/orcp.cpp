/*
  orcp.h - ORCP library
  Copyright (C) 2012 Vladimir Savinov.  All rights reserved.
  
 ORCP - ORV Communication Protocol
 ORCP is Point-to-Point protocol for interaction with Arduino/CraftDuino.
  
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

#include "orcp.h"
#include "HardwareSerial.h"

extern "C" {
#include <string.h>
#include <stdlib.h>
}

// SLIP definitions
#define SLIP_END		0xC0
#define SLIP_ESC		0xDB
#define SLIP_ESC_END	0xDC
#define SLIP_ESC_ESC	0xDD

//=============================================================
// Constructors
//=============================================================

ORCPClass::ORCPClass(Stream &s, byte _id):
ORCPSerial(s), identity_id(_id)
{
  system_reset();
}

//=============================================================
// Public Methods
//=============================================================

/* begin method for overriding default serial bitrate */
void ORCPClass::begin(void)
{
  begin(CP_DEFAULT_RATE);
  identity_id = CP_DUINO_IR;
}

/* begin method for overriding default serial bitrate */
void ORCPClass::begin(long speed, byte _id)
{
  Serial.begin(speed);
  ORCPSerial = Serial;
  identity_id = _id;
}

void ORCPClass::begin(Stream &s, byte _id)
{
  ORCPSerial = s;
  identity_id = _id;
  system_reset();
}

//=============================================================
// Serial Receive Handling

int ORCPClass::available(void)
{
  return ORCPSerial.available();
}

void ORCPClass::processInput(void)
{
  int inputData = ORCPSerial.read(); // this is 'int' to handle -1 when no data
  int command;
  
  // TODO: make sure it handles -1 properly
  
  if(inputData == SLIP_END) // END
  {
    parse_packet();
  }
  else // if(inputData != -1)
  {
    if(input_buffer_size < CP_MAX_DATA_BYTES-2)
	{
	  // decode from SLIP
	  if(inputData == SLIP_ESC) // ESC
	    esc_flag = true;
	  else
	  {
	    if(esc_flag && inputData == SLIP_ESC_ESC) // ESC_ESC
		{
		  input_buffer[input_buffer_size] = SLIP_ESC; // ESC
		  esc_flag = false;
		}
		else if(esc_flag && inputData == SLIP_ESC_END) // ESC_END
		{
		  input_buffer[input_buffer_size] = SLIP_END; // END
		  esc_flag = false;
		}
		else
		{
		  if(esc_flag) // error ( flag was set but we didnt get right second byte)
		    esc_flag = false;
          input_buffer[input_buffer_size] = inputData;
		}
	    ++input_buffer_size;
	  }
	}
	else
	{
	  input_buffer_size = 0;
	}
  }
}

//=============================================================
// Serial Send Handling

// send ORCP packet
void ORCPClass::sendPacket(ORCP_header* h, byte* data, uint8_t data_size)
{
  if(!h)
    return;
  
  uint8_t i;
  uint8_t crc = 0;
  
  send_slip_byte(h->IR);
  send_slip_byte(h->IT);
  send_slip_byte(h->SC);
  send_slip_byte(h->SS);
  
  crc ^= h->IR;
  crc ^= h->IT;
  crc ^= h->SC;
  crc ^= h->SS;

  for(i=0; i<data_size; i++)
  {
    send_slip_byte(data[i]);
	crc ^= data[i];
  }
  send_slip_byte(crc);
  
  ORCPSerial.write(SLIP_END);
}

// send value of digital pin
void ORCPClass::sendDigital(byte pin, int value) 
{
  byte data[2];
  data[0] = pin;
  data[1] = value;
  send_resp(data, 2);
}

// send value of analog pin
void ORCPClass::sendAnalog(byte pin, int value) 
{
  byte data[3];
  data[0] = pin;
  data[1] = (byte)(value >> 8);
  data[2] = (byte)(0xFF & value);
  send_resp(data, 3);
}

// send a string
void ORCPClass::sendString(const char* string) 
{
  send_resp((byte*)string, strlen(string));
}

//=============================================================
// Internal Actions

// generic callbacks
void ORCPClass::attach(byte command, callbackFunction newFunction)
{
  switch(command)
  {
  case CP_DUINO_PINMODE:
	currentPinModeCallback = newFunction;
	break;
  case CP_DUINO_DIGITALREAD:
	currentDigitalReadCallback = newFunction;
	break;
  case CP_DUINO_ANALOGREAD:
	currentAnalogReadCallback = newFunction;
	break;
  case CP_DUINO_DIGITALWRITE:
	currentDigitalWriteCallback = newFunction;
	break;
  case CP_DUINO_ANALOGWRITE:
	currentAnalogWriteCallback = newFunction;
	break;
  }
}

void ORCPClass::attach(byte command, systemResetCallbackFunction newFunction)
{
  switch(command)
  {
  case CP_DUINO_RESET:
    currentSystemResetCallback = newFunction;
    break;
  }
}

void ORCPClass::attach(byte command, stringCallbackFunction newFunction)
{
  switch(command)
  {
  case CP_DUINO_SENDSTRING:
    currentStringCallback = newFunction;
	break;
  }
}

void ORCPClass::detach(byte command)
{
  switch(command)
  {
  case CP_DUINO_RESET:
   currentSystemResetCallback = NULL;
   break;
  case CP_DUINO_SENDSTRING:
    currentStringCallback = NULL;
	break;
  default:
    attach(command, (callbackFunction)NULL);
  }
}

//=============================================================
// Private Methods
//=============================================================

// resets the system state upon a SYSTEM_RESET message from the host software
void ORCPClass::system_reset(void)
{
  if(currentSystemResetCallback)
    (*currentSystemResetCallback)();

  input_buffer_size = 0;
  esc_flag = false;
  header.IT = 0;
  header.SC = 0;
  header.SS = 0;
  //flush(); // TODO: if a subclass of HardwareSerial
}

// parse received packet
void ORCPClass::parse_packet(void)
{
  if(input_buffer_size >= CP_MIN_LENGTH && 
     input_buffer[0] == identity_id)
  {
    // check CRC
	uint8_t crc = input_buffer[input_buffer_size-CP_PACKET_CRC_LENGTH];
	uint8_t mes_crc = calc_crc(input_buffer, input_buffer_size-CP_PACKET_CRC_LENGTH);
	
	if(crc != mes_crc)
	  goto end_parse;
	
    header.IT = input_buffer[1];
	// System Code
	header.SC = input_buffer[2];
    if(header.SC == CP_DUINO_BS)
	{
	  header.SS = input_buffer[3];
	  switch(header.SS) // Base Subsystem Codes
	  {
	    case CP_DUINO_PINMODE:
		  if(currentPinModeCallback)
            (*currentPinModeCallback)(input_buffer[4], input_buffer[5]);
		  break;
		case CP_DUINO_DIGITALREAD:
		  if(currentDigitalReadCallback)
            (*currentDigitalReadCallback)(input_buffer[4], 0);
		  break;
		case CP_DUINO_ANALOGREAD:
		  if(currentAnalogReadCallback)
            (*currentAnalogReadCallback)(input_buffer[4], 0);
		  break;
		case CP_DUINO_DIGITALWRITE:
		  if(currentDigitalWriteCallback)
		    (*currentDigitalWriteCallback)(input_buffer[4], input_buffer[5]);
		  break;
		case CP_DUINO_ANALOGWRITE:
		  if(currentAnalogWriteCallback)
		    (*currentAnalogWriteCallback)(input_buffer[4], (input_buffer[5]<<8 | input_buffer[6]) );
		  break;
		case CP_DUINO_SENDSTRING:
		  if(currentStringCallback)
		  {
		    input_buffer[input_buffer_size-2] = 0;
		    (*currentStringCallback)((char*)(input_buffer+CP_PACKET_HEADER_LENGTH));
		  }
		  break;
	  }
	}
  }
end_parse:  
  // go ready to get next packet
  input_buffer_size = 0;
  esc_flag = false;
}

// send data via ORCP packet
void ORCPClass::send_resp(byte* data, uint8_t data_size)
{
  header.IR = header.IT;
  header.IT = identity_id;
  sendPacket(&header, data, data_size);
}

// send byte into SLIP
void ORCPClass::send_slip_byte(byte data)
{
  // code into SLIP
  if(data == SLIP_ESC)
  {
	ORCPSerial.write(SLIP_ESC);
	ORCPSerial.write(SLIP_ESC_ESC);
  }
  else if(data == SLIP_END)
  {
	ORCPSerial.write(SLIP_ESC);
	ORCPSerial.write(SLIP_ESC_END);
  }
  else
	ORCPSerial.write(data);
}

// calc CRC (XOR of all bytes)
uint8_t ORCPClass::calc_crc(byte* src, uint8_t src_size)
{
  uint8_t crc = 0;
  uint8_t i;
  for(i=0; i<src_size; i++)
    crc ^= src[i];
  return crc;
}

// make one instance for the user to use
ORCPClass ORCP(Serial);

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

#ifndef _CP_h
#define _CP_h

#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// default rate
#define CP_DEFAULT_RATE 57600

// max number of data bytes in packet
#define CP_MAX_DATA_BYTES 64 //32

// packet header and CRC length
#define CP_PACKET_HEADER_LENGTH 4
#define CP_PACKET_CRC_LENGTH 1

// smallest packet length (header + CRC)
#define CP_MIN_LENGTH 5

// identity of transmitter (PC) and receiver (Arduino/CraftDuino)
#define CP_DUINO_IT 0x01
#define CP_DUINO_IR 0x02

// Arduino Base System Code
#define CP_DUINO_BS 0x01
// Base Sunsystem Codes
#define CP_DUINO_RESET 			0x00
#define CP_DUINO_PINMODE		0x01
#define CP_DUINO_DIGITALREAD	0x02
#define CP_DUINO_ANALOGREAD		0x03
#define CP_DUINO_DIGITALWRITE	0x04
#define CP_DUINO_ANALOGWRITE	0x05
#define CP_DUINO_SENDSTRING		0x06

// pin modes
#define CP_MODE_INPUT    0x00
#define CP_MODE_OUTPUT   0x01
#define CP_MODE_ANALOG   0x02
#define CP_MODE_PWM      0x03
#define CP_MODE_SERVO    0x04

// to write a high value
#define CP_LOW	0
// to write a low value
#define CP_HIGH	1

extern "C" {
// callback function types
    typedef void (*callbackFunction)(byte, int);
    typedef void (*systemResetCallbackFunction)(void);
    typedef void (*stringCallbackFunction)(char*);
}

typedef struct ORCP_header
{
	byte IR;
	byte IT;
	byte SC;
	byte SS;
} ORCP_header;

class ORCPClass
{
public:
	ORCPClass(Stream &s, byte identity_id=CP_DUINO_IR);
    void begin();
    void begin(long, byte identity_id=CP_DUINO_IR);
    void begin(Stream &s, byte identity_id=CP_DUINO_IR);

    // serial receive handling
    int available(void);
    void processInput(void);
	// send ORCP packet
	void sendPacket(ORCP_header* header, byte* data, uint8_t data_size);
    // serial send handling
	void sendDigital(byte pin, int value); 
	void sendAnalog(byte pin, int value);
    void sendString(const char* string);
    // attach & detach callback functions to messages
    void attach(byte command, callbackFunction newFunction);
    void attach(byte command, systemResetCallbackFunction newFunction);
    void attach(byte command, stringCallbackFunction newFunction);
    void detach(byte command);

private:
    Stream &ORCPSerial;
    // for store data 
    byte input_buffer[CP_MAX_DATA_BYTES];
	uint8_t input_buffer_size;
	bool esc_flag; // for decode SLIP
	byte identity_id; // id of MC
	ORCP_header header;
    // callback functions
	callbackFunction currentPinModeCallback;
	callbackFunction currentDigitalReadCallback;
    callbackFunction currentAnalogReadCallback;
    callbackFunction currentDigitalWriteCallback;
    callbackFunction currentAnalogWriteCallback;
    stringCallbackFunction currentStringCallback;
	systemResetCallbackFunction currentSystemResetCallback;

//
	void system_reset(void);	
	void parse_packet(void);
	// send data via ORCP packet
	void send_resp(byte* data, uint8_t data_size);
	// send byte into SLIP
	void send_slip_byte(byte data);
	// calc CRC (XOR of all bytes)
	uint8_t calc_crc(byte* src, uint8_t src_size);
};

extern ORCPClass ORCP;

#endif // #ifndef _CP_h

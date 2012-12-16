//
// OpenRoboVision
//
// 
// ORCP - ORV Communication Protocol
// for working with Arduino/CraftDuino
//
//
// robocraft.ru
//

#ifndef _ORV_ORCP_H_
#define _ORV_ORCP_H_

#include "orv/system/types.h"
#include "orv/system/serial.h"

// default output buffer size
#define CP_DEFAULT_BUFFER_SIZE 256
// default input buffer size
#define CP_DEFAULT_INPUT_BUFFER_SIZE 512
// default time for wait data (ms)
#define CP_DEFAULT_WAIT_TIMEOUT 50

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
// Base Subsystem Codes
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

// for store ORCP packet
typedef struct orcp_packet
{
	uint8_t IR;
	uint8_t IT;
	uint8_t SC;
	uint8_t SS;
	uint8_t *data; 
	uint32_t data_size;
	uint8_t crc;

	orcp_packet()
	{
		IR=IT=SC=SS=data_size=crc=0;
		data = NULL;
	}

	void reset()
	{
		IR=IT=SC=SS=data_size=crc=0;
		if(data)
		{
			delete []data;
			data = NULL;
		}
	}

} orcp_packet;

// for work with buffers
typedef struct orcp_buffer
{
	uint8_t	*data;
	uint32_t size;
	uint32_t real_size;

	orcp_buffer()
	{
		data = NULL;
		size = 0;
		real_size = 0;
	}

	// delete buffer
	void reset()
	{
		if(data)
		{
			delete []data;
			data = NULL;
		}
		size = 0;
		real_size = 0;
	}

	void zero()
	{
		if(data)
		{
			memset(data, 0, real_size);
			size = 0;
		}
	}

	// resize buffer if needed size if bigger than real_size
	void resize(uint32_t need_buf_size)
	{
		if(data && real_size >= need_buf_size)
			return;

		// delete old buffer
		reset();

		// allocate new buffer
		data = new uint8_t [need_buf_size];
		if(!data)
		{
			fprintf(stderr, "[!] Error: cant allocate memoty!\n");
			return;
		}
		real_size = need_buf_size;	
	}

} orcp_buffer;

class ORCP
{
public:
	ORCP();
	ORCP(int port, int rate);
	ORCP(const char* port_name, int rate);
	~ORCP();

	// open serial port
	int open(int port, int rate);
	int open(const char* port_name, int rate);
	// close serial port
	void close();

	// create packet from data (docs/orcp.txt)
	uint32_t create_packet(uint8_t IR, uint8_t IT, uint8_t SC, uint8_t SS, uint8_t* data, uint32_t data_size);

	// send packet (docs/orcp.txt)
	uint32_t send_packet(uint8_t IR, uint8_t IT, uint8_t SC, uint8_t SS, uint8_t* data, uint32_t data_size);
	uint32_t send_packet(uint8_t* packet, uint32_t packet_size);
	uint32_t send_packet(orcp_packet* packet);

	// read data from serial
	int update(uint32_t ms=CP_DEFAULT_WAIT_TIMEOUT);
	int parse(uint8_t *buf, uint32_t buf_size);
	orcp_packet* get_input_packet();

	//------------------------------------------------------------
	//
	// Arduino functions 
	//

	// Returns the last known value read from the digital pin: HIGH or LOW.
	int digitalRead(int pin);
	// Returns the last known value read from the analog pin: 0 (0 volts) to 1023 (5 volts).
	int analogRead(int pin);
	// Set a digital pin to input or output mode.
	void pinMode(int pin, int mode);
	// Write to a digital pin (the pin must have been put into output mode with pinMode()).
	void digitalWrite(int pin, int value);
	// Write an analog value (PWM-wave) to a digital pin.
	void analogWrite(int pin, int value);
	// Send string
	void sendString(const char* str);
	//------------------------------------------------------------

protected:
	void init();

	Serial serial;
	// buffer
	orcp_buffer out_buffer;
	// SLIP buffer
	orcp_buffer slip_buffer;
	// input buffer
	orcp_buffer input_buffer;
	orcp_buffer input_slip_buffer;
	orcp_buffer input_message;
	// packet
	orcp_packet input_packet;
};

#endif //#ifndef _ORV_ORCP_H_

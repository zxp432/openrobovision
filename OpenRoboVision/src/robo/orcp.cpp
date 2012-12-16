//
// OpenRoboVision
//
// 
// OCP - ORV Communication Protocol
// for working with Arduino/CraftDuino
//
//
// robocraft.ru
//

#include "orv/robo/orcp.h"
#include "orv/system/crypto.h"
#include "orv/system/slip.h"

ORCP::ORCP()
{
	init();
}

ORCP::ORCP(int port, int rate)
{
	init();
	if( this->open(port, rate) )
		printf("[!][CommunicationProtocol] Error: cant open port: %d \n", port);
}

ORCP::ORCP(const char* port_name, int rate)
{
	init();
	if( this->open(port_name, rate) )
		printf("[!][CommunicationProtocol] Error: cant open port: %s \n", port_name);
}

ORCP::~ORCP()
{
	close();
	out_buffer.reset();
	slip_buffer.reset();
	input_buffer.reset();
	input_slip_buffer.reset();
	input_message.reset();
	input_packet.reset();
}

void ORCP::init()
{
	out_buffer.resize(CP_DEFAULT_BUFFER_SIZE);
	slip_buffer.resize(CP_DEFAULT_BUFFER_SIZE);
	input_buffer.resize(CP_DEFAULT_INPUT_BUFFER_SIZE);
	input_slip_buffer.resize(CP_DEFAULT_INPUT_BUFFER_SIZE);
	input_message.resize(CP_DEFAULT_INPUT_BUFFER_SIZE);
}

// open serial port
int ORCP::open(int _port, int _rate)
{
	return serial.open(_port, _rate);
}

int ORCP::open(const char* port_name, int _rate)
{
	return serial.open(port_name, _rate);
}

// close serial port
void ORCP::close()
{
	serial.close();
}

// create packet from data (docs/pp-protocol.txt)
uint32_t ORCP::create_packet(uint8_t IR, uint8_t IT, uint8_t SC, uint8_t SS, uint8_t* data, uint32_t data_size)
{
	// get needed buffer size
	uint32_t needed_size = CP_PACKET_HEADER_LENGTH + data_size + CP_PACKET_CRC_LENGTH; // header + data length + CRC 
	out_buffer.resize(needed_size);
	
	if(!out_buffer.data)
		return 0;
	
	// create packet in buffer
	out_buffer.size = 0;
	// header
	out_buffer.data[0] = IR;
	out_buffer.data[1] = IT;
	out_buffer.data[2] = SC;
	out_buffer.data[3] = SS;
	out_buffer.size += CP_PACKET_HEADER_LENGTH;

	if(data && data_size > 0)
	{
		// data
		memcpy(out_buffer.data+out_buffer.size, data, data_size);
		out_buffer.size += data_size;
	}

	// CRC
	uint8_t crc = orv::crypto::xor8(out_buffer.data, out_buffer.size);
	out_buffer.data[out_buffer.size] = crc;
	out_buffer.size += CP_PACKET_CRC_LENGTH;

	return out_buffer.size;
}

// send packet (docs/orcp.txt)
uint32_t ORCP::send_packet(uint8_t IR, uint8_t IT, uint8_t SC, uint8_t SS, uint8_t* data, uint32_t data_size)
{
	if(!data || data_size == 0)
		return 0;

	// create packet in buffer
	if(create_packet(IR, IT, SC, SS, data, data_size) == 0)
		return 0;

	return send_packet(out_buffer.data, out_buffer.size);
}

uint32_t ORCP::send_packet(uint8_t* packet, uint32_t packet_size)
{
	if(!packet || packet_size == 0)
		return 0;
	
	// get needed SLIP buffer size
	uint32_t needed_slip_size = orv::SLIP::get_code_size(packet, packet_size);
	slip_buffer.resize(needed_slip_size);

	// code into SLIP
	slip_buffer.size = orv::SLIP::code(packet, packet_size, slip_buffer.data);

	if(slip_buffer.size>0)
		return serial.write(slip_buffer.data, slip_buffer.size);

	return 0;
}

uint32_t ORCP::send_packet(orcp_packet* p)
{
	if(!p)
		return 0;

	// create packet in buffer
	if(create_packet(p->IR, p->IT, p->SC, p->SS, p->data, p->data_size) == 0)
		return 0;

	return send_packet(out_buffer.data, out_buffer.size);

}

// read data from serial
int ORCP::update(uint32_t ms)
{
	int res = serial.waitInput(ms);
	if (res > 0)
	{
		if(res = serial.available())
		{
			// check 
			if(input_buffer.size + res < input_buffer.real_size)
			{
				if( res = serial.read(input_buffer.data+input_buffer.size, input_buffer.real_size) )
				{
					input_buffer.size += res;
					// search SLIP
					if( res = orv::SLIP::find_in_data(input_buffer.data, input_buffer.size, input_slip_buffer.data, input_slip_buffer.size) )
						return parse(input_slip_buffer.data, res);
				}
			}
			else
			{
				printf("[!][ORCP][update] too much data!\n"); 
				serial.discardInput();
				input_buffer.zero();
			}
		}
		else if (res < 0)
		{
			printf("[i][ORCP] no data!");
			return 0;
		}  
	}
	return 0;
}

int ORCP::parse(uint8_t *buf, uint32_t buf_size)
{
#if 0
	// for debug
	for (int i=0; i < buf_size; i++) 
		printf("%02X ", buf[i]);
	printf("\n");
#endif

	// decode
	input_message.size = orv::SLIP::decode(buf, buf_size, input_message.data);

	if(input_message.size < CP_MIN_LENGTH) //6
		return 0;

	input_packet.reset();

	// check CRC
	input_packet.crc = input_message.data[input_message.size-CP_PACKET_CRC_LENGTH];
	
	uint16_t mes_crc = orv::crypto::xor8(input_message.data, input_message.size-CP_PACKET_CRC_LENGTH);

	if(input_packet.crc != mes_crc)
	{
		//printf("[i][ORCP] Warning: bad CRC: 0x%0X vs 0x%0X \n", input_packet.crc, mes_crc);
		return 0;
	}

	input_packet.IR = input_message.data[0];
	input_packet.IT = input_message.data[1];
	input_packet.SC = input_message.data[2];
	input_packet.SS = input_message.data[3];
	input_packet.data = new uint8_t[input_message.size-CP_MIN_LENGTH];
	if(!input_packet.data)
		return 0;
	memcpy(input_packet.data, input_message.data+CP_PACKET_HEADER_LENGTH, input_message.size-CP_MIN_LENGTH);
	input_packet.data_size = input_message.size-CP_MIN_LENGTH;
	
	return input_message.size;
}

orcp_packet* ORCP::get_input_packet()
{
	return &input_packet;
}

//------------------------------------------------------------
//
// Arduino functions 
//

// Returns the last known value read from the digital pin: HIGH or LOW.
int ORCP::digitalRead(int pin)
{
	uint8_t m = (uint8_t)(0xFF & pin);
	send_packet(CP_DUINO_IR, CP_DUINO_IT, CP_DUINO_BS, CP_DUINO_DIGITALREAD, &m, 1);

	if( update() > 0)
	{
		if( input_packet.IR == CP_DUINO_IT &&
			input_packet.IT == CP_DUINO_IR && 
			input_packet.SC == CP_DUINO_BS && 
			input_packet.SS == CP_DUINO_DIGITALREAD &&
			(input_packet.data && input_packet.data[0] == m) )
			return input_packet.data[1];
	}

	return 0;
}

// Returns the last known value read from the analog pin: 0 (0 volts) to 1023 (5 volts).
int ORCP::analogRead(int pin)
{
	uint8_t m = (uint8_t)(0xFF & pin);
	send_packet(CP_DUINO_IR, CP_DUINO_IT, CP_DUINO_BS, CP_DUINO_ANALOGREAD, &m, 1);

	if( update() > 0)
	{
		if( input_packet.IR == CP_DUINO_IT &&
			input_packet.IT == CP_DUINO_IR && 
			input_packet.SC == CP_DUINO_BS && 
			input_packet.SS == CP_DUINO_ANALOGREAD &&
			(input_packet.data && input_packet.data[0] == m) )
			return (input_packet.data[1]<<8 | input_packet.data[2]);
	}

	return 0;
}

// Set a digital pin to input or output mode.
void ORCP::pinMode(int pin, int mode)
{
	uint8_t m[2];
	m[0] = (uint8_t)(0xFF & pin);
	m[1] = (uint8_t)(0xFF & mode);
	send_packet(CP_DUINO_IR, CP_DUINO_IT, CP_DUINO_BS, CP_DUINO_PINMODE, m, 2);
}

// Write to a digital pin (the pin must have been put into output mode with pinMode()).
void ORCP::digitalWrite(int pin, int value)
{
	uint8_t m[2];
	m[0] = (uint8_t)(0xFF & pin);
	m[1] = (uint8_t)(0xFF & value);
	send_packet(CP_DUINO_IR, CP_DUINO_IT, CP_DUINO_BS, CP_DUINO_DIGITALWRITE, m, 2);
}

// Write an analog value (PWM-wave) to a digital pin.
void ORCP::analogWrite(int pin, int value)
{
	uint8_t m[3];
	m[0] = (uint8_t)(0xFF & pin);
	m[1] = (uint8_t)(value >> 8);
	m[2] = (uint8_t)(0xFF & value);
	send_packet(CP_DUINO_IR, CP_DUINO_IT, CP_DUINO_BS, CP_DUINO_ANALOGWRITE, m, 3);
}

// Send string
void ORCP::sendString(const char* str)
{
	if(str)
		send_packet(CP_DUINO_IR, CP_DUINO_IT, CP_DUINO_BS, CP_DUINO_SENDSTRING, (uint8_t*)str, strlen(str));
}

//------------------------------------------------------------

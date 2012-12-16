//
// OpenRoboVision
//
// test SLIP routines
//
//
// robocraft.ru
//

#include "gtest/gtest.h"

#include "orv/system/slip.h"

TEST(slip, code)
{
	// for store packet
	uint8_t packet[256];
	uint32_t packet_size = 0;

	uint8_t mess[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
	uint32_t mess_size = 9;
	packet_size = orv::SLIP::code(mess, mess_size, packet);
	EXPECT_EQ(10, packet_size);

	// 0xC0
	uint8_t mess1[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xC0};
	uint32_t mess1_size = sizeof(mess1);
	memset(packet, 0, 256); packet_size = 0;
	packet_size = orv::SLIP::code(mess1, mess1_size, packet);
	EXPECT_EQ(12, packet_size);
	EXPECT_EQ(0xDB, packet[9]);
	EXPECT_EQ(0xDC, packet[10]);

	// 0xDB
	uint8_t mess2[] = {0xDB, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
	uint32_t mess2_size = sizeof(mess2);
	memset(packet, 0, 256); packet_size = 0;
	packet_size = orv::SLIP::code(mess2, mess2_size, packet);
	EXPECT_EQ(12, packet_size);
	EXPECT_EQ(0xDB, packet[0]);
	EXPECT_EQ(0xDD, packet[1]);
}

TEST(slip, decode)
{
	// for store data
	uint8_t data[256];
	uint32_t data_size = 0;

	// no SLIP data
	uint8_t mess[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
	uint32_t mess_size = 9;
	data_size = orv::SLIP::decode(mess, mess_size, data);
	EXPECT_EQ(0, data_size);

	// SLIP data
	uint8_t mess1[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xC0};
	uint32_t mess1_size = sizeof(mess1);
	memset(data, 0, 256); data_size = 0;
	data_size = orv::SLIP::decode(mess1, mess1_size, data);
	EXPECT_EQ(9, data_size);

	// first END
	uint8_t mess2[] = {0xC0, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xC0};
	uint32_t mess2_size = sizeof(mess2);
	memset(data, 0, 256); data_size = 0;
	data_size = orv::SLIP::decode(mess2, mess2_size, data);
	EXPECT_EQ(0, data_size);

}

TEST(slip, find_packet)
{
	// for store SLIP packet
	uint8_t data[256];
	uint32_t data_size = 0;

	// no SLIP data
	uint8_t mess[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
	uint32_t mess_size = 9;
	orv::SLIP::find_in_data(mess, mess_size, data, data_size);
	EXPECT_EQ(0, data_size);

	// SLIP data
	uint8_t mess1[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xC0};
	uint32_t mess1_size = sizeof(mess1);
	memset(data, 0, 256); data_size = 0;
	orv::SLIP::find_in_data(mess1, mess1_size, data, data_size);
	EXPECT_EQ(10, data_size);

	// first END
	uint8_t mess2[] = {0xC0, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xC0};
	uint32_t mess2_size = sizeof(mess2);
	memset(data, 0, 256); data_size = 0;
	orv::SLIP::find_in_data(mess2, mess2_size, data, data_size);
	EXPECT_EQ(1, data_size);

}

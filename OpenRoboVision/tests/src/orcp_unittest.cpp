//
// OpenRoboVision
//
// test ORCP
//
//
// robocraft.ru
//

#include "gtest/gtest.h"

#include "orv/robo/orcp.h"

TEST(ORCP, create_packet)
{
	ORCP orcp;

	uint8_t mess[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
	uint32_t mess_size = 9;

	uint32_t packet_size = orcp.create_packet(1, 2, 3, 4, mess, mess_size);
	EXPECT_EQ(14, packet_size);
}

TEST(ORCP, parse)
{
	ORCP orcp;

	// bad CRC
	uint8_t mess[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xC0};
	uint32_t mess_size = 10;

	int res = orcp.parse(mess, mess_size);
	EXPECT_EQ(0, res);

	// good CRC
	uint8_t mess1[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x29, 0x18, 0xC0};
	uint32_t mess1_size = 12;

	res = orcp.parse(mess1, mess1_size);
	EXPECT_EQ(11, res);

	// zero package
	uint8_t mess2[] = {0xC0, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x29, 0xB1, 0xC0};
	uint32_t mess2_size = 13;

	res = orcp.parse(mess2, mess2_size);
	EXPECT_EQ(0, res);

	// too small package
	uint8_t mess3[] = {0x31, 0x32, 0x33, 0x34, 0xC0};
	uint32_t mess3_size = 5;

	res = orcp.parse(mess3, mess3_size);
	EXPECT_EQ(0, res);
}

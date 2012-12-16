//
// OpenRoboVision
//
// test crypto routines
//
//
// robocraft.ru
//

#include "gtest/gtest.h"

#include "orv/system/types.h"
#include "orv/system/crypto.h"

TEST(crypto, crc16)
{
	uint8_t test_str[] = "123456789";

	uint16_t _crc16 = orv::crypto::crc16(test_str, 9);
	EXPECT_EQ(0x29B1, _crc16);
}

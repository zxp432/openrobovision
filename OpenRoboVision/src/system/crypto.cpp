//
// OpenRoboVision
//
// crypto routines
//
// robocraft.ru
//

#include "orv/system/crypto.h"

// XOR of all bytes
uint8_t orv::crypto::xor8(uint8_t *src, uint32_t src_size)
{
	if(!src || src_size==0)
		return 0;

	uint8_t crc = 0;
	for(uint32_t i=0; i< src_size; i++)
		crc ^= src[i];
	return crc;
}

// Name  : CRC-16 CCITT
// Poly  : 0x1021    x^16 + x^12 + x^5 + 1
// Init  : 0xFFFF
// Revert: false
// XorOut: 0x0000
// Check : 0x29B1 ("123456789")
// MaxLen: 4095 byte (32767 bit) 
uint16_t orv::crypto::crc16(uint8_t *pdata, uint16_t size)
{
	uint16_t crc = 0xFFFF;
	uint8_t i;

	while( size-- )
	{
		crc ^= *pdata++ << 8;

		for( i = 0; i < 8; i++ )
		{
			crc = crc & 0x8000 ? ( crc << 1 ) ^ 0x1021 : crc << 1;
		}
	}

	return crc;
}

uint32_t orv::crypto::crc32(uint8_t *pdata, uint32_t size)
{
	uint32_t crc = CRC32_INIT_VALUE;

    while (size-- > 0)
    {
        crc = (crc >> 8) ^ crc32_table[(crc ^ *pdata++) & 0xff];
    }

    return crc;
}

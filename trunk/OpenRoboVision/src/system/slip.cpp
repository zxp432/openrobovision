//
// OpenRoboVision
//
// SLIP routines
// SLIP - Serial Line Internet Protocol (SLIP)
//
// robocraft.ru
//

/******************************************

Serial Line Internet Protocol (SLIP)

SLIP define 4 special symbols:
END, ESC, ESC_END and ESC_ESC:

symbol    value (hexademical)
----------------------------
END       0xC0
ESC       0xDB
ESC_END   0xDC
ESC_ESC   0xDD

Bytes ESC (0xDB) and END (0xC0) are filtered from data by process of byte stuffling. 
If ESC (0xDB) or END (0xC0) appears in the data, the ESC symbol (0xDB) is send, followed by ESC_ESC (0xDD) or ESC_END (0xDC), correspondingly.
I. e. the data byte replaced by two bytes:
0xDB -> 0xDB 0xDD
0xC0 -> 0xDB 0xDC

********************************************/

#include "orv/system/slip.h"

#define SLIP_END		0xC0
#define SLIP_ESC		0xDB
#define SLIP_ESC_END	0xDC
#define SLIP_ESC_ESC	0xDD

// code buffer into SLIP-packet
uint32_t orv::SLIP::code(uint8_t *src, uint32_t src_size, uint8_t *dst)
{
	if(!src || src_size==0 || !dst)
		return 0;

	uint32_t dst_size=0;
	
	//dst[0] = (uint8_t)SLIP_END;
	//dst_size++;

	for(uint32_t i=0; i<src_size; i++)
	{
		if(src[i] == (uint8_t)SLIP_ESC)
		{
			dst[dst_size] = (uint8_t)SLIP_ESC;
			dst[dst_size+1] = (uint8_t)SLIP_ESC_ESC;
			dst_size+=2;
		}
		else if(src[i] == (uint8_t)SLIP_END)
		{
			dst[dst_size] = (uint8_t)SLIP_ESC;
			dst[dst_size+1] = (uint8_t)SLIP_ESC_END;
			dst_size+=2;
		}
		else
		{
			dst[dst_size] = src[i];
			++dst_size;
		}
	}
	dst[dst_size] = (uint8_t)SLIP_END;
	++dst_size;

	return dst_size;
}

// decode SLIP-packet into buffer
uint32_t orv::SLIP::decode(uint8_t *src, uint32_t src_size, uint8_t *dst)
{
	if(!src || src_size==0 || !dst)
		return 0;

	uint32_t dst_size=0;

	// check last byte
	if(src[src_size-1]!=(uint8_t)SLIP_END)
		return 0;

	for(uint32_t i=0; i<src_size-1; i++)
	{
		if(src[i]==(uint8_t)SLIP_END)
			break;
		if(src[i]==(uint8_t)SLIP_ESC && src[i+1]==(uint8_t)SLIP_ESC_ESC)
		{
			dst[dst_size] = (uint8_t)SLIP_ESC;
			++i;
		}
		else if( src[i]==(uint8_t)SLIP_ESC && src[i+1]==(uint8_t)SLIP_ESC_END )
		{
			dst[dst_size] = (uint8_t)SLIP_END;
			++i;
		}
		else
		{
			dst[dst_size] = src[i];
		}
		++dst_size;
	}

	return dst_size;
}

// get size of SLIP-packet
uint32_t orv::SLIP::get_code_size(uint8_t *src, uint32_t src_size)
{
	if(!src || src_size==0)
		return 0;

	uint32_t dst_size=0;

	for(uint32_t i=0; i<src_size; i++)
	{
		if(src[i] == (uint8_t)SLIP_ESC || src[i] == (uint8_t)SLIP_END)
			dst_size+=2;
		else
			++dst_size;
	}
	++dst_size; // SLIP_END

	return dst_size;
}

// find SLIP-packet in buffer
uint32_t orv::SLIP::find_in_data(uint8_t *src, uint32_t &src_size, uint8_t *dst, uint32_t &dst_size)
{
	if(!src || src_size==0 || !dst)
		return 0;

	dst_size = 0;

	uint32_t i=0, j=0;
	bool slip_is_done = false;

	// search SLIP-packet
	for(i=0; i < src_size; i++)
	{
		if(src[i] == (uint8_t)SLIP_END)
			slip_is_done = true;

		dst[dst_size] = src[i];
		++dst_size;

		if(slip_is_done){
			break;
		}
	}

	if(!slip_is_done)
	{
		dst_size = 0;
	}
	else
	{
		// move data to buffer begin
		for(j=0, i=dst_size; i<src_size; i++, j++)
			src[j] = src[i];

		src_size = j;
	}

	return dst_size;
}

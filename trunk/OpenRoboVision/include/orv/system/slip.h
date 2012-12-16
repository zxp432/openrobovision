//
// OpenRoboVision
//
// SLIP routines
// SLIP - Serial Line Internet Protocol (SLIP)
//
// robocraft.ru
//

#ifndef _ORV_SLIP_H_
#define _ORV_SLIP_H_

#include "types.h"

namespace orv
{
	namespace SLIP
	{
		// code buffer into SLIP-packet
		uint32_t code(uint8_t *src, uint32_t src_size, uint8_t *dst);

		// decode SLIP-packet into buffer
		uint32_t decode(uint8_t *src, uint32_t src_size, uint8_t *dst);

		// get size of SLIP-packet
		uint32_t get_code_size(uint8_t *src, uint32_t src_size);

		// find SLIP-packet in buffer
		uint32_t find_in_data(uint8_t *src, uint32_t &src_size, uint8_t *dst, uint32_t &dst_size);

	}; // namespace SLIP
}; //namespace orv

#endif //#ifndef _ORV_SLIP_H_

#ifndef __ROUNDING__H__
#define __ROUNDING__H__


#include "Data/DataBuffer.h"

class Rounding
{
public:
	static void rtr_lrint(rfa::data::Real64 * oReal, rfa::common::Double * iValue, rfa::common::UInt8 iHint);
	static void rtr_lrintf(rfa::data::Real32 * oReal, rfa::common::Float * iValue, rfa::common::UInt8 iHint);
};

#endif //  __ROUNDING_EXAMPLE__H__

#include "Rounding.h"
#include <math.h>

using namespace rfa::common;
using namespace rfa::data;

void Rounding::rtr_lrint(Real64 * oReal, Double * iValue, UInt8 iHint)
{
	//Pass in double value, shift decimal 2 places to the right.
	double shiftedDouble = (*iValue)/(pow((double)10, (double)(iHint - Exponent0)));
#ifdef WIN32
	//The floor represents the largest integer that is less than or equal
	//to the variable shiftedDouble. The floor function is defined in math.h.
	//Note that adding 0.5 will always cause the value to round up.
	Int64 val = (Int64)floor(shiftedDouble + 0.5);
#else
	//Round argument to the nearest integer value, using the current rounding direction. If x is 
	//infinite or NaN, or if the rounded value is outside the range of the return type, the numeric  
	//result is unspecified.
	Int64 val = (Int64)llrint(shiftedDouble);

#endif
	oReal->setValue(val);
	oReal->setMagnitudeType(iHint);
}

void Rounding::rtr_lrintf(Real32 * oReal, Float * iValue, UInt8 iHint)
{
	Float shiftedFloat = (*iValue)/(pow((float)10, (float)(iHint - Exponent0)));
#ifdef WIN32
	//The floor represents the largest integer that is less than or equal
	//to the variable shiftedDouble. The floor function is defined in math.h.
	//Note that adding 0.5 will always cause the value to round up.
	Int32 val = (Int32)floor(shiftedFloat + 0.5f);
#else
	//Round argument to the nearest integer value, using the current rounding direction. If x is 
	//infinite or NaN, or if the rounded value is outside the range of the return type, the numeric  
	//result is unspecified.
	Int32 val = lrintf(shiftedFloat);

#endif
	oReal->setValue(val);
	oReal->setMagnitudeType(iHint);
}

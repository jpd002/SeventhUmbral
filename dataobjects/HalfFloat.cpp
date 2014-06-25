#include "HalfFloat.h"

float CHalfFloat::ToFloat(uint16 half)
{
	uint32 result = 0;
	if(half & 0x8000) result |= 0x80000000;
	uint8 exponent = ((half >> 10) & 0x1F);
	//Flush denormal to zero
	if(exponent == 0) return *reinterpret_cast<float*>(&result);
	exponent -= 0xF;	//Convert to absolute exponent
	exponent += 0x7F;	//Then to float exponent
	uint32 mantissa = (half & 0x3FF);
	result |= exponent << 23;
	result |= mantissa << 13;
	return *reinterpret_cast<float*>(&result);
}

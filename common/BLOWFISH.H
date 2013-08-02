#include "Types.h"

#define MAXKEYBYTES 56          /* 448 bits */
#define little_endian 1              /* Eg: Intel */
//#define big_endian 1            /* Eg: Motorola */

#ifdef __cplusplus
extern "C"
{
#endif
	int16 opensubkeyfile(void);
	uint32 F(uint32 x);
	void Blowfish_encipher(uint32 *xl, uint32 *xr);
	void Blowfish_decipher(uint32 *xl, uint32 *xr);
	short InitializeBlowfish(char key[], int16 keybytes);
#ifdef __cplusplus
}
#endif

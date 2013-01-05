#define MAXKEYBYTES 56          /* 448 bits */
#define little_endian 1              /* Eg: Intel */
//#define big_endian 1            /* Eg: Motorola */

#ifdef __cplusplus
extern "C"
{
#endif
	short opensubkeyfile(void);
	unsigned long F(unsigned long x);
	void Blowfish_encipher(unsigned long *xl, unsigned long *xr);
	void Blowfish_decipher(unsigned long *xl, unsigned long *xr);
	short InitializeBlowfish(char key[], short keybytes);
#ifdef __cplusplus
}
#endif

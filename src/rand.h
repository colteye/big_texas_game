#include <stdint.h>

static uint16_t lfsr = 0xACE1u;
uint8_t rand(uint8_t seed)
{
	uint16_t bit = ((lfsr >> 0) ^ (lfsr + seed >> 2) ^ (lfsr >> 3) ^ (lfsr + seed >> 5)) & 1;
	lfsr = ((lfsr >> 1) | (bit << 15));
	return (lfsr % 2 == 0);
}
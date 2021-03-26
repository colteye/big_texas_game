#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>
#include <stdio.h>

struct cbt_sprite
{
	uint8_t width;
	uint8_t height;
	uint8_t *data;
};

struct cbta_sprite
{
	uint8_t width;
	uint8_t height;
	uint8_t *data;
	uint8_t *mask;
};

struct cbt_sprite open_cbt_sprite(const char *fname);
struct cbta_sprite open_cbta_sprite(const char *fname);

#endif /* SPRITE_H */
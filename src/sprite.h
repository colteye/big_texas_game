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

struct cbt_sprite cbt_sprite_open(const char *fname);
struct cbta_sprite cbta_sprite_open(const char *fname);

#endif /* SPRITE_H */
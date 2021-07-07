#include "sprite.h"
#include "stdlib.h"
#include "error_handling.h"

struct cbt_sprite cbt_sprite_open(const char *fname)
{
	struct cbt_sprite out = { 0, 0, NULL };
	if (is_null((char *)fname)) return out;

	FILE *fp = fopen(fname, "rb");

	uint8_t head[2];
	fread(head, 1, 2, fp);

	uint16_t img_size = head[0] * head[1] / 8;

	uint8_t *data = (uint8_t *)malloc(img_size);

	fread(data, 1, img_size, fp);

	out.width = head[0];
	out.height = head[1];
	out.data = data;
	return out;
}

struct cbta_sprite cbta_sprite_open(const char *fname)
{
	struct cbta_sprite out = { 0, 0, NULL, NULL };
	if (is_null((char *)fname)) return out;

	FILE *fp = fopen(fname, "rb");

	uint8_t head[2];
	fread(head, 1, 2, fp);

	uint16_t img_size = head[0] * head[1] / 8;

	uint8_t *data = (uint8_t *)malloc(img_size);
	uint8_t *mask = (uint8_t *)malloc(img_size);

	fread(data, 1, img_size, fp);
	fread(mask, 1, img_size, fp);

	out.width = head[0];
	out.height = head[1];
	out.data = data;
	out.mask = mask;
	return out;
}
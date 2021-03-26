#include "sprite.h"

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

struct cbt_sprite open_cbt_sprite(const char *fname)
{
	FILE *fp = fopen(fname, "rb");

	uint8_t head[2];
	fread(head, 1, 2, fp);

	uint16_t img_size = head[0] * head[1] / 8;

	uint8_t *data = (uint8_t *)malloc(img_size);

	fread(data, 1, img_size, fp);

	struct cbt_sprite out = { head[0], head[1], data };
	return out;
}

struct cbta_sprite open_cbta_sprite(const char *fname)
{
	FILE *fp = fopen(fname, "rb");

	uint8_t head[2];
	fread(head, 1, 2, fp);

	uint16_t img_size = head[0] * head[1] / 8;

	uint8_t *data = (uint8_t *)malloc(img_size);
	uint8_t *mask = (uint8_t *)malloc(img_size);

	fread(data, 1, img_size, fp);
	fread(mask, 1, img_size, fp);

	struct cbta_sprite out = { head[0], head[1], data, mask };
	return out;
}
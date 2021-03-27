#include "scene.h"

void init_scene_buf(struct scene_buf_t *scene_buf, struct scene_t *scene)
{
	for (int x = 0; x < NUM_TILES; ++x)
	{
		for (int y = 0; y < NUM_TILES; ++y)
		{
			for (int i = 0; i < TILE_BYTE_SIZE; ++i)
			{
				scene_buf->buf[x][y][i] = scene->sprite->data[(y * TILE_BYTE_SIZE + i) * NUM_TILES + x];
			}
		}
	}
}

void scroll_scene_right(struct scene_buf_t *scene_buf, uint8_t pixels)
{
	for (int y = 0; y < NUM_TILES; ++y)
	{
		for (int i = 0; i < TILE_BYTE_SIZE; ++i)
		{
			uint8_t prev = scene_buf->buf[NUM_TILES - 1][y][i];
			uint8_t carry = prev << (8 - pixels);

			for (int x = 0; x < NUM_TILES; ++x)
			{
				prev = scene_buf->buf[x][y][i];

				scene_buf->buf[x][y][i] = (scene_buf->buf[x][y][i] >> pixels) | carry;

				carry = prev << (8 - pixels);
			}
		}
	}
}

void scroll_scene_left(struct scene_buf_t *scene_buf, uint8_t pixels)
{
	for (int y = 0; y < NUM_TILES; ++y)
	{
		for (int i = 0; i < TILE_BYTE_SIZE; ++i)
		{
			uint8_t prev = scene_buf->buf[0][y][i];
			uint8_t carry = prev >> (8 - pixels);

			for (int x = NUM_TILES - 1; x >= 0; --x)
			{
				prev = scene_buf->buf[x][y][i];

				scene_buf->buf[x][y][i] = (scene_buf->buf[x][y][i] << pixels) | carry;

				carry = prev >> (8 - pixels);
			}
		}
	}
}

struct scene_t init_scene(struct u_bounds_t bounds, struct cbt_sprite *sprite)
{
	struct scene_t scene;

	scene.sprite = sprite;
	scene.bounds = bounds;
	scene.modified = 0x00;

	return scene;
}
#include "scene.h"
#include "error_handling.h"

void scene_buf_init(struct scene_buf_t *scene_buf, struct scene_t *scene)
{
	if (is_null(scene_buf) || is_null(scene)) return;
	if (is_null(scene->sprite)) return;

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

void scene_scroll_right(struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf, uint8_t pixels)
{
	if (is_null(scene_buf)) return;
	if (is_null(obj_buf)) return;

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

void scene_scroll_left(struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf, uint8_t pixels)
{
	if (is_null(scene_buf)) return;	
	if (is_null(obj_buf)) return;

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

	for (uint8_t i = 0; i < obj_buf->filled; ++i)
	{
		//	Make sure not a player using the collision layers.
		if (!(obj_buf->buf[i].collision & COLLIDE_LAYER_1 & COLLIDE_MATRIX_MASK))
		{
			object_offset_pos(&obj_buf->buf[i], (struct vec2) { -pixels, 0 });
		}
	}
}

struct scene_t scene_init(struct u_bounds_t bounds, struct cbt_sprite *sprite)
{
	struct scene_t scene = { NULL, {{0, 0}, {0, 0}}, 0 };

	if (is_null(sprite)) return scene;
	if (is_null(sprite->data)) return scene;

	scene.sprite = sprite;
	scene.bounds = bounds;
	scene.modified = 0;

	return scene;
}
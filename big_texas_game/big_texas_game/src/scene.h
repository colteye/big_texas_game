#ifndef SCENE_H
#define SCENE_H

#include <stdint.h>
#include "sprite.h"
#include "bounds.h"
#include "define.h"

// Scene not actually an object, dont keep track of pos.
struct scene_t
{
	struct cbt_sprite *sprite;
	struct u_bounds_t bounds;
	uint8_t modified;
};

struct scene_buf_t
{
	uint8_t buf[NUM_TILES][NUM_TILES][TILE_BYTE_SIZE];
};

void init_scene_buf(struct scene_buf_t *scene_buf, struct scene_t *scene);

void scroll_scene_right(struct scene_buf_t *scene_buf, uint8_t pixels);

void scroll_scene_left(struct scene_buf_t *scene_buf, uint8_t pixels);

struct scene_t init_scene(struct u_bounds_t bounds, struct cbt_sprite *sprite);

#endif /* SCENE_H */
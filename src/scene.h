#ifndef SCENE_H
#define SCENE_H

#include <stdint.h>
#include "sprite.h"
#include "bounds.h"
#include "define.h"
#include "object.h"

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

void scene_buf_init(struct scene_buf_t *scene_buf, struct scene_t *scene);

void scene_scroll_right(struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf, uint8_t pixels);

void scene_scroll_left(struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf, uint8_t pixels);

struct scene_t scene_init(struct u_bounds_t bounds, struct cbt_sprite *sprite);

#endif /* SCENE_H */
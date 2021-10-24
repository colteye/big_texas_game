#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>
#include "vec.h"
#include "sprite.h"
#include "define.h"
#include "bounds.h"

struct object_t
{
	struct cbta_sprite *sprite;
	struct u_bounds_t bounds;
	struct u_vec2 pos;
	uint8_t y_flip;
	uint8_t collision;
	uint8_t vis_mod_collision_mask;
};

struct obj_buf_t
{
	struct object_t buf[MAX_PHYS_OBJ];
	uint8_t filled;
};

void object_set_pos(struct object_t *obj, struct u_vec2 pos);
void object_offset_pos(struct object_t *obj, struct vec2 offset);

struct object_t *object_init(struct obj_buf_t *obj_buf, struct u_vec2 pos, struct u_bounds_t bounds, struct cbta_sprite *sprite, uint8_t collidable);

#endif /* OBJECT_H */
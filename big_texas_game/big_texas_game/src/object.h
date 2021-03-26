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
	uint8_t modified;
};

struct obj_buf_t
{
	struct object_t buf[MAX_PHYS_OBJ];
	uint8_t filled;
};

void move_obj(struct object_t *obj, struct vec2 *pos);

struct object_t *init_obj(struct obj_buf_t *obj_buf, struct u_vec2 pos, struct u_bounds_t bounds, struct cbta_sprite *sprite);

#endif /* OBJECT_H */
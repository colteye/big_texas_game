#ifndef PHYSICS_H
#define PHYSICS_H

#include "vec.h"
#include "object.h"
#include "define.h"
#include "bounds.h"
#include "scene.h"

struct phys_object_t
{
	struct object_t *obj;
	struct vec2 vel;
	struct vec2 acc;
};

struct phys_obj_buf_t
{
	struct phys_object_t buf[MAX_PHYS_OBJ];
	uint8_t filled;
};

struct u_vec2 phys_object_collide(struct phys_object_t *phys_obj, struct vec2 new_offset_pos, struct obj_buf_t *obj_buf, struct u_bounds_t scene_bounds);

void physics_tick(struct phys_obj_buf_t *phys_obj_buf, struct obj_buf_t *obj_buf, struct scene_t * scene);

struct phys_object_t *phys_object_init(struct phys_obj_buf_t *phys_obj_buf, struct object_t *obj, struct vec2 vel, struct vec2 acc);


#endif /* PHYSICS_H */
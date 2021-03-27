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
	uint8_t collision_status;
};

struct phys_obj_buf_t
{
	struct phys_object_t buf[MAX_PHYS_OBJ];
	uint8_t filled;
};

void collide_phys_obj_in_bounds(struct phys_object_t *phys_obj, struct u_bounds_t *bounds);

void physics_tick(struct phys_obj_buf_t *phys_obj_buf, struct scene_t * scene);

struct phys_object_t *init_phys_obj(struct phys_obj_buf_t *phys_obj_buf, struct object_t *obj, struct vec2 vel, struct vec2 acc);


#endif /* PHYSICS_H */
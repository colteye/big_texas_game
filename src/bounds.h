#ifndef BOUNDS_H
#define BOUNDS_H

#include "vec.h"

struct u_bounds_t
{
	struct u_vec2 min;
	struct u_vec2 max;
};

uint8_t within_bounds(struct u_bounds_t bounds, struct u_bounds_t bounds_within);

uint8_t collide_bounds(struct u_bounds_t bounds_collider, struct u_bounds_t bounds_collided);

uint8_t intersect_bounds(struct u_bounds_t bounds_collider, struct u_bounds_t bounds_collided);

#endif /* BOUNDS_H */
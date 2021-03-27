#include "physics.h"

void collide_phys_obj_in_bounds(struct phys_object_t *phys_obj, struct u_bounds_t *bounds)
{
	// Collision status: 0000 1    1    1    1    
	//                       up down left right	
	phys_obj->collision_status = 0x00;

	if (bounds->max.y >= phys_obj->obj->bounds.max.y)
	{
		uint8_t delta = bounds->max.y - phys_obj->obj->bounds.max.y;
		phys_obj->obj->pos.y += delta;
		phys_obj->obj->bounds.min.y += delta;
		phys_obj->obj->bounds.max.y += delta;

		phys_obj->vel.y = 0;

		phys_obj->collision_status |= BIT0;
	}

	if (bounds->min.y <= phys_obj->obj->bounds.min.y)
	{
		uint8_t delta = phys_obj->obj->bounds.min.y - bounds->min.y;
		phys_obj->obj->pos.y -= delta;
		phys_obj->obj->bounds.min.y -= delta;
		phys_obj->obj->bounds.max.y -= delta;

		phys_obj->vel.y = 0;

		phys_obj->collision_status |= BIT1;
	}

	if (bounds->min.x >= phys_obj->obj->bounds.min.x)
	{
		uint8_t delta = bounds->min.x - phys_obj->obj->bounds.min.x;
		phys_obj->obj->pos.x += delta;
		phys_obj->obj->bounds.min.x += delta;
		phys_obj->obj->bounds.max.x += delta;

		phys_obj->vel.x = 0;

		phys_obj->collision_status |= BIT2;
	}

	if (bounds->max.x <= phys_obj->obj->bounds.max.x)
	{
		uint8_t delta = phys_obj->obj->bounds.max.x - bounds->max.x;
		phys_obj->obj->pos.x -= delta;
		phys_obj->obj->bounds.min.x -= delta;
		phys_obj->obj->bounds.max.x -= delta;

		phys_obj->vel.x = 0;

		phys_obj->collision_status |= BIT3;
	}
}

void physics_tick(struct phys_obj_buf_t *phys_obj_buf, struct scene_t * scene)
{
	for (uint8_t i = 0; i < phys_obj_buf->filled; ++i)
	{
		struct vec2 pos_update = { phys_obj_buf->buf[i].vel.x + phys_obj_buf->buf[i].acc.x / 2,
								   phys_obj_buf->buf[i].vel.y + phys_obj_buf->buf[i].acc.y / 2 };

		move_obj(phys_obj_buf->buf[i].obj, &pos_update);

		// Collide with scene.
		collide_phys_obj_in_bounds(&phys_obj_buf->buf[i], &scene->bounds);

		phys_obj_buf->buf[i].vel.x += phys_obj_buf->buf[i].acc.x;
		phys_obj_buf->buf[i].vel.y += phys_obj_buf->buf[i].acc.y;
	}
}

struct phys_object_t *init_phys_obj(struct phys_obj_buf_t *phys_obj_buf, struct object_t *obj, struct vec2 vel, struct vec2 acc)
{
	struct phys_object_t phys_obj;
	phys_obj.vel = vel;
	phys_obj.acc = acc;
	phys_obj.obj = obj;
	phys_obj.collision_status = 0x00;

	phys_obj_buf->buf[phys_obj_buf->filled] = phys_obj;
	phys_obj_buf->filled++;

	return &phys_obj_buf->buf[phys_obj_buf->filled - 1];
}
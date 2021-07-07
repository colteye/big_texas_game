#include "object.h"
#include "error_handling.h"

void object_set_pos(struct object_t *obj, struct u_vec2 pos)
{
	if (is_null(obj)) return;
	object_offset_pos(obj, (struct vec2) { pos.x - obj->pos.x, pos.y - obj->pos.y });
}

void object_offset_pos(struct object_t *obj, struct vec2 offset)
{
	if (is_null(obj)) return;

	// Out of bounds check.
	if (!IN_X_RANGE(obj->bounds.min.x + offset.x) ||
		!IN_X_RANGE(obj->bounds.max.x + offset.x) ||
		!IN_Y_RANGE(obj->bounds.max.y + offset.y) ||
		!IN_Y_RANGE(obj->bounds.min.y + offset.y))
	{
		//set_error(ERROR_OUT_OF_BOUNDS);
		//return;
	}

	// increment object coordinates according to pos.
	obj->bounds.min.x += offset.x;
	obj->bounds.max.x += offset.x;

	obj->bounds.min.y += offset.y;
	obj->bounds.max.y += offset.y;

	obj->pos.x += offset.x;
	obj->pos.y += offset.y;
}

struct object_t *object_init(struct obj_buf_t *obj_buf, struct u_vec2 pos, struct u_bounds_t bounds, struct cbta_sprite *sprite, uint8_t collision_layer)
{
	if (is_null(obj_buf) || is_null(sprite)) return NULL;

	// Buffer overload check.
	if (obj_buf->filled >= MAX_OBJ)
	{
		set_error(ERROR_BUFFER_OVERRUN);
		return NULL;
	}

	// Out of bounds check.
	if (!IN_X_RANGE(bounds.min.x) ||
		!IN_X_RANGE(bounds.max.x) ||
		!IN_Y_RANGE(bounds.min.y) ||
		!IN_Y_RANGE(bounds.max.y))
	{
		set_error(ERROR_OUT_OF_BOUNDS);
		return NULL;
	}

	// Create object components.
	struct object_t obj;
	obj.pos = pos;
	obj.bounds = bounds;
	obj.sprite = sprite;

	// Collision mask system with flags.
	obj.collision = collision_layer;
	obj.vis_mod_collision_mask = VISIBLE;

	switch (collision_layer)
	{
		case COLLIDE_LAYER_1:
			obj.vis_mod_collision_mask |= COLLIDE_MATRIX_1;
			break;
		case COLLIDE_LAYER_2:
			obj.vis_mod_collision_mask |= COLLIDE_MATRIX_2;
			break;
		case COLLIDE_LAYER_3:
			obj.vis_mod_collision_mask |= COLLIDE_MATRIX_3;
			break;
		case COLLIDE_LAYER_4:
			obj.vis_mod_collision_mask |= COLLIDE_MATRIX_4;
			break;
	}

	// Add to buffer.
	obj_buf->buf[obj_buf->filled] = obj;
	obj_buf->filled++;

	return &obj_buf->buf[obj_buf->filled - 1];
}
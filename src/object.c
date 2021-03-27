#include "object.h"

void move_obj(struct object_t *obj, struct vec2 *pos)
{
	if (!IN_X_RANGE(obj->bounds.min.x + pos->x) ||
		!IN_X_RANGE(obj->bounds.max.x + pos->x) ||
		!IN_Y_RANGE(obj->bounds.min.y - pos->y) ||
		!IN_Y_RANGE(obj->bounds.max.y - pos->y))
		return;

	obj->bounds.min.x += pos->x;
	obj->bounds.max.x += pos->x;

	obj->bounds.min.y -= pos->y;
	obj->bounds.max.y -= pos->y;

	obj->pos.x += pos->x;
	obj->pos.y -= pos->y;
}

struct object_t *init_obj(struct obj_buf_t *obj_buf, struct u_vec2 pos, struct u_bounds_t bounds, struct cbta_sprite *sprite)
{
	struct object_t obj;
	obj.pos = pos;
	obj.bounds = bounds;
	obj.sprite = sprite;
	obj.modified = 0x00;

	obj_buf->buf[obj_buf->filled] = obj;
	obj_buf->filled++;

	return &obj_buf->buf[obj_buf->filled - 1];
}
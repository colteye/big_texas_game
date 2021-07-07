#include "physics.h"
#include "error_handling.h"

// Returns new position with updates based on object collision.
struct u_vec2 phys_object_collide(struct phys_object_t *phys_obj, struct vec2 new_offset_pos, struct obj_buf_t *obj_buf, struct u_bounds_t scene_bounds)
{
	// Null checks.
	if (is_null(phys_obj) || is_null(obj_buf)) return (struct u_vec2) { 0, 0 };
	if (is_null(phys_obj->obj)) return (struct u_vec2) { 0, 0 };

	// Reset collision status but not layer.
	phys_obj->obj->collision &= COLLIDE_MATRIX_MASK;

	// Clamp offsets if they go past the viewable area.
	// X offsets.
	int16_t min_x_off = phys_obj->obj->bounds.min.x + new_offset_pos.x;
	int16_t max_x_off = phys_obj->obj->bounds.max.x + new_offset_pos.x;

	if (min_x_off > WIDTH + WIDTH_OFFSET) new_offset_pos.x = WIDTH + WIDTH_OFFSET - phys_obj->obj->bounds.min.x;
	else if (min_x_off < WIDTH_OFFSET) new_offset_pos.x = - phys_obj->obj->bounds.min.x;

	else if (max_x_off > WIDTH + WIDTH_OFFSET) new_offset_pos.x = WIDTH + WIDTH_OFFSET - phys_obj->obj->bounds.max.x;
	else if (max_x_off < WIDTH_OFFSET) new_offset_pos.x = -phys_obj->obj->bounds.max.x;

	// Y offsets.
	int16_t min_y_off = phys_obj->obj->bounds.min.y + new_offset_pos.y;
	int16_t max_y_off = phys_obj->obj->bounds.max.y + new_offset_pos.y;

	if (min_y_off > HEIGHT + HEIGHT_OFFSET) new_offset_pos.y = HEIGHT + HEIGHT_OFFSET - phys_obj->obj->bounds.min.y;
	else if (min_y_off < HEIGHT_OFFSET) new_offset_pos.y = -phys_obj->obj->bounds.min.y;

	else if (max_y_off > HEIGHT + HEIGHT_OFFSET) new_offset_pos.y = HEIGHT + HEIGHT_OFFSET - phys_obj->obj->bounds.max.y;
	else if (max_y_off < HEIGHT_OFFSET) new_offset_pos.y = -phys_obj->obj->bounds.max.y;


	// Update position with potential new coordinates.
	phys_obj->obj->bounds.min.x += new_offset_pos.x;
	phys_obj->obj->bounds.max.x += new_offset_pos.x;

	phys_obj->obj->bounds.min.y += new_offset_pos.y;
	phys_obj->obj->bounds.max.y += new_offset_pos.y;

	phys_obj->obj->pos.x += new_offset_pos.x;
	phys_obj->obj->pos.y += new_offset_pos.y;


	struct u_bounds_t obj_bounds = phys_obj->obj->bounds;
	struct u_vec2 update_pos = phys_obj->obj->pos;

	// Check for scene bounds collision.
	uint8_t scene_collision = within_bounds(phys_obj->obj->bounds, scene_bounds);

	if (scene_collision & COLLIDE_UP) 
		update_pos.y = scene_bounds.max.y;

	if (scene_collision & COLLIDE_DOWN) 
		update_pos.y = scene_bounds.min.y - (obj_bounds.min.y - obj_bounds.max.y);

	if (scene_collision & COLLIDE_LEFT) 
		update_pos.x = scene_bounds.min.x;

	if (scene_collision & COLLIDE_RIGHT) 
		update_pos.x = scene_bounds.max.x - (obj_bounds.max.x - obj_bounds.min.x);

	phys_obj->obj->collision |= scene_collision;

	// Check for object collisions.
	for (uint8_t i = 0; i < obj_buf->filled; ++i)
	{
		// Dont use own object for collision!
		// Check if they have compatible collision layers!
		if (phys_obj->obj == &obj_buf->buf[i] ||
			!(phys_obj->obj->collision & obj_buf->buf[i].vis_mod_collision_mask & COLLIDE_MATRIX_MASK))
			continue;

		uint8_t obj_collision = collide_bounds(obj_buf->buf[i].bounds, phys_obj->obj->bounds);

		// Object collision can only be one type of collision.
		switch (obj_collision)
		{
			case COLLIDE_UP:
				update_pos.y = obj_buf->buf[i].bounds.min.y;
				break;
			case COLLIDE_DOWN:
				update_pos.y = obj_buf->buf[i].bounds.max.y - (obj_bounds.min.y - obj_bounds.max.y);
				break;
			case COLLIDE_LEFT:
				update_pos.x = obj_buf->buf[i].bounds.max.x;
				break;
			case COLLIDE_RIGHT:
				update_pos.x = obj_buf->buf[i].bounds.min.x - (obj_bounds.max.x - obj_bounds.min.x);
				break;
		}

		phys_obj->obj->collision |= obj_collision;
	}

	// Check if colliding with the up/down of the bounds.
	if (phys_obj->obj->collision & (COLLIDE_UP | COLLIDE_DOWN))
		phys_obj->vel.y = 0;

	// Check if colliding with the left/right of the bounds.
	if (phys_obj->obj->collision & (COLLIDE_LEFT | COLLIDE_RIGHT))
		phys_obj->vel.x = 0;

	return update_pos;
}

void physics_tick(struct phys_obj_buf_t *phys_obj_buf, struct obj_buf_t *obj_buf, struct scene_t * scene)
{
	if (is_null(phys_obj_buf) || is_null(obj_buf) || is_null(scene)) return;

	// Update physics for every physics object.
	for (uint8_t i = 0; i < phys_obj_buf->filled; ++i)
	{
		// Calculate position update based on physics.
		struct vec2 new_offset_pos = { phys_obj_buf->buf[i].vel.x + phys_obj_buf->buf[i].acc.x,
								    -(phys_obj_buf->buf[i].vel.y + phys_obj_buf->buf[i].acc.y) };

		// Compute the final object position.
		struct u_vec2 new_pos = phys_object_collide(&phys_obj_buf->buf[i], new_offset_pos, obj_buf, scene->bounds);

		// Move.
		object_set_pos(phys_obj_buf->buf[i].obj, new_pos);

		// Update physics object velocities.
		phys_obj_buf->buf[i].vel.x += phys_obj_buf->buf[i].acc.x;
		phys_obj_buf->buf[i].vel.y += phys_obj_buf->buf[i].acc.y;

	}
}

struct phys_object_t *phys_object_init(struct phys_obj_buf_t *phys_obj_buf, struct object_t *obj, struct vec2 vel, struct vec2 acc)
{
	// Create phys object components.
	struct phys_object_t phys_obj;

	if (is_null(phys_obj_buf) || is_null(obj)) return NULL;

	// Buffer overload check.
	if (phys_obj_buf->filled >= MAX_PHYS_OBJ)
	{
		set_error(ERROR_BUFFER_OVERRUN);
		return NULL;
	}

	phys_obj.vel = vel;
	phys_obj.acc = acc;
	phys_obj.obj = obj;

	// Add to buffer.
	phys_obj_buf->buf[phys_obj_buf->filled] = phys_obj;
	phys_obj_buf->filled++;

	return &phys_obj_buf->buf[phys_obj_buf->filled - 1];
}
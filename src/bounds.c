#include "define.h"
#include "bounds.h"

// Collision status: 0000 1    1    1    1    
//                       up down left right	

uint8_t within_bounds(struct u_bounds_t bounds, struct u_bounds_t bounds_within)
{
	uint8_t collision = 0x00;

	// Check if colliding with the top of the bounds.
	if (bounds_within.max.y >= bounds.max.y) collision |= COLLIDE_UP;

	// Check if colliding with the bottom of the bounds.
	if (bounds_within.min.y <= bounds.min.y) collision |= COLLIDE_DOWN;

	// Check if colliding with the left of the bounds.
	if (bounds_within.min.x >= bounds.min.x) collision |= COLLIDE_LEFT;

	// Check if colliding with the right of the bounds.
	if (bounds_within.max.x <= bounds.max.x) collision |= COLLIDE_RIGHT;

	return collision;
}

// Return collision status of bounds_collided.
uint8_t collide_bounds(struct u_bounds_t bounds_collider, struct u_bounds_t bounds_collided)
{
	// Check if bounds actually intersecting first.
	if (bounds_collided.min.y < bounds_collider.max.y
		|| bounds_collided.max.y > bounds_collider.min.y
		|| bounds_collided.max.x < bounds_collider.min.x
		|| bounds_collided.min.x > bounds_collider.max.x) 
		return 0;

	if ((ABS(bounds_collided.min.x - bounds_collider.min.x)) <= (bounds_collided.max.x - bounds_collided.min.x) ||
		(ABS(bounds_collided.max.x - bounds_collider.max.x)) <= (bounds_collided.max.x - bounds_collided.min.x))
	{
		if (bounds_collided.max.y <= bounds_collider.min.y &&
			bounds_collided.max.y <= bounds_collider.max.y)
			return COLLIDE_DOWN;

		if (bounds_collided.min.y <= bounds_collider.max.y &&
			bounds_collided.min.y >= bounds_collider.min.y)
			return COLLIDE_UP;
	}

	if ((ABS(bounds_collided.min.y - bounds_collider.min.y)) <= (bounds_collided.min.y - bounds_collided.max.y) ||
		(ABS(bounds_collided.max.y - bounds_collider.max.y)) <= (bounds_collided.min.y - bounds_collided.max.y))
	{
		if (bounds_collided.min.x <= bounds_collider.max.x &&
			bounds_collided.min.x >= bounds_collider.min.x)
			return COLLIDE_LEFT;

		if (bounds_collided.max.x >= bounds_collider.min.x &&
			bounds_collided.max.x <= bounds_collider.max.x)
			return COLLIDE_RIGHT;
	}
	return 0;
}

// Return collision status of bounds_collided.
uint8_t intersect_bounds(struct u_bounds_t bounds_collider, struct u_bounds_t bounds_collided)
{
	// Check if bounds actually intersecting first.
	if (bounds_collided.min.y < bounds_collider.max.y
		|| bounds_collided.max.y > bounds_collider.min.y
		|| bounds_collided.max.x < bounds_collider.min.x
		|| bounds_collided.min.x > bounds_collider.max.x) return 0;

	uint8_t collision = 0x00;

	// Check if colliding with the top of the bounds.
	if (bounds_collided.max.y <= bounds_collider.min.y)
		collision |= COLLIDE_UP;

	// Check if colliding with the bottom of the bounds.
	if (bounds_collided.min.y >= bounds_collider.max.y)
		collision |= COLLIDE_DOWN;

	// Check if colliding with the left of the bounds.
	if (bounds_collided.min.x <= bounds_collider.max.x)
		collision |= COLLIDE_LEFT;

	// Check if colliding with the right of the bounds.
	if (bounds_collided.max.x >= bounds_collider.min.x)
		collision |= COLLIDE_RIGHT;

	return collision;
}
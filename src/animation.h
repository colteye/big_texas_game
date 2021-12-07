#ifndef ANIMATION_H
#define ANIMATION_H

#include "vec.h"
#include "object.h"
#include "define.h"
#include "bounds.h"
#include "scene.h"

struct anim_sys_t
{
	// Keep track of anim and frame to update sprite
	uint8_t current_anim;
	uint8_t current_frame;
	uint8_t frames_counter;

	// Start indexes in spritesheet for animation
	struct vec2 *anim_starts;
	uint8_t *anim_lens;

	// Frames need to be next to one another
	uint8_t frame_width;

	// Object to animate!
	struct object_t *animate_obj;
};

struct anim_sys_buf_t
{
	struct anim_sys_t buf[MAX_ANIM_SYS];
	uint8_t filled;
};

struct anim_sys_t *anim_sys_init(struct anim_sys_buf_t *anim_sys_buf, struct object_t *obj, struct vec2 *anim_starts, uint8_t *anim_lens, uint8_t frame_width);

void anim_sys_set_anim(struct anim_sys_t *anim_sys, uint8_t anim_idx);

void animation_tick(struct anim_sys_buf_t *anim_sys_buf);

#endif /* ANIMATION_H */
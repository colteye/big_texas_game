#include "animation.h"
#include "error_handling.h"

#define FRAME_DELAY 2

struct anim_sys_t *anim_sys_init(struct anim_sys_buf_t *anim_sys_buf, struct object_t *obj, struct vec2 *anim_starts, uint8_t *anim_lens, uint8_t frame_width)
{
	// Create phys object components.
	struct anim_sys_t anim_sys;

	if (is_null(anim_sys_buf) || is_null(obj)) return NULL;

	// Buffer overload check.
	if (anim_sys_buf->filled >= MAX_ANIM_SYS)
	{
		set_error(ERROR_BUFFER_OVERRUN);
		return NULL;
	}

	anim_sys.animate_obj = obj;
	anim_sys.anim_starts = anim_starts;
	anim_sys.anim_lens = anim_lens;
	anim_sys.frame_width = frame_width;

	anim_sys.current_anim = 0;
	anim_sys.current_frame = 0;
	anim_sys.frames_counter = FRAME_DELAY;

	// Add to buffer.
	anim_sys_buf->buf[anim_sys_buf->filled] = anim_sys;
	anim_sys_buf->filled++;

	return &anim_sys_buf->buf[anim_sys_buf->filled - 1];
}


void anim_sys_set_anim(struct anim_sys_t *anim_sys, uint8_t anim_idx)
{
	if (is_null(anim_sys)) return;

	if (anim_sys->current_anim != anim_idx)
	{
		anim_sys->current_anim = anim_idx;
		anim_sys->current_frame = 0;
	}
}

void animation_tick(struct anim_sys_buf_t *anim_sys_buf)
{
	if (is_null(anim_sys_buf)) return;

	// Update animation frame for every animation system.
	for (uint8_t i = 0; i < anim_sys_buf->filled; ++i)
	{
		// Loop frames with modulus!
		// Use delay to make things a bit slower if needed
		struct anim_sys_t *a = &anim_sys_buf->buf[i];
		if (a->anim_lens[a->current_anim] > 1)
		{
			if (a->frames_counter)
			{
				a->frames_counter--;
			}
			else
			{
				a->current_frame = (a->current_frame + 1) % (a->anim_lens[a->current_anim]);
				a->frames_counter = FRAME_DELAY;
			}
		}

		//  Modify animation object sprite offset!
		a->animate_obj->spritesheet_offset = a->anim_starts[a->current_anim];
		a->animate_obj->spritesheet_offset.x += (a->current_frame * a->frame_width);

		printf("NEW POS X SPRITE: %d\n", a->animate_obj->spritesheet_offset.x);
		printf("%d\n", a->animate_obj);
	}
}
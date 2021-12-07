#include <input.h>
#include <stdio.h>

uint8_t is_key_pressed(struct key_status_t *key_s, uint32_t key_mask)
{
	// Based on truth table with repeat and key values.
	return ((key_s->keys_press) & (~key_s->repeat_press) & key_mask) > 0;
};

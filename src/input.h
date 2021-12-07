#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

struct key_status_t
{
	uint8_t keys_press;
	uint8_t repeat_press;
};

uint8_t is_key_pressed(struct key_status_t *key_s, uint32_t key_mask);

#endif /* INPUT_H */
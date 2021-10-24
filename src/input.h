#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include "player.h"
#include "scene.h"

struct key_status_t
{
	uint8_t attack_1_key_s;
	uint8_t attack_2_key_s;
	uint8_t left_key_s;
	uint8_t right_key_s;
	uint8_t jump_key_s;
};

#endif /* INPUT_H */
#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include "player.h"
#include "define.h"
#include "scene.h"

struct key_status_t
{
	uint8_t left_key_s;
	uint8_t right_key_s;
	uint8_t jump_key_s;
};

void check_input(struct player_t *player, struct scene_buf_t *scene_buf, struct key_status_t key_s);

#endif /* INPUT_H */
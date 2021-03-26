#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "vec.h"
#include "scene.h"
#include "define.h"
#include "sprite.h"
#include "object.h"
#include "physics.h"

struct player_t
{
	struct phys_object_t *phys_obj;
	uint32_t points;
	uint8_t health;
	uint8_t jump_state;
};

struct player_t init_player(struct phys_object_t *phys_obj);

void move_jump(struct player_t *player);

void move_left(struct player_t *player);

void move_right(struct player_t *player, struct scene_buf_t *scene_buf);

void move_stop_x(struct player_t *player);

void move_stop_y(struct player_t *player);

#endif /* PLAYER_H */
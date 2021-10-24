#ifndef PLAYER_H
#define PLAYER_H

#define NOT_JUMPING 0x00
#define START_JUMP 0x10
#define END_JUMP 0x20
#define MAX_STACKED_JUMPS 2

#include <stdint.h>
#include "vec.h"
#include "scene.h"
#include "define.h"
#include "sprite.h"
#include "object.h"
#include "physics.h"
#include "input.h"

struct player_t
{
	struct phys_object_t *phys_obj;
	uint32_t points;
	uint8_t health;
	uint8_t jump_state;
};

struct player_t player_init(struct phys_object_t *phys_obj);

void player_move_jump(struct player_t *player);

void player_move_left(struct player_t *player, struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf);

void player_move_right(struct player_t *player, struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf);

void player_move_stop_x(struct player_t *player);

void player_move_stop_y(struct player_t *player);

void player_send_damage(struct player_t *damage_send, struct player_t *damage_recv, uint8_t damage);

#endif /* PLAYER_H */
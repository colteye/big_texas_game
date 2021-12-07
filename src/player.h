#ifndef PLAYER_H
#define PLAYER_H

#define NOT_JUMPING 0x00
#define START_JUMP 0x10
#define END_JUMP 0x20
#define MAX_STACKED_JUMPS 1

#include <stdint.h>
#include "vec.h"
#include "scene.h"
#include "define.h"
#include "sprite.h"
#include "object.h"
#include "physics.h"
#include "animation.h"
#include "input.h"

struct player_t
{
	struct phys_object_t *phys_obj;
	struct anim_sys_t *anim_sys;

	uint32_t points;
	uint8_t health;
	uint8_t jump_state;
};

struct player_t player_init(struct phys_object_t *phys_obj, struct anim_sys_t *anim_sys);

void player_move_jump(struct player_t *player);

void player_check_jump(struct player_t *player);

void player_move(struct player_t *player, struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf, int8_t velocity);

void player_stop(struct player_t *player, uint8_t y);

void player_send_damage(struct player_t *damage_send, struct player_t *damage_recv, uint8_t damage);

#endif /* PLAYER_H */
#ifndef USER_H
#define USER_H

#include <stdint.h>
#include "vec.h"
#include "scene.h"
#include "define.h"
#include "sprite.h"
#include "object.h"
#include "physics.h"
#include "input.h"
#include "player.h"

struct user_t
{
	uint8_t attack_damage;
	struct player_t *player;
	uint8_t collision_with_player;
};

struct user_t user_init(struct player_t *player, uint8_t damage);

void user_move(struct user_t *user, struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf, struct key_status_t key_s);

void user_attack(struct user_t *user, struct enemy_t *enemy, struct key_status_t key_s);

#endif /* USER_H */
#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>
#include "vec.h"
#include "scene.h"
#include "define.h"
#include "sprite.h"
#include "object.h"
#include "physics.h"
#include "input.h"
#include "user.h"
#include "player.h"

struct enemy_t
{
	uint8_t attack_damage;
	struct player_t *player;
	uint8_t collision_with_player;
};

struct enemy_t enemy_init(struct player_t *player, uint8_t damage);

void enemy_move(struct enemy_t *enemy);

void enemy_attack(struct enemy_t *enemy, struct user_t *user);

void enemy_ai(struct enemy_t *enemy, struct user_t *user, struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf);

#endif /* ENEMY_H */
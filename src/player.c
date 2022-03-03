#include "player.h"
#include "error_handling.h"
#include "bounds.h"

struct player_t player_init(struct phys_object_t *phys_obj, struct anim_sys_t *anim_sys)
{
	// Create player components.
	struct player_t player = { NULL, NULL, 0, 0, NOT_JUMPING};

	// Early return if null.
	if (is_null(phys_obj) || is_null(anim_sys)) return player;

	player.health = 100;	
	player.jump_state = NOT_JUMPING;

	// Assign player physics to player.
	player.phys_obj = phys_obj;
	player.anim_sys = anim_sys;

	anim_sys_set_anim(player.anim_sys, IDLE_ANIM_INDEX);
	return player;
}

void player_move_jump(struct player_t *player)
{
	if (is_null(player)) return;
	if (is_null(player->phys_obj)) return;

	if (player->jump_state == NOT_JUMPING)
	{
		player->phys_obj->vel.y = 8;
		player->jump_state = START_JUMP;
		anim_sys_set_anim(player->anim_sys, JUMP_ANIM_INDEX);
	}
}

void player_check_jump(struct player_t *player)
{
	if (player->jump_state != NOT_JUMPING)
	{
		if (player->phys_obj->obj->collision & (COLLIDE_DOWN | COLLIDE_UP))
			player->jump_state = NOT_JUMPING;
			anim_sys_set_anim(player->anim_sys, IDLE_ANIM_INDEX);
	}
}

void player_move(struct player_t *player, struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf, int8_t velocity)
{
	if (is_null(player) || is_null(scene_buf)) return;
	if (is_null(player->phys_obj)) return;

	if (velocity < 0) player->phys_obj->obj->y_flip = 1;
	else player->phys_obj->obj->y_flip = 0;
	
	player->phys_obj->vel.x = velocity;

	if (player->jump_state == NOT_JUMPING) anim_sys_set_anim(player->anim_sys, RUN_ANIM_INDEX);
}

void player_stop(struct player_t *player, uint8_t y)
{
	if (is_null(player)) return;
	if (is_null(player->phys_obj)) return;

	if (y) player->phys_obj->vel.y = 0;
	else player->phys_obj->vel.x = 0;

	anim_sys_set_anim(player->anim_sys, IDLE_ANIM_INDEX);
}

void player_send_damage(struct player_t *damage_send, struct player_t *damage_recv, uint8_t damage)
{
	if (is_null(damage_send) || is_null(damage_recv)) return;

	if (damage >= damage_recv->health) damage_recv->health = 0;
	else damage_recv->health -= damage;
}

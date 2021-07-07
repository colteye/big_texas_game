#include "player.h"
#include "error_handling.h"

struct player_t player_init(struct phys_object_t *phys_obj)
{
	// Create player components.
	struct player_t player = { NULL, 0, 0, NOT_JUMPING};

	// Early return if null.
	if (is_null(phys_obj)) return player;

	player.points = 0;
	player.health = 100;	
	player.jump_state = NOT_JUMPING;

	// Assign player physics to player.
	player.phys_obj = phys_obj;

	return player;
}

void player_move_jump(struct player_t *player)
{
	if (is_null(player)) return;
	if (is_null(player->phys_obj)) return;

	// Check jump state.
	switch (player->jump_state & 0xF0) {

	// If not jumping, begin to jump.
	case NOT_JUMPING:
		player->phys_obj->vel.y = 7;
		player->jump_state = (player->jump_state & 0x0F) + START_JUMP;
		break;

	// If jumping has already started, see if you can stack jumps.
	case START_JUMP:
		if ((player->jump_state & 0x0F) >= MAX_STACKED_JUMPS)
		{
			player->jump_state = END_JUMP;
		}
		else
		{
			player->jump_state = (player->jump_state & 0x0F) + NOT_JUMPING + 1;
		}
		break;

	// If at the top of the scene, immediately stop jumping.
	case END_JUMP:
		if (player->phys_obj->obj->collision & BIT1)
		{
			player->jump_state = (player->jump_state & 0x0F) + NOT_JUMPING;
		}
		break;
	}
}

void player_move_left(struct player_t *player)
{
	if (is_null(player)) return;
	if (is_null(player->phys_obj)) return;

	player->phys_obj->vel.x = -2;
}

void player_move_right(struct player_t *player, struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf)
{
	if (is_null(player) || is_null(scene_buf)) return;
	if (is_null(player->phys_obj)) return;

	scene_scroll_left(scene_buf, obj_buf, 1);
	player->phys_obj->vel.x = 2;

	// Set velocity to move to the right as long as within certain bounds. Also scroll the scene.
	/*if (player->phys_obj->obj->pos.x < 48)
	{
		player->phys_obj->vel.x = 2;
		scene_scroll_left(scene_buf, 1);
	}
	else
	{
		player->phys_obj->vel.x = 0;
		scene_scroll_left(scene_buf, 2);
	}*/
}

void player_move_stop_x(struct player_t *player)
{
	if (is_null(player)) return;
	if (is_null(player->phys_obj)) return;

	player->phys_obj->vel.x = 0;
}

void player_move_stop_y(struct player_t *player)
{
	if (is_null(player)) return;
	if (is_null(player->phys_obj)) return;

	player->phys_obj->vel.y = 0;
}

void player_send_damage(struct player_t *damage_send, struct player_t *damage_recv, uint8_t damage)
{
	if (is_null(damage_send) || is_null(damage_recv)) return;

	if (damage >= damage_recv->health)
	{
		damage_recv->health = 0;
		damage_send->points += 1;
	}
	else damage_recv->health -= damage;
}

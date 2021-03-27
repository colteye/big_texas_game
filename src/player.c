#include "player.h"

struct player_t init_player(struct phys_object_t *phys_obj)
{
	struct player_t player;

	player.points = 0;
	player.health = 100;
	player.jump_state = NOT_JUMPING;

	// Assign player physics to player.
	player.phys_obj = phys_obj;

	return player;
}

void move_jump(struct player_t *player)
{
	player->points++;


	switch (player->jump_state & 0xF0) {

	case NOT_JUMPING:
		player->phys_obj->vel.y = 6;
		player->phys_obj->acc.y = -1;
		player->jump_state = (player->jump_state & 0x0F) + START_JUMP;
		break;

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

	case END_JUMP:
		if (player->phys_obj->collision_status & BIT1)
		{
			player->jump_state = (player->jump_state & 0x0F) + NOT_JUMPING;
		}
		break;
	}
}

void move_left(struct player_t *player)
{
	if (player->phys_obj->collision_status & BIT2)
	{
		player->phys_obj->vel.x = 0;
	}
	else
	{
		player->phys_obj->vel.x = -2;
	}
}

void move_right(struct player_t *player, struct scene_buf_t *scene_buf)
{
	if (player->phys_obj->obj->pos.x < 48)
	{
		player->phys_obj->vel.x = 2;
		scroll_scene_left(scene_buf, 2);
	}
	else
	{
		player->phys_obj->vel.x = 0;
		scroll_scene_left(scene_buf, 3);
	}
}

void move_stop_x(struct player_t *player)
{
	player->phys_obj->vel.x = 0;
}

void move_stop_y(struct player_t *player)
{
	player->phys_obj->vel.y = 0;
}
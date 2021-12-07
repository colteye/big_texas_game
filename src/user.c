#include "enemy.h"
#include "error_handling.h"

struct user_t user_init(struct player_t *player, uint8_t damage)
{
	// Create player components.
	struct user_t user = { damage, NULL, 0 };
	if (is_null(player)) return user;
	if (is_null(player->phys_obj)) return user;

	user.player = player;

	return user;
}

void user_move(struct user_t *user, struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf, struct key_status_t *key_s)
{
	if (is_null(user) || is_null(scene_buf)) return;
	if (is_null(user->player)) return;

	player_check_jump(user->player);

	// Check keys for presses.
	if (is_key_pressed(key_s, LEFT_KEY))
	{
		scene_scroll_right(scene_buf, obj_buf, 1);
		player_move(user->player, scene_buf, obj_buf, -2);
	}
	else if (is_key_pressed(key_s, RIGHT_KEY))
	{
		scene_scroll_left(scene_buf, obj_buf, 1);
		player_move(user->player, scene_buf, obj_buf, 2);
	}
	else
	{
		player_stop(user->player, 0);
	}

	if (is_key_pressed(key_s, JUMP_KEY))
	{
		player_move_jump(user->player);
	}
}

void user_attack(struct user_t *user, struct enemy_t *enemy, struct key_status_t *key_s)
{
	if (is_null(user) || is_null(enemy) || is_null(key_s)) return;
	if (is_null(user->player)) return;
	if (is_null(enemy->player)) return;

	uint8_t collision = intersect_bounds(enemy->player->phys_obj->obj->bounds, user->player->phys_obj->obj->bounds);

	// Check if button is actually pressed.
	if (is_key_pressed(key_s, ATTACK_1_KEY) || is_key_pressed(key_s, ATTACK_2_KEY))
	{
		anim_sys_set_anim(user->player->anim_sys, ATTACK_ANIM_INDEX);

		// Only send damage if enemy is under user.
		if ((collision & (BIT1 | BIT2 | BIT3)) > 0)
		{
			player_send_damage(user->player, enemy->player, user->attack_damage);
		}	
	}
	user->collision_with_player = collision;
}
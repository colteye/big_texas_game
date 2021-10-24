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

void user_move(struct user_t *user, struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf, struct key_status_t key_s)
{
	if (is_null(user) || is_null(scene_buf)) return;
	if (is_null(user->player)) return;

	// Check keys for presses.
	if (key_s.left_key_s)
	{
		player_move_left(user->player, scene_buf, obj_buf);
	}
	else if (key_s.right_key_s)
	{
		player_move_right(user->player, scene_buf, obj_buf);
	}
	else
	{
		player_move_stop_x(user->player);
	}

	if (key_s.jump_key_s)
	{
		player_move_jump(user->player);
	}
}

void user_attack(struct user_t *user, struct enemy_t *enemy, struct key_status_t key_s)
{
	if (is_null(user) || is_null(enemy)) return;
	if (is_null(user->player)) return;
	if (is_null(enemy->player)) return;

	uint8_t collision = intersect_bounds(enemy->player->phys_obj->obj->bounds, user->player->phys_obj->obj->bounds);
	// Only send damage if enemy is under user.
	if ((collision & BIT1) || (collision & BIT2) || (collision & BIT3))

		
		if (key_s.attack_1_key_s || key_s.attack_2_key_s) {
			player_send_damage(user->player, enemy->player, user->attack_damage);
		}

	user->collision_with_player = collision;
}
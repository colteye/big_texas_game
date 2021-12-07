#include "rand.h"
#include "enemy.h"
#include "player.h"
#include "error_handling.h"

struct enemy_t enemy_init(struct player_t *player, uint8_t damage)
{
	// Create player components.
	struct enemy_t enemy = { damage, NULL, 0 };
	if (is_null(player)) return enemy;
	if (is_null(player->phys_obj)) return enemy;

	enemy.player = player;

	return enemy;
}

void enemy_move(struct enemy_t *enemy)
{
	/*if (is_null(enemy)) return;
	if (is_null(enemy->player)) return;

	uint8_t collision = enemy->player->phys_obj->obj->collision;
	if ((collision & BIT2) || enemy->player->phys_obj->vel.x != -1)
	{
		enemy->player->phys_obj->obj->y_flip = 0;
		enemy->player->phys_obj->vel.x = 1;

	}
		
	if (collision & BIT3) 
	{
		enemy->player->phys_obj->obj->y_flip = 1;
		enemy->player->phys_obj->vel.x = -1;
	}*/
}

void enemy_attack(struct enemy_t *enemy, struct user_t *user)
{
	/*if (is_null(user) || is_null(enemy)) return;
	if (is_null(user->player)) return;
	if (is_null(enemy->player)) return;

	uint8_t collision = intersect_bounds(user->player->phys_obj->obj->bounds, enemy->player->phys_obj->obj->bounds);

	// Dont send damage if user is on top of enemy.
	if (collision != enemy->collision_with_player)
		if ((collision & BIT1) || (collision & BIT2) || (collision & BIT3))
			player_send_damage(enemy->player, user->player, enemy->attack_damage);

	enemy->collision_with_player = collision;*/
}

void enemy_ai(struct enemy_t *enemy, struct user_t *user, struct scene_buf_t *scene_buf, struct obj_buf_t *obj_buf)
{
	// Always check jump state
	player_check_jump(enemy->player);

	// Decision tree
	// Get player and enemy directions
	uint8_t enemy_dir = enemy->player->phys_obj->obj->y_flip;
	uint8_t user_dir = user->player->phys_obj->obj->y_flip;

	// Get player and enemy distance
	int16_t dist_raw_x = user->player->phys_obj->obj->pos.x - enemy->player->phys_obj->obj->pos.x;
	uint8_t dist_x = ABS(dist_raw_x);

	uint8_t on_right = dist_raw_x >= 0;
	uint8_t random_1 = rand(42);
	uint8_t random_2 = rand(69);

	//printf("%d | %d | %d | RANDOMS: %d | %d \n", dist_raw_x, dist_x, ((user->player->jump_state & 0xF0) != NOT_JUMPING), random_1, random_2);

	// Pretty far from player
	if (dist_x > 60)
	{
		// Move towards player fast
		if (on_right) player_move(enemy->player, scene_buf, obj_buf, 2);
		else player_move(enemy->player, scene_buf, obj_buf, -2);

		if (random_1 && random_2) player_move_jump(enemy->player);
	}
	// Pretty close from player
	else if (dist_x > 32)
	{
		if (user->player->jump_state != NOT_JUMPING)
		{
			// Move away player slow
			if (on_right) player_move(enemy->player, scene_buf, obj_buf, 2);
			else player_move(enemy->player, scene_buf, obj_buf, -2);
			player_move_jump(enemy->player);
		}
		else
		{
			// Move towards player slow
			if (on_right) player_move(enemy->player, scene_buf, obj_buf, 1);
			else player_move(enemy->player, scene_buf, obj_buf, -1);
		}
	}
	else if (dist_x > 15)
	{
		
		if (user->player->phys_obj->vel.x != 0)
		{
			if (enemy->player->health >= 50)
			{
				player_stop(enemy->player, 0);
			}
			else
			{
				// Either 1 or 2 depending on random value
				int8_t vel = 1 + random_1 * 2;

				//jump away from player fast
				if (on_right) player_move(enemy->player, scene_buf, obj_buf, 0 - vel);
				else player_move(enemy->player, scene_buf, obj_buf, vel);
			}
		}
		else
		{
			if (on_right) player_move(enemy->player, scene_buf, obj_buf, 1);
			else player_move(enemy->player, scene_buf, obj_buf, -1);
		}

	}
	// Next to player
	else
	{
		//
		/*if (user->player->health >= 50)
		{
			attack 1
		}
		else
		{
			attack 2
		}*/


		if (enemy->player->health >= 50)
		{
			// Move away from player slow
			if (on_right) player_move(enemy->player, scene_buf, obj_buf, -1);
			else player_move(enemy->player, scene_buf, obj_buf, 1);
			
		}
		else
		{
			//jump away from player fast
			if (on_right) player_move(enemy->player, scene_buf, obj_buf, -2);
			else player_move(enemy->player, scene_buf, obj_buf, 2);
			player_move_jump(enemy->player);
		}
	}
}
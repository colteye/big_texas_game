#include "enemy.h"
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
	if (is_null(enemy)) return;
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
	}
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
#include "input.h"

void check_input(struct player_t *player, struct scene_buf_t *scene_buf, struct key_status_t key_s)
{
	if (key_s.left_key_s)
	{
		move_left(player);
	}
	else if (key_s.right_key_s)
	{
		move_right(player, scene_buf);
	}
	else
	{
		move_stop_x(player);
	}

	if (key_s.jump_key_s)
	{
		move_jump(player);
	}
}
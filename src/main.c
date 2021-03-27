#include <stdint.h>
#include <time.h>

#include "scene.h"
#include "player.h"
#include "render.h"

#include <stdio.h>
#include "driver/gl_driver.h"

void delay(uint32_t ms)
{
	clock_t start = clock();
	uint32_t elapsed_ms = 0;

	while (elapsed_ms < ms)
	{
		clock_t end = clock();
		elapsed_ms = (end - start);
	}
}

int main(void)
{
	struct render_buf_t render_buf;
	struct scene_buf_t scene_buf;
	struct phys_obj_buf_t phys_obj_buf;
	phys_obj_buf.filled = 0;
	struct obj_buf_t obj_buf;
	obj_buf.filled = 0;

	init_render_buf(&render_buf);

	struct cbt_sprite scene_sprite = open_cbt_sprite("big_texas_game_scene_2.cbt");
	struct scene_t scene = init_scene((struct u_bounds_t){ {0, 104}, {128, 0} }, &scene_sprite);

	init_scene_buf(&scene_buf, &scene);

	struct cbta_sprite player_sprite = open_cbta_sprite("big_texas_game_player.cbta");
	struct u_vec2 player_pos = { 16, 72 };
	struct u_bounds_t player_bounds = { {player_pos.x, player_pos.y + 32 }, {player_pos.x + 32, player_pos.y} };

	struct object_t *player_obj = init_obj(&obj_buf, player_pos, player_bounds, &player_sprite);
	struct phys_object_t *player_phys_obj = init_phys_obj(&phys_obj_buf, player_obj, (struct vec2){ 0, 0 }, (struct vec2){ 0, 0 });

	struct player_t player = init_player(player_phys_obj);

	struct cbt_sprite ui_char_sheet = open_cbt_sprite("ui_text.cbt");

	GL_init();

	while (GL_is_open())
	{
		struct key_status_t key_s = GL_check_keys();
		check_input(&player, &scene_buf, key_s);

		physics_tick(&phys_obj_buf, &scene);
		
		draw_scene(&render_buf, &scene_buf);

		draw_all_obj(&render_buf, &obj_buf);

		draw_panel(&render_buf, (struct u_vec2){ 0, 0 }, 128, 20);

		draw_str(&render_buf, (struct u_vec2){ 8, 2 }, "HEALTH", 6, &ui_char_sheet);
		draw_str(&render_buf, (struct u_vec2){ 8, 10 }, "POINTS", 6, &ui_char_sheet);

		draw_uint8(&render_buf, (struct u_vec2){ 64, 2 }, player.health, &ui_char_sheet);
		draw_uint32(&render_buf, (struct u_vec2){ 64, 10 }, player.points, &ui_char_sheet);
		
		GL_render(&render_buf);

		// delay render for this many ms.
		delay(28);
	}

	return 0;
}

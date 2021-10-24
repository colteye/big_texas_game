#include <stdint.h>
#include <time.h>

#include "scene.h"
#include "render.h"

#include "enemy.h"
#include "user.h"

#include <stdio.h>
#include "driver/gl_driver.h"

int main(void)
{
	// Instantiate all relevant buffers.
	struct render_buf_t render_buf;
	struct scene_buf_t scene_buf;

	struct phys_obj_buf_t phys_obj_buf;
	phys_obj_buf.filled = 0;
	struct obj_buf_t obj_buf;
	obj_buf.filled = 0;

	render_buf_init(&render_buf);

	// Code to initialize the scene.
	struct cbt_sprite scene_sprite = cbt_sprite_open("big_texas_game_scene.cbt");
	struct scene_t scene = scene_init((struct u_bounds_t) { {64, 168}, { 192, 64 } }, &scene_sprite);
	scene_buf_init(&scene_buf, &scene);

	// Code to initialize the user.	
	struct cbta_sprite user_sprite = cbta_sprite_open("big_texas_game_player.cbta");
	struct u_vec2 user_pos = { 64, 136 };
	struct u_bounds_t user_bounds = { {user_pos.x, user_pos.y + 32 }, {user_pos.x + 32, user_pos.y} };

	struct object_t *user_obj = object_init(&obj_buf, user_pos, user_bounds, &user_sprite, COLLIDE_LAYER_1);
	struct phys_object_t *user_phys_obj = phys_object_init(&phys_obj_buf, user_obj, (struct vec2) { 0, 0 }, (struct vec2) { 0, -1 });
	struct player_t user_player = player_init(user_phys_obj);

	struct user_t user = user_init(&user_player, 10);

	// Code to initialize an enemy.	
	struct cbta_sprite enemy_sprite = cbta_sprite_open("big_texas_game_enemy.cbta");
	struct u_vec2 enemy_pos = { 160, 136 };
	struct u_bounds_t enemy_bounds = { {enemy_pos.x, enemy_pos.y + 32 }, {enemy_pos.x + 32, enemy_pos.y} };

	struct object_t *enemy_obj = object_init(&obj_buf, enemy_pos, enemy_bounds, &enemy_sprite, COLLIDE_LAYER_1);
	struct phys_object_t *enemy_phys_obj = phys_object_init(&phys_obj_buf, enemy_obj, (struct vec2) { 0, 0 }, (struct vec2) { 0, -1 });
	struct player_t enemy_player = player_init(enemy_phys_obj);

	struct enemy_t enemy = enemy_init(&enemy_player, 10);
	
	// Code to initialize the UI sprites.
	struct cbt_sprite ui_char_sheet = cbt_sprite_open("ui_text.cbt");

	// Init OpenGL backend.
	gl_init();

	while (gl_is_open())
	{
		// First check all inputs for changes.
		struct key_status_t key_s = gl_check_keys();
		user_move(&user, &scene_buf, &obj_buf, key_s);
		enemy_move(&enemy);

		// Once inputs are checked, run physics for the scene.
		physics_tick(&phys_obj_buf, &obj_buf, &scene);

		user_attack(&user, &enemy, key_s);
		enemy_attack(&enemy, &user);

		// Draw scene first.
		scene_buf_draw(&render_buf, &scene_buf);

		// Draw objects on top of scene.
		obj_buf_draw(&render_buf, &obj_buf);

		// Draw UI on top of everything else.
		panel_draw(&render_buf, (struct u_vec2){ 0, 0 }, 64, 12);
		panel_draw(&render_buf, (struct u_vec2) { 64, 0 }, 64, 12);

		string_draw(&render_buf, (struct u_vec2){ 66, 66 }, "CBOY", 4, &ui_char_sheet);

		string_draw(&render_buf, (struct u_vec2) { 130, 66 }, "TROL", 4, &ui_char_sheet);

		uint8_draw(&render_buf, (struct u_vec2){ 100, 66 }, user_player.health, &ui_char_sheet);
		uint8_draw(&render_buf, (struct u_vec2) { 164, 66 }, enemy_player.health, &ui_char_sheet);

		// After all is drawn, render.
		gl_render(&render_buf);

		// delay for consistent frame rate.
		gl_delay(28);
	}

	return 0;
}

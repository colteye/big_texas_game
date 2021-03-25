#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

// 256 8x8 tiles for 128x128 screen.
#define TILE_BYTE_SIZE 8
#define BIT_PER_BYTE 8
#define NUM_TILES 16
#define HEIGHT 128
#define WIDTH 128

#define BIT0 0b00000001
#define BIT1 0b00000010
#define BIT2 0b00000100
#define BIT3 0b00001000
#define BIT4 0b00010000
#define BIT5 0b00100000
#define BIT6 0b01000000
#define BIT7 0b10000000

#define N_BIT(x, n) ((x >> n) & 0x01)
#define IN_Y_RANGE(x) (x >= 0 && x <= HEIGHT)
#define IN_X_RANGE(x) (x >= 0 && x <= WIDTH)

uint8_t render_buffer[NUM_TILES * NUM_TILES * TILE_BYTE_SIZE];

uint8_t scene_buffer[NUM_TILES][NUM_TILES][TILE_BYTE_SIZE];

// WINDOWS SPECIFIC.
HDC hdc;

#define SHOOT_KEY VK_LBUTTON
#define PUNCH_KEY VK_RBUTTON
#define SPACE_KEY VK_SPACE
#define A_KEY 0x41
#define D_KEY 0x44

struct cbt_sprite
{
	uint8_t width;
	uint8_t height;
	uint8_t *data;
};

struct cbta_sprite
{
	uint8_t width;
	uint8_t height;
	uint8_t *data;
	uint8_t *mask;
};

struct u_vec2
{
	uint8_t x;
	uint8_t y;
};

struct vec2
{
	int8_t x;
	int8_t y;
};

struct u_bounds_t
{
	struct u_vec2 min;
	struct u_vec2 max;
};

struct player_t
{
	struct cbta_sprite *sprite;
	struct u_bounds_t bounds;
	struct u_vec2 pos;
};

struct scene_t
{
	struct cbt_sprite *sprite;
	struct u_bounds_t bounds;
};

struct scene_t scene;
struct player_t player;

struct cbt_sprite open_cbt_sprite(const char* fname)
{
	FILE *fp = fopen(fname, "rb");

	uint8_t head[2];
	fread(head, 1, 2, fp);

	uint16_t img_size = head[0] * head[1] / 8;

	uint8_t *data = (uint8_t *)malloc(img_size);

	fread(data, 1, img_size, fp);

	struct cbt_sprite out = { head[0], head[1], data };
	return out;
}

struct cbta_sprite open_cbta_sprite(const char* fname)
{
	FILE *fp = fopen(fname, "rb");

	uint8_t head[2];
	fread(head, 1, 2, fp);

	uint16_t img_size = head[0] * head[1] / 8;

	uint8_t *data = (uint8_t *)malloc(img_size);
	uint8_t *mask = (uint8_t *)malloc(img_size);

	fread(data, 1, img_size, fp);
	fread(mask, 1, img_size, fp);

	struct cbta_sprite out = { head[0], head[1], data, mask };
	return out;
}

void init(void)
{
	for (int x = 0; x < NUM_TILES; x++)
	{
		for (int y = 0; y < NUM_TILES; y++)
		{
			for (int i = 0; i < TILE_BYTE_SIZE; i++)
			{
				render_buffer[(y * TILE_BYTE_SIZE + i) * NUM_TILES + x] = 0xFF;
				scene_buffer[x][y][i] = 0xFF;
			}
		}
	}	
	hdc = GetDC(GetConsoleWindow());
}

void render_tick(void)
{
	HBITMAP map = CreateBitmap(128, // width. 512 in my case
		128, // height
		1, // Color Planes, unfortanutelly don't know what is it actually. Let it be 1
		1, // Size of memory for one pixel in bits (in win32 4 bytes = 4*8 bits)
		(void*)render_buffer); // pointer to array

	HDC src = CreateCompatibleDC(hdc); // hdc - Device context for window, I've got earlier with GetDC(hWnd) or GetDC(NULL);
	SelectObject(src, map); // Inserting picture into our temp HDC
	// Copy image from temp HDC to window
	/*BitBlt(hdc, // Destination
		0,  // x and
		0,  // y - upper-left corner of place, where we'd like to copy
		128, // width of the region
		128, // height
		src, // source
		0,   // x and
		0,   // y of upper left corner  of part of the source, from where we'd like to copy
		SRCCOPY); // Defined DWORD to juct copy pixels. Watch more on msdn;*/

	// Can stretch to any power of 2 square size.
	SetStretchBltMode(hdc, COLORONCOLOR);
	StretchBlt(hdc, 0, 0, 512, 512, src, 0, 0, 128, 128, SRCCOPY);


	DeleteDC(src); // Deleting temp HDC
}


void render_tick_old(void)
{
	for (int x = 0; x < NUM_TILES; ++x)
	{
		for (int y = 0; y < NUM_TILES; ++y)
		{
			for (int i = 0; i < TILE_BYTE_SIZE; ++i)
			{
				for (int j = 0; j < BIT_PER_BYTE; ++j)
				{
					uint8_t current_color = N_BIT(render_buffer[(y * TILE_BYTE_SIZE + i) * NUM_TILES + x], j) * 255;
					SetPixel(hdc, x*TILE_BYTE_SIZE + j, y*TILE_BYTE_SIZE + i, RGB(current_color, current_color, current_color));
				}
			}
		}
	}
}

void init_scene(void)
{
	for (int x = 0; x < NUM_TILES; ++x)
	{
		for (int y = 0; y < NUM_TILES; ++y)
		{
			for (int i = 0; i < TILE_BYTE_SIZE; ++i)
			{
				scene_buffer[x][y][i] = scene.sprite->data[(y * TILE_BYTE_SIZE + i) * NUM_TILES + x];
			}
		}
	}
}

void draw_scene(void)
{
	for (int x = 0; x < NUM_TILES; ++x)
	{
		for (int y = 0; y < NUM_TILES; ++y)
		{
			for (int i = 0; i < TILE_BYTE_SIZE; ++i)
			{
				// Render scene from scene buffer!
				render_buffer[(y * TILE_BYTE_SIZE + i) * NUM_TILES + x] = scene_buffer[x][y][i];
			}
		}
	}
}

void draw_player(void)
{
	for (int y = 0; y < player.sprite->height; ++y)
	{
		uint8_t prev_data = player.sprite->data[y * (player.sprite->width / TILE_BYTE_SIZE)];
		uint8_t carry_data = (prev_data << 8 - (player.pos.x % TILE_BYTE_SIZE));

		uint8_t prev_mask = player.sprite->mask[y * (player.sprite->width / TILE_BYTE_SIZE)];
		uint8_t carry_mask = (prev_mask << 8 - (player.pos.x % TILE_BYTE_SIZE));

		for (int x = 0; x < player.sprite->width / TILE_BYTE_SIZE; ++x)
		{
			prev_data = player.sprite->data[y * (player.sprite->width / TILE_BYTE_SIZE) + x];
			prev_mask = player.sprite->mask[y * (player.sprite->width / TILE_BYTE_SIZE) + x];

			uint8_t player_data = player.sprite->data[y * (player.sprite->width / TILE_BYTE_SIZE) + x] >> (player.pos.x % TILE_BYTE_SIZE) | carry_data;
			uint8_t player_mask = player.sprite->mask[y * (player.sprite->width / TILE_BYTE_SIZE) + x] >> (player.pos.x % TILE_BYTE_SIZE) | carry_mask;

			render_buffer[(player.pos.y + y) * NUM_TILES + ((player.pos.x / TILE_BYTE_SIZE) + x)] &= ~player_mask;
			render_buffer[(player.pos.y + y) * NUM_TILES + ((player.pos.x / TILE_BYTE_SIZE) + x)] |= (player_data & player_mask);

			carry_data = (prev_data << 8 - (player.pos.x % TILE_BYTE_SIZE));
			carry_mask = (prev_mask << 8 - (player.pos.x % TILE_BYTE_SIZE));
		}
	}
}

void scroll_scene_right(uint8_t pixels)
{
	for (int y = 0; y < NUM_TILES; ++y)
	{
		for (int i = 0; i < TILE_BYTE_SIZE; ++i)
		{
			uint8_t prev = scene_buffer[NUM_TILES - 1][y][i];
			uint8_t carry = (prev << 8 - pixels);

			for (int x = 0; x < NUM_TILES; ++x)
			{
				prev = scene_buffer[x][y][i];

				scene_buffer[x][y][i] = (scene_buffer[x][y][i] >> pixels) | carry;

				carry = (prev << 8 - pixels);
			}
		}
	}
}

void scroll_scene_left(uint8_t pixels)
{
	for (int y = 0; y < NUM_TILES; ++y)
	{
		for (int i = 0; i < TILE_BYTE_SIZE; ++i)
		{
			uint8_t prev = scene_buffer[0][y][i];
			uint8_t carry = (prev >> 8 - pixels);

			for (int x = NUM_TILES - 1; x >= 0; --x)
			{
				prev = scene_buffer[x][y][i];

				scene_buffer[x][y][i] = (scene_buffer[x][y][i] << pixels) | carry;

				carry = (prev >> 8 - pixels);
			}
		}
	}
}

struct vec2 vel = { 0, 0 };
struct vec2 acc = { 0, 0 };

void move_player(struct player_t *player, struct vec2 pos)
{
	if (!IN_X_RANGE(player->bounds.min.x + pos.x) ||
		!IN_X_RANGE(player->bounds.max.x + pos.x) ||
		!IN_Y_RANGE(player->bounds.min.y - pos.y) ||
		!IN_Y_RANGE(player->bounds.max.y - pos.y))
		return;

	player->bounds.min.x += pos.x;
	player->bounds.max.x += pos.x;

	player->bounds.min.y -= pos.y;
	player->bounds.max.y -= pos.y;

	player->pos.x += pos.x;
	player->pos.y -= pos.y;
}

uint8_t collide_within_bounds(struct player_t *player, struct u_bounds_t *bounds)
{
	// Collision status: 0000 1    1    1    1    
	//                       up down left right	
	uint8_t collision_status = 0x00;

	if (bounds->max.y >= player->bounds.max.y)
	{
		uint8_t delta = bounds->max.y - player->bounds.max.y;
		player->pos.y += delta;
		player->bounds.min.y += delta;
		player->bounds.max.y += delta;

		vel.y = 0;
		collision_status |= BIT0;
	}

	if (bounds->min.y <= player->bounds.min.y)
	{
		uint8_t delta = player->bounds.min.y - bounds->min.y;
		player->pos.y -= delta;
		player->bounds.min.y -= delta;
		player->bounds.max.y -= delta;

		vel.y = 0;
		collision_status |= BIT1;
	}

	if (bounds->min.x >= player->bounds.min.x)
	{
		uint8_t delta = bounds->min.x - player->bounds.min.x;
		player->pos.x += delta;
		player->bounds.min.x += delta;
		player->bounds.max.x += delta;

		vel.x = 0;
		collision_status |= BIT2;
	}

	if (bounds->max.x <= player->bounds.max.x)
	{
		uint8_t delta =  player->bounds.max.x - bounds->max.x;
		player->pos.x -= delta;
		player->bounds.min.x -= delta;
		player->bounds.max.x -= delta;

		vel.x = 0;
		collision_status |= BIT3;
	}

	return collision_status;
}

uint8_t player_col_stat = 0x00;

void physics_tick(void)
{
	struct vec2 pos_update = { vel.x + acc.x / 2,
							   vel.y + acc.y / 2 };
	move_player(&player, pos_update);

	player_col_stat = collide_within_bounds(&player, &scene.bounds);

	vel.x += acc.x;
	vel.y += acc.y;
}

uint8_t jump_state = 0x00;
#define NOT_JUMPING 0x00
#define START_JUMP 0x10
#define END_JUMP 0x20
#define MAX_STACKED_JUMPS 2

void jump(void)
{
	switch (jump_state & 0xF0) {

	case NOT_JUMPING:
		vel.y = 6;
		acc.y = -1;
		jump_state = (jump_state & 0x0F) + START_JUMP;
		break;

	case START_JUMP:
		if ((jump_state & 0x0F) >= MAX_STACKED_JUMPS)
		{
			jump_state = END_JUMP;
		}
		else
		{
			jump_state = (jump_state & 0x0F) + NOT_JUMPING + 1;
		}
		break;

	case END_JUMP:
		if (player_col_stat & BIT1) 
		{
			jump_state = (jump_state & 0x0F) + NOT_JUMPING;
		}
		break;
	}
}

uint8_t is_key_down(uint32_t keyCode)
{
	return ((GetAsyncKeyState(keyCode) & 0x8000) ? 1 : 0);
};

uint8_t is_key_up(uint32_t keyCode)
{
	return ((GetAsyncKeyState(keyCode) & 0x8000) ? 0 : 1);
};

void check_input(void)
{
	if (is_key_down(A_KEY))
	{
		if (player_col_stat & BIT2) vel.x = 0;
		else vel.x = -2;
	}
	else if (is_key_down(D_KEY))
	{
		if (player.pos.x < 48)
		{
			vel.x = 2;
			scroll_scene_left(2);
		}	
		else
		{
			vel.x = 0;
			scroll_scene_left(3);
		}
	}
	else
	{
		vel.x = 0;
	}

	if (is_key_down(SPACE_KEY))
	{ 
		jump();
	}
}

void draw_text(u_vec2 pos, cbt_sprite *sprite, u_vec2 size, u_vec2 offset)
{
	for (int y = 0; y < size.y; ++y)
	{
		uint8_t prev_data = sprite->data[(y + offset.y) * (sprite->width / TILE_BYTE_SIZE) + (offset.x / TILE_BYTE_SIZE)];
		uint8_t carry_data = (prev_data << 8 - (pos.x % TILE_BYTE_SIZE));

		for (int x = 0; x < size.x / TILE_BYTE_SIZE; ++x)
		{
			prev_data = sprite->data[(y + offset.y) * (sprite->width / TILE_BYTE_SIZE) + x + (offset.x / TILE_BYTE_SIZE)];

			uint8_t data = sprite->data[(y + offset.y) * (sprite->width / TILE_BYTE_SIZE) + x + (offset.x / TILE_BYTE_SIZE)] >> (pos.x % TILE_BYTE_SIZE) | carry_data;

			render_buffer[(pos.y + y) * NUM_TILES + ((pos.x / TILE_BYTE_SIZE) + x)] &= ~data;
			render_buffer[(pos.y + y) * NUM_TILES + ((pos.x / TILE_BYTE_SIZE) + x)] |= (0x00 & data);

			carry_data = (prev_data << 8 - (pos.x % TILE_BYTE_SIZE));
		}
	}
}

struct cbt_sprite ui_text_sprite;

void draw_str_internal(u_vec2 pos, char ch, u_vec2 char_offset)
{
	uint8_t n_char = ch - char_offset.x;

	u_vec2 offset = { (n_char % 8) * 8,
						((n_char / 8) + char_offset.y) * 8 };

	draw_text({ pos.x , pos.y }, &ui_text_sprite, { 8, 8 }, offset);
}

void draw_str(u_vec2 pos, const char* str, size_t len)
{
	for (uint8_t i = 0; i < len; ++i)
	{
		u_vec2 char_offset = { 0, 0 };

		switch (str[i]) {
		case '!':
			draw_text({ pos.x , pos.y }, &ui_text_sprite, { 8, 8 }, { 32, 32 });
			break;
		case '?':
			draw_text({ pos.x , pos.y }, &ui_text_sprite, { 8, 8 }, { 40, 32 });
			break;
		case '.':
			draw_text({ pos.x , pos.y }, &ui_text_sprite, { 8, 8 }, { 48, 32 });
			break;
		case ',':
			draw_text({ pos.x , pos.y }, &ui_text_sprite, { 8, 8 }, { 56, 32 });
			break;
		default:
			if (str[i] > 64 && str[i] < 91)
			{
				char_offset = { 65 - 2, 1 };
				draw_str_internal(pos, str[i], char_offset);
			}
			else if (str[i] > 47 && str[i] < 58)
			{
				char_offset = { 48, 0 };
				draw_str_internal(pos, str[i], char_offset);
			}
			break;
		}
		pos.x += 8;
	}

	// 0 = {16, 8}
	// 1 = {24, 8}
	// 2 = {32, 8}
	// 3 = {40, 8}
	// 4 = {48, 8}
	// 5 = {56, 8}

	// 6 = {0, 16}
	// 7 = {8, 16}
	// 8 = {16, 16}
	// 9 = {24, 16}
	// 10 = {32, 16}
	// 11 = {40, 16}
	// 12 = {48, 16}
	// 13 = {56, 16}
}

void draw_uint8(u_vec2 pos, uint8_t num)
{
	uint8_t i = 2;
	uint8_t buf[3] = { 255, 255, 255 };

	while (num > 0)
	{
		uint8_t dig = num % 10;
		buf[i] = dig;
		num = num / 10;
		pos.x += 8;
		--i;
	}

	for (i = 0; i < 3; ++i)
	{
		if (buf[i] < 10)
		{
			draw_str_internal(pos, buf[i], { 0, 0 });
			pos.x += 8;
		}	
	}
}





void draw_panel(u_vec2 pos, uint8_t width, uint8_t height)
{
	uint8_t top_bottom[3] = { 0b00111111, 0b11111111, 0b11111100 };
	uint8_t middle[3] = { 0b01111111, 0b11111111, 0b11111110 };

	for (int y = 0; y < height; ++y)
	{
		uint8_t data = 0b11111111;
		uint8_t prev_data = data;
		uint8_t carry_data = (prev_data << 8 - (pos.x % TILE_BYTE_SIZE));

		for (int x = 0; x < width / TILE_BYTE_SIZE; ++x)
		{
			if (y == 0 || y == height - 1)
			{
				data = 0b00000000;
			}
			else if (y == 1 || y == height - 2)
			{
				if (x == 0) data = top_bottom[0];
				else if (x == width / TILE_BYTE_SIZE - 1) data = top_bottom[2];
				else  data = top_bottom[1];
			}
			else
			{
				if (x == 0) data = middle[0];
				else if (x == width / TILE_BYTE_SIZE - 1) data = middle[2];
				else  data = middle[1];
			}

			prev_data = data;

			uint8_t player_data = data >> (pos.x % TILE_BYTE_SIZE) | carry_data;

			render_buffer[(pos.y + y) * NUM_TILES + ((pos.x / TILE_BYTE_SIZE) + x)] = 0x00 | player_data;

			carry_data = (prev_data << 8 - (pos.x % TILE_BYTE_SIZE));
		}
	}
}

void main(void)
{
	init();

	ui_text_sprite = open_cbt_sprite("ui_text.cbt");

	struct cbta_sprite player_sprite = open_cbta_sprite("big_texas_game_player.cbta");
	player.sprite = &player_sprite;

	player.pos.x = 16;
	player.pos.y = 72;

	uint8_t bound_x_p = player.pos.x + 32;
	uint8_t bound_y_p = player.pos.y + 32;

	player.bounds.min.x = player.pos.x;
	player.bounds.max.x = bound_x_p;

	player.bounds.min.y = bound_y_p;
	player.bounds.max.y = player.pos.y;


	struct cbt_sprite scene_sprite = open_cbt_sprite("big_texas_game_scene_2.cbt");
	scene.sprite = &scene_sprite;

	scene.bounds.min.x = 0;
	scene.bounds.max.x = 128;
	
	scene.bounds.max.y = 0;
	scene.bounds.min.y = 104;

	init_scene();

	uint8_t frame_ms= 28;
	uint8_t frame_elapsed_ms = 0;
	clock_t start = clock();
	while (1)
	{
		clock_t end = clock();
		frame_elapsed_ms = (end - start);
		
		if (frame_elapsed_ms == frame_ms)
		{
			check_input();

			physics_tick();
		
			draw_scene();
			draw_player();

			draw_panel({ 0, 0 }, 128, 20);

			draw_str({ 8, 2 }, "HEALTH", 6);
			draw_str({ 8, 10 }, "POINTS", 6);

			draw_uint8({ 64, 2 }, 100);
			draw_uint8({ 64, 10 }, 255);
			
			render_tick();
			
			frame_elapsed_ms = 0;
			start = clock();
		}
	}
}
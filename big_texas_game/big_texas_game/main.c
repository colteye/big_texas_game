#include <stdint.h>
#include <stdio.h>
#include <time.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

// 256 8x8 tiles for 128x128 screen.
#define TILE_BYTE_SIZE 8
#define BIT_PER_BYTE 8
#define NUM_TILES 16
#define HEIGHT 128
#define WIDTH 128

#define N_BIT(x, n) ((x >> n) & 0x01)
#define IN_Y_RANGE(x) (x >= 0 && x <= HEIGHT)
#define IN_X_RANGE(x) (x >= 0 && x <= WIDTH)

uint8_t render_buffer[NUM_TILES * NUM_TILES * TILE_BYTE_SIZE];

uint8_t scene_buffer[NUM_TILES][NUM_TILES][TILE_BYTE_SIZE];

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
}

void render_tick(void)
{
	/*HBITMAP map = CreateBitmap(128, // width. 512 in my case
		128, // height
		1, // Color Planes, unfortanutelly don't know what is it actually. Let it be 1
		1, // Size of memory for one pixel in bits (in win32 4 bytes = 4*8 bits)
		(void*)render_buffer); // pointer to array

	HDC src = CreateCompatibleDC(hdc); // hdc - Device context for window, I've got earlier with GetDC(hWnd) or GetDC(NULL);
	SelectObject(src, map); // Inserting picture into our temp HDC
	// Copy image from temp HDC to window
	BitBlt(hdc, // Destination
		0,  // x and
		0,  // y - upper-left corner of place, where we'd like to copy
		128, // width of the region
		128, // height
		src, // source
		0,   // x and
		0,   // y of upper left corner  of part of the source, from where we'd like to copy
		SRCCOPY); // Defined DWORD to juct copy pixels. Watch more on msdn;

	DeleteDC(src); // Deleting temp HDC*/
}


void render_tick_old(void)
{
	/*for (int x = 0; x < NUM_TILES; ++x)
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
	}*/
}

void init_scene()
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

void draw_scene()
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

void draw_player()
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

			render_buffer[(player.pos.y + y) * NUM_TILES + ((player.pos.x / TILE_BYTE_SIZE) + x)] = (render_buffer[(player.pos.y + y) * NUM_TILES + ((player.pos.x / TILE_BYTE_SIZE) + x)] & ~player_mask) | (player_data & player_mask);

			//render_buffer[(player.pos.y + y) * NUM_TILES + ((player.pos.x / TILE_BYTE_SIZE) + x)] = player_mask;

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

	//printf("x pos %d", player->bounds.min.y + pos.x);


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

void collide_within_bounds(struct player_t *player, struct u_bounds_t *bounds)
{
	if (bounds->min.x > player->bounds.min.x)
	{
		uint8_t delta = bounds->min.x - player->bounds.min.x;
		player->pos.x += delta;
		player->bounds.min.x += delta;
		player->bounds.max.x += delta;

		vel.x = 0;
	}

	if (bounds->max.x < player->bounds.max.x)
	{
		uint8_t delta =  player->bounds.max.x - bounds->max.x;
		player->pos.x -= delta;
		player->bounds.min.x -= delta;
		player->bounds.max.x -= delta;

		vel.x = 0;
	}

	if (bounds->min.y < player->bounds.min.y)
	{
		uint8_t delta = player->bounds.min.y - bounds->min.y;
		player->pos.y -= delta;
		player->bounds.min.y -= delta;
		player->bounds.max.y -= delta;

		vel.y = 0;
	}

	if (bounds->max.y > player->bounds.max.y)
	{
		uint8_t delta = bounds->max.y - player->bounds.max.y;
		player->pos.y += delta;
		player->bounds.min.y += delta;
		player->bounds.max.y += delta;

		vel.y = 0;
	}
	
}

void physics_tick()
{
	struct vec2 pos_update = { vel.x + acc.x / 2,
							   vel.y + acc.y / 2 };
	move_player(&player, pos_update);

	collide_within_bounds(&player, &scene.bounds);

	vel.x += acc.x;
	vel.y += acc.y;
}

void jump()
{
	vel.y = 6;
	acc.y = -1;
}

uint8_t is_key_down(uint32_t keyCode)
{
	//return ((GetAsyncKeyState(keyCode) & 0x8000) ? 1 : 0);

	return 0;
};

uint8_t is_key_up(uint32_t keyCode)
{
	//return ((GetAsyncKeyState(keyCode) & 0x8000) ? 0 : 1);

	return 0;
};

void check_input(void)
{
	/*if (is_key_down(A_KEY))
	{
		vel.x = -1;
		scroll_scene_right(3);
	}
	else if (is_key_down(D_KEY))
	{
		vel.x = 1;
		scroll_scene_left(3);
	}
	else
	{
		vel.x = 0;
	}

	if (is_key_down(SPACE_KEY))
	{ 
		jump();
	}*/
}

void main(void)
{
	init();

	struct cbta_sprite player_sprite = open_cbta_sprite("big_texas_game_player.cbta");
	player.sprite = &player_sprite;

	player.pos.x = 48;
	player.pos.y = 72;

	uint8_t bound_x_p = player.pos.x + 32;
	uint8_t bound_y_p = player.pos.y - 32;

	player.bounds.min.x = player.pos.x;
	player.bounds.min.y = player.pos.y;

	player.bounds.max.x = bound_x_p;
	player.bounds.max.y = bound_y_p;


	struct cbt_sprite scene_sprite = open_cbt_sprite("big_texas_game_scene_troll.cbt");
	scene.sprite = &scene_sprite;

	scene.bounds.min.x = 0;
	scene.bounds.min.y = 72;

	scene.bounds.max.x = 128;
	scene.bounds.max.y = 0;

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
			
			render_tick();
			
			frame_elapsed_ms = 0;
			start = clock();
		}
	}
}
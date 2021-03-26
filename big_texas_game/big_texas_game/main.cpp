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

#define MAX_PHYS_OBJ 16
#define MAX_OBJ 32

#define N_BIT(x, n) ((x >> n) & 0x01)
#define IN_Y_RANGE(x) (x >= 0 && x <= HEIGHT)
#define IN_X_RANGE(x) (x >= 0 && x <= WIDTH)

#define NOT_JUMPING 0x00
#define START_JUMP 0x10
#define END_JUMP 0x20
#define MAX_STACKED_JUMPS 2

#define SHOOT_KEY VK_LBUTTON
#define PUNCH_KEY VK_RBUTTON
#define SPACE_KEY VK_SPACE
#define A_KEY 0x41
#define D_KEY 0x44

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

struct u_bounds_t
{
	struct u_vec2 min;
	struct u_vec2 max;
};

struct object_t
{
	struct cbta_sprite *sprite;
	struct u_bounds_t bounds;
	struct u_vec2 pos;
	uint8_t modified;
};

struct phys_object_t
{
	struct object_t *obj;
	struct vec2 vel;
	struct vec2 acc;
	uint8_t collision_status;
};

struct player_t
{
	phys_object_t *phys_obj;
	uint32_t points;
	uint8_t health;
	uint8_t jump_state;
};

// Scene not actually an object, dont keep track of pos.
struct scene_t
{
	struct cbt_sprite *sprite;
	struct u_bounds_t bounds;
	uint8_t modified;
};

struct render_buf_t
{
	uint8_t buf[NUM_TILES * NUM_TILES * TILE_BYTE_SIZE];
};

struct scene_buf_t
{
	uint8_t buf[NUM_TILES][NUM_TILES][TILE_BYTE_SIZE];
};

struct phys_obj_buf_t
{
	struct phys_object_t buf[MAX_PHYS_OBJ];
	uint8_t filled;
};

struct obj_buf_t
{
	struct object_t buf[MAX_PHYS_OBJ];
	uint8_t filled;
};

struct cbt_sprite open_cbt_sprite(const char *fname)
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

struct cbta_sprite open_cbta_sprite(const char *fname)
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

/*void render_tick_old(void)
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
}*/

void init_render_buf(struct render_buf_t *render_buf)
{
	for (int x = 0; x < NUM_TILES; x++)
	{
		for (int y = 0; y < NUM_TILES; y++)
		{
			for (int i = 0; i < TILE_BYTE_SIZE; i++)
			{
				render_buf->buf[(y * TILE_BYTE_SIZE + i) * NUM_TILES + x] = 0xFF;
			}
		}
	}	
}

void render_tick(struct render_buf_t *render_buf, HDC *hdc)
{
	HBITMAP map = CreateBitmap(128, // width. 512 in my case
		128, // height
		1, // Color Planes, unfortanutelly don't know what is it actually. Let it be 1
		1, // Size of memory for one pixel in bits (in win32 4 bytes = 4*8 bits)
		(void*)render_buf->buf); // pointer to array

	HDC src = CreateCompatibleDC(*hdc); // hdc - Device context for window, I've got earlier with GetDC(hWnd) or GetDC(NULL);
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
	SetStretchBltMode(*hdc, COLORONCOLOR);
	StretchBlt(*hdc, 0, 0, 768, 768, src, 0, 0, 128, 128, SRCCOPY);

	DeleteDC(src); // Deleting temp HDC
}

void init_scene_buf(struct scene_buf_t *scene_buf, struct scene_t *scene)
{
	for (int x = 0; x < NUM_TILES; ++x)
	{
		for (int y = 0; y < NUM_TILES; ++y)
		{
			for (int i = 0; i < TILE_BYTE_SIZE; ++i)
			{
				scene_buf->buf[x][y][i] = scene->sprite->data[(y * TILE_BYTE_SIZE + i) * NUM_TILES + x];
			}
		}
	}
}

void draw_scene(struct render_buf_t *render_buf, struct scene_buf_t *scene_buf)
{
	for (int x = 0; x < NUM_TILES; ++x)
	{
		for (int y = 0; y < NUM_TILES; ++y)
		{
			for (int i = 0; i < TILE_BYTE_SIZE; ++i)
			{
				// Render scene from scene buffer!
				render_buf->buf[(y * TILE_BYTE_SIZE + i) * NUM_TILES + x] = scene_buf->buf[x][y][i];
			}
		}
	}
}
void draw_obj(struct render_buf_t *render_buf, struct object_t *obj)
{
	for (int y = 0; y < obj->sprite->height; ++y)
	{
		uint8_t prev_data = 0x00;
		uint8_t prev_mask = 0x00;
		uint8_t carry_data = 0x00;
		uint8_t carry_mask = 0x00;
		uint8_t data = 0x00;
		uint8_t mask = 0x00;

		for (int x = 0; x < obj->sprite->width / TILE_BYTE_SIZE; ++x)
		{
			prev_data = obj->sprite->data[y * (obj->sprite->width / TILE_BYTE_SIZE) + x];
			prev_mask = obj->sprite->mask[y * (obj->sprite->width / TILE_BYTE_SIZE) + x];

			data = prev_data >> (obj->pos.x % TILE_BYTE_SIZE) | carry_data;
			mask = prev_mask >> (obj->pos.x % TILE_BYTE_SIZE) | carry_mask;

			render_buf->buf[(obj->pos.y + y) * NUM_TILES + ((obj->pos.x / TILE_BYTE_SIZE) + x)] &= ~mask;
			render_buf->buf[(obj->pos.y + y) * NUM_TILES + ((obj->pos.x / TILE_BYTE_SIZE) + x)] |= (data & mask);

			carry_data = (prev_data << 8 - (obj->pos.x % TILE_BYTE_SIZE));
			carry_mask = (prev_mask << 8 - (obj->pos.x % TILE_BYTE_SIZE));
		}

		// Make sure to add a bit of padding if the sprite goes over!
		data = 0x00 | carry_data;
		mask = 0x00 | carry_mask;
		render_buf->buf[(obj->pos.y + y) * NUM_TILES + ((obj->pos.x / TILE_BYTE_SIZE) + obj->sprite->width / TILE_BYTE_SIZE)] &= ~mask;
		render_buf->buf[(obj->pos.y + y) * NUM_TILES + ((obj->pos.x / TILE_BYTE_SIZE) + obj->sprite->width / TILE_BYTE_SIZE)] |= (data & mask);
	}
}

void draw_all_obj(struct render_buf_t *render_buf, struct obj_buf_t *obj_buf)
{
	for (uint8_t i = 0; i < obj_buf->filled; ++i)
	{
		draw_obj(render_buf, &obj_buf->buf[i]);
	}
}


void scroll_scene_right(struct scene_buf_t *scene_buf, uint8_t pixels)
{
	for (int y = 0; y < NUM_TILES; ++y)
	{
		for (int i = 0; i < TILE_BYTE_SIZE; ++i)
		{
			uint8_t prev = scene_buf->buf[NUM_TILES - 1][y][i];
			uint8_t carry = (prev << 8 - pixels);

			for (int x = 0; x < NUM_TILES; ++x)
			{
				prev = scene_buf->buf[x][y][i];

				scene_buf->buf[x][y][i] = (scene_buf->buf[x][y][i] >> pixels) | carry;

				carry = (prev << 8 - pixels);
			}
		}
	}
}

void scroll_scene_left(struct scene_buf_t *scene_buf, uint8_t pixels)
{
	for (int y = 0; y < NUM_TILES; ++y)
	{
		for (int i = 0; i < TILE_BYTE_SIZE; ++i)
		{
			uint8_t prev = scene_buf->buf[0][y][i];
			uint8_t carry = (prev >> 8 - pixels);

			for (int x = NUM_TILES - 1; x >= 0; --x)
			{
				prev = scene_buf->buf[x][y][i];

				scene_buf->buf[x][y][i] = (scene_buf->buf[x][y][i] << pixels) | carry;

				carry = (prev >> 8 - pixels);
			}
		}
	}
}

void move_obj(struct object_t *obj, struct vec2 *pos)
{
	if (!IN_X_RANGE(obj->bounds.min.x + pos->x) ||
		!IN_X_RANGE(obj->bounds.max.x + pos->x) ||
		!IN_Y_RANGE(obj->bounds.min.y - pos->y) ||
		!IN_Y_RANGE(obj->bounds.max.y - pos->y))
		return;

	obj->bounds.min.x += pos->x;
	obj->bounds.max.x += pos->x;

	obj->bounds.min.y -= pos->y;
	obj->bounds.max.y -= pos->y;

	obj->pos.x += pos->x;
	obj->pos.y -= pos->y;
}

void collide_phys_obj_in_bounds(struct phys_object_t *phys_obj, struct u_bounds_t *bounds)
{
	// Collision status: 0000 1    1    1    1    
	//                       up down left right	
	phys_obj->collision_status = 0x00;

	if (bounds->max.y >= phys_obj->obj->bounds.max.y)
	{
		uint8_t delta = bounds->max.y - phys_obj->obj->bounds.max.y;
		phys_obj->obj->pos.y += delta;
		phys_obj->obj->bounds.min.y += delta;
		phys_obj->obj->bounds.max.y += delta;

		phys_obj->vel.y = 0;

		phys_obj->collision_status |= BIT0;
	}

	if (bounds->min.y <= phys_obj->obj->bounds.min.y)
	{
		uint8_t delta = phys_obj->obj->bounds.min.y - bounds->min.y;
		phys_obj->obj->pos.y -= delta;
		phys_obj->obj->bounds.min.y -= delta;
		phys_obj->obj->bounds.max.y -= delta;

		phys_obj->vel.y = 0;

		phys_obj->collision_status |= BIT1;
	}

	if (bounds->min.x >= phys_obj->obj->bounds.min.x)
	{
		uint8_t delta = bounds->min.x - phys_obj->obj->bounds.min.x;
		phys_obj->obj->pos.x += delta;
		phys_obj->obj->bounds.min.x += delta;
		phys_obj->obj->bounds.max.x += delta;

		phys_obj->vel.x = 0;

		phys_obj->collision_status |= BIT2;
	}

	if (bounds->max.x <= phys_obj->obj->bounds.max.x)
	{
		uint8_t delta = phys_obj->obj->bounds.max.x - bounds->max.x;
		phys_obj->obj->pos.x -= delta;
		phys_obj->obj->bounds.min.x -= delta;
		phys_obj->obj->bounds.max.x -= delta;

		phys_obj->vel.x = 0;

		phys_obj->collision_status |= BIT3;
	}
}

void physics_tick(struct phys_obj_buf_t *phys_obj_buf, struct scene_t * scene)
{
	for (uint8_t i = 0; i < phys_obj_buf->filled; ++i)
	{
		struct vec2 pos_update = { phys_obj_buf->buf[i].vel.x + phys_obj_buf->buf[i].acc.x / 2,
								   phys_obj_buf->buf[i].vel.y + phys_obj_buf->buf[i].acc.y / 2 };

		move_obj(phys_obj_buf->buf[i].obj, &pos_update);

		// Collide with scene.
		collide_phys_obj_in_bounds(&phys_obj_buf->buf[i], &scene->bounds);

		phys_obj_buf->buf[i].vel.x += phys_obj_buf->buf[i].acc.x;
		phys_obj_buf->buf[i].vel.y += phys_obj_buf->buf[i].acc.y;
	}
}

void jump(struct player_t *player)
{
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

uint8_t is_key_down(uint32_t keyCode)
{
	return ((GetAsyncKeyState(keyCode) & 0x8000) ? 1 : 0);
};

uint8_t is_key_up(uint32_t keyCode)
{
	return ((GetAsyncKeyState(keyCode) & 0x8000) ? 0 : 1);
};

void check_input(struct player_t *player, struct scene_buf_t *scene_buf)
{
	if (is_key_down(A_KEY))
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
	else if (is_key_down(D_KEY))
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
	else
	{
		player->phys_obj->vel.x = 0;
	}

	if (is_key_down(SPACE_KEY))
	{ 
		jump(player);
	}
}

void draw_sprite_offset(struct render_buf_t *render_buf, struct u_vec2 pos, struct cbt_sprite *sprite, struct u_vec2 size, struct u_vec2 offset)
{
	for (int y = 0; y < size.y; ++y)
	{
		uint8_t prev_data = sprite->data[(y + offset.y) * (sprite->width / TILE_BYTE_SIZE) + (offset.x / TILE_BYTE_SIZE)];
		uint8_t carry_data = (prev_data << 8 - (pos.x % TILE_BYTE_SIZE));

		for (int x = 0; x < size.x / TILE_BYTE_SIZE; ++x)
		{
			prev_data = sprite->data[(y + offset.y) * (sprite->width / TILE_BYTE_SIZE) + x + (offset.x / TILE_BYTE_SIZE)];

			uint8_t data = sprite->data[(y + offset.y) * (sprite->width / TILE_BYTE_SIZE) + x + (offset.x / TILE_BYTE_SIZE)] >> (pos.x % TILE_BYTE_SIZE) | carry_data;

			render_buf->buf[(pos.y + y) * NUM_TILES + ((pos.x / TILE_BYTE_SIZE) + x)] &= ~data;
			render_buf->buf[(pos.y + y) * NUM_TILES + ((pos.x / TILE_BYTE_SIZE) + x)] |= (0x00 & data);

			carry_data = (prev_data << 8 - (pos.x % TILE_BYTE_SIZE));
		}
	}
}

void draw_char(struct render_buf_t *render_buf, struct u_vec2 pos, char ch, struct u_vec2 char_offset, cbt_sprite *ui_char_sheet)
{
	uint8_t n_char = ch - char_offset.x;

	u_vec2 offset = { (n_char % 8) * 8,
						((n_char / 8) + char_offset.y) * 8 };

	draw_sprite_offset(render_buf, { pos.x , pos.y }, ui_char_sheet, { 8, 8 }, offset);
}

void draw_str(struct render_buf_t *render_buf, struct u_vec2 pos, const char* str, uint8_t len, cbt_sprite *ui_char_sheet)
{
	for (uint8_t i = 0; i < len; ++i)
	{
		u_vec2 char_offset = { 0, 0 };

		switch (str[i]) {
		case '!':
			draw_sprite_offset(render_buf, { pos.x , pos.y }, ui_char_sheet, { 8, 8 }, { 32, 32 });
			break;
		case '?':
			draw_sprite_offset(render_buf, { pos.x , pos.y }, ui_char_sheet, { 8, 8 }, { 40, 32 });
			break;
		case '.':
			draw_sprite_offset(render_buf, { pos.x , pos.y }, ui_char_sheet, { 8, 8 }, { 48, 32 });
			break;
		case ',':
			draw_sprite_offset(render_buf, { pos.x , pos.y }, ui_char_sheet, { 8, 8 }, { 56, 32 });
			break;
		default:
			if (str[i] > 64 && str[i] < 91)
			{
				char_offset = { 65 - 2, 1 };
				draw_char(render_buf, pos, str[i], char_offset, ui_char_sheet);
			}
			else if (str[i] > 47 && str[i] < 58)
			{
				char_offset = { 48, 0 };
				draw_char(render_buf, pos, str[i], char_offset, ui_char_sheet);
			}
			break;
		}
		pos.x += 8;
	}
}

void draw_uint8(struct render_buf_t *render_buf, struct u_vec2 pos, uint8_t num, struct cbt_sprite *ui_char_sheet)
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
			draw_char(render_buf, pos, buf[i], { 0, 0 }, ui_char_sheet);
			pos.x += 8;
		}	
	}
}

void draw_panel(struct render_buf_t *render_buf, struct u_vec2 pos, uint8_t width, uint8_t height)
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

			data = data >> (pos.x % TILE_BYTE_SIZE) | carry_data;

			render_buf->buf[(pos.y + y) * NUM_TILES + ((pos.x / TILE_BYTE_SIZE) + x)] = 0x00 | data;

			carry_data = (prev_data << 8 - (pos.x % TILE_BYTE_SIZE));
		}
	}
}

void delay(uint32_t ms)
{
	clock_t start = clock();
	uint8_t elapsed_ms = 0;

	while (elapsed_ms < ms)
	{
		clock_t end = clock();
		elapsed_ms = (end - start);
	}
}

struct object_t *init_obj(struct obj_buf_t *obj_buf, struct u_vec2 pos, struct u_bounds_t bounds, struct cbta_sprite *sprite)
{
	struct object_t obj;
	obj.pos = pos;
	obj.bounds = bounds;
	obj.sprite = sprite;
	obj.modified = 0x00;

	obj_buf->buf[obj_buf->filled] = obj;
	obj_buf->filled++;

	return &obj_buf->buf[obj_buf->filled - 1];
}

struct phys_object_t *init_phys_obj(struct phys_obj_buf_t *phys_obj_buf, struct object_t *obj, struct vec2 vel, struct vec2 acc)
{
	struct phys_object_t phys_obj;
	phys_obj.vel = vel;
	phys_obj.acc = acc;
	phys_obj.obj = obj;
	phys_obj.collision_status = 0x00;

	phys_obj_buf->buf[phys_obj_buf->filled] = phys_obj;
	phys_obj_buf->filled++;

	return &phys_obj_buf->buf[phys_obj_buf->filled - 1];
}

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

struct scene_t init_scene(struct u_bounds_t bounds, struct cbt_sprite *sprite)
{
	struct scene_t scene;

	scene.sprite = sprite;
	scene.bounds = bounds;
	scene.modified = 0x00;

	return scene;
}

void main(void)
{
	HDC hdc = GetDC(GetConsoleWindow());

	struct render_buf_t render_buf;

	struct scene_buf_t scene_buf;

	struct phys_obj_buf_t phys_obj_buf;
	phys_obj_buf.filled = 0;

	struct obj_buf_t obj_buf;
	obj_buf.filled = 0;

	init_render_buf(&render_buf);

	struct cbt_sprite scene_sprite = open_cbt_sprite("big_texas_game_scene_troll.cbt");
	scene_t scene = init_scene({ {0, 104}, {128, 0} }, &scene_sprite);

	init_scene_buf(&scene_buf, &scene);

	struct cbta_sprite player_sprite = open_cbta_sprite("big_texas_game_player.cbta");
	u_vec2 player_pos = { 16, 72 };
	u_bounds_t player_bounds = { {player_pos.x, player_pos.y + 32 }, {player_pos.x + 32, player_pos.y} };

	struct object_t *player_obj = init_obj(&obj_buf, player_pos, player_bounds, &player_sprite);
	struct phys_object_t *player_phys_obj = init_phys_obj(&phys_obj_buf, player_obj, { 0, 0 }, { 0, 0 });

	player_t player = init_player(player_phys_obj);

	struct cbt_sprite ui_char_sheet = open_cbt_sprite("ui_text.cbt");

	while (1)
	{
		check_input(&player, &scene_buf);

		physics_tick(&phys_obj_buf, &scene);
		
		draw_scene(&render_buf, &scene_buf);

		draw_all_obj(&render_buf, &obj_buf);

		draw_panel(&render_buf, { 0, 0 }, 128, 20);

		draw_str(&render_buf, { 8, 2 }, "HEALTH", 6, &ui_char_sheet);
		draw_str(&render_buf, { 8, 10 }, "POINTS", 6, &ui_char_sheet);

		draw_uint8(&render_buf, { 64, 2 }, player.health, &ui_char_sheet);
		draw_uint8(&render_buf, { 64, 10 }, 255, &ui_char_sheet);
			
		render_tick(&render_buf, &hdc);
		
		// delay render for this many ms.
		delay(28);
	}
}
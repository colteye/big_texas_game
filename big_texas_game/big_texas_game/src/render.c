#include "render.h"

void init_render_buf(struct render_buf_t *render_buf)
{
	for (int x = 0; x < NUM_TILES; x++)
	{
		for (int y = 0; y < NUM_TILES; y++)
		{
			for (int i = 0; i < TILE_BYTE_SIZE; i++)
			{
				render_buf->buf[(y * TILE_BYTE_SIZE + i) * NUM_TILES + x] = 0x00;
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

void draw_char(struct render_buf_t *render_buf, struct u_vec2 pos, char ch, struct u_vec2 char_offset, struct cbt_sprite *ui_char_sheet)
{
	uint8_t n_char = ch - char_offset.x;

	struct u_vec2 offset = { (n_char % 8) * 8,
						((n_char / 8) + char_offset.y) * 8 };

	draw_sprite_offset(render_buf, (struct u_vec2){ pos.x , pos.y }, ui_char_sheet, (struct u_vec2) { 8, 8 }, offset);
}

void draw_str(struct render_buf_t *render_buf, struct u_vec2 pos, const char* str, uint8_t len, struct cbt_sprite *ui_char_sheet)
{
	for (uint8_t i = 0; i < len; ++i)
	{
		struct u_vec2 char_offset = { 0, 0 };

		switch (str[i]) {
		case '!':
			draw_sprite_offset(render_buf, (struct u_vec2){ pos.x , pos.y }, ui_char_sheet, (struct u_vec2){ 8, 8 }, (struct u_vec2){ 32, 32 });
			break;
		case '?':
			draw_sprite_offset(render_buf, (struct u_vec2){ pos.x , pos.y }, ui_char_sheet, (struct u_vec2){ 8, 8 }, (struct u_vec2){ 40, 32 });
			break;
		case '.':
			draw_sprite_offset(render_buf, (struct u_vec2){ pos.x , pos.y }, ui_char_sheet, (struct u_vec2){ 8, 8 }, (struct u_vec2){ 48, 32 });
			break;
		case ',':
			draw_sprite_offset(render_buf, (struct u_vec2){ pos.x , pos.y }, ui_char_sheet, (struct u_vec2){ 8, 8 }, (struct u_vec2){ 56, 32 });
			break;
		default:
			if (str[i] > 64 && str[i] < 91)
			{
				char_offset = (struct u_vec2){ 65 - 2, 1 };
				draw_char(render_buf, pos, str[i], char_offset, ui_char_sheet);
			}
			else if (str[i] > 47 && str[i] < 58)
			{
				char_offset = (struct u_vec2){ 48, 0 };
				draw_char(render_buf, pos, str[i], char_offset, ui_char_sheet);
			}
			break;
		}
		pos.x += 8;
	}
}

void draw_uint_internal(struct render_buf_t *render_buf, struct u_vec2 pos, uint32_t num, uint8_t* buf, uint8_t buf_len, struct cbt_sprite *ui_char_sheet)
{
	if (num < 10)
	{
		draw_char(render_buf, pos, num, (struct u_vec2){ 0, 0 }, ui_char_sheet);
		return;
	}

	uint8_t i = buf_len - 1;

	while (num > 0)
	{
		uint8_t dig = num % 10;
		buf[i] = dig;
		num = num / 10;
		--i;
	}

	for (i = 0; i < buf_len; ++i)
	{
		if (buf[i] < 10)
		{
			draw_char(render_buf, pos, buf[i], (struct u_vec2){ 0, 0 }, ui_char_sheet);
			pos.x += 8;
		}
	}
}

void draw_uint8(struct render_buf_t *render_buf, struct u_vec2 pos, uint8_t num, struct cbt_sprite *ui_char_sheet)
{
	uint8_t buf[3] = { 255, 255, 255 };
	draw_uint_internal(render_buf, pos, num, buf, 3, ui_char_sheet);
}

void draw_uint32(struct render_buf_t *render_buf, struct u_vec2 pos, uint32_t num, struct cbt_sprite *ui_char_sheet)
{
	uint8_t buf[10] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };
	draw_uint_internal(render_buf, pos, num, buf, 10, ui_char_sheet);
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
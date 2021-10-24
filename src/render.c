#include "render.h"
#include "error_handling.h"

// Core rendering code.

void render_buf_init(struct render_buf_t *render_buf)
{
	if (is_null(render_buf)) return;

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

void scene_buf_draw(struct render_buf_t *render_buf, struct scene_buf_t *scene_buf)
{
	if (is_null(render_buf) || is_null(scene_buf)) return;

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
void object_draw(struct render_buf_t *render_buf, struct object_t *obj)
{
	if (is_null(render_buf) || is_null(obj)) return;

	// Make sure object is visible!
	if (!(obj->vis_mod_collision_mask & VISIBLE)) return;

	//printf("pos X: %d\n", obj->pos.x);
	cbta_sprite_draw(render_buf, obj->pos, obj->sprite, (struct u_vec2) { 0, 0 }, 0, obj->y_flip, (struct vec2) { 0, 0 });
}

void obj_buf_draw(struct render_buf_t *render_buf, struct obj_buf_t *obj_buf)
{
	if (is_null(render_buf) || is_null(obj_buf)) return;

	//object_draw(render_buf, &obj_buf->buf[0]);

	for (uint8_t i = 0; i < obj_buf->filled; ++i)
	{
		object_draw(render_buf, &obj_buf->buf[i]);
	}
}

void cbta_sprite_draw(struct render_buf_t *render_buf, struct u_vec2 pos, struct cbta_sprite *sprite, struct u_vec2 size, uint8_t use_size, uint8_t y_flip, struct vec2 offset)
{
	uint8_t h = 0;
	uint8_t w = 0;

	if (use_size) h = size.y;
	else h = sprite->height;

	if (use_size) w = size.x;
	else w = sprite->width;

	for (int y = 0; y < h; ++y)
	{
		uint8_t prev_data = 0x00;
		uint8_t prev_mask = 0x00;
		uint8_t carry_data = 0x00;
		uint8_t carry_mask = 0x00;
		uint8_t data = 0x00;
		uint8_t mask = 0x00;

		int16_t render_buf_index = 0x00;

		for (int x = 0; x < w / TILE_BYTE_SIZE; ++x)
		{
			if (y_flip)
			{
				prev_data = MSB_2_LSB(sprite->data[((y + offset.y) % sprite->height) * (sprite->width / TILE_BYTE_SIZE) + (sprite->width / TILE_BYTE_SIZE - x - 1) + (offset.x / TILE_BYTE_SIZE)]);
				prev_mask = MSB_2_LSB(sprite->mask[((y + offset.y) % sprite->height) * (sprite->width / TILE_BYTE_SIZE) + (sprite->width / TILE_BYTE_SIZE - x - 1) + (offset.x / TILE_BYTE_SIZE)]);
			}
			else
			{
				prev_data = sprite->data[((y + offset.y) % sprite->height) * (sprite->width / TILE_BYTE_SIZE) + x + (offset.x / TILE_BYTE_SIZE)];
				prev_mask = sprite->mask[((y + offset.y) % sprite->height) * (sprite->width / TILE_BYTE_SIZE) + x + (offset.x / TILE_BYTE_SIZE)];
			}
			
			data = prev_data >> (pos.x % TILE_BYTE_SIZE) | carry_data;
			mask = prev_mask >> (pos.x % TILE_BYTE_SIZE) | carry_mask;

			carry_data = prev_data << (8 - (pos.x % TILE_BYTE_SIZE));
			carry_mask = prev_mask << (8 - (pos.x % TILE_BYTE_SIZE));

			//printf("rbufidx: %d\n", render_buf_index);
			if (!IN_X_RANGE(pos.x + x * TILE_BYTE_SIZE)) {
				++x; continue;
			}

			render_buf_index = (pos.y + y - HEIGHT_OFFSET) * NUM_TILES + (((pos.x - WIDTH_OFFSET) / TILE_BYTE_SIZE) + x);

			render_buf->buf[render_buf_index] &= ~mask;
			render_buf->buf[render_buf_index] |= (data & mask);
		}

		// Make sure to add a the last bit if the sprite goes over!
		data = 0x00 | carry_data;
		mask = 0x00 | carry_mask;

		//if (!IN_X_RANGE(obj->pos.x + obj->sprite->width))  continue;

		render_buf_index = (pos.y + y - HEIGHT_OFFSET) * NUM_TILES + (((pos.x - WIDTH_OFFSET) / TILE_BYTE_SIZE) + w / TILE_BYTE_SIZE);

		render_buf->buf[render_buf_index] &= ~mask;
		render_buf->buf[render_buf_index] |= (data & mask);
	}
}

void cbt_sprite_draw(struct render_buf_t *render_buf, struct u_vec2 pos, struct cbt_sprite *sprite, struct u_vec2 size, uint8_t use_size, uint8_t y_flip, struct vec2 offset)
{
	uint8_t h = 0;
	uint8_t w = 0;

	if (use_size) h = size.y;
	else h = sprite->height;

	if (use_size) w = size.x;
	else w = sprite->width;

	for (int y = 0; y < h; ++y)
	{
		uint8_t prev_data = 0x00;
		uint8_t carry_data = 0x00;
		uint8_t data = 0x00;

		int16_t render_buf_index = 0x00;

		for (int x = 0; x < w / TILE_BYTE_SIZE; ++x)
		{

			if (y_flip)
			{
				prev_data = MSB_2_LSB(sprite->data[((y + offset.y) % sprite->height) * (sprite->width / TILE_BYTE_SIZE) + (sprite->width / TILE_BYTE_SIZE - x - 1) + (offset.x / TILE_BYTE_SIZE)]);
			}
			else
			{
				prev_data = sprite->data[((y + offset.y) % sprite->height) * (sprite->width / TILE_BYTE_SIZE) + x + (offset.x / TILE_BYTE_SIZE)];
			}

			data = prev_data >> (pos.x % TILE_BYTE_SIZE) | carry_data;

			carry_data = prev_data << (8 - (pos.x % TILE_BYTE_SIZE));

			//printf("rbufidx: %d\n", render_buf_index);
			if (!IN_X_RANGE(pos.x + x * TILE_BYTE_SIZE)) {
				++x; continue;
			}

			render_buf_index = (pos.y + y - HEIGHT_OFFSET) * NUM_TILES + (((pos.x - WIDTH_OFFSET) / TILE_BYTE_SIZE) + x);

			render_buf->buf[render_buf_index] &= ~data;
			render_buf->buf[render_buf_index] |= (data & 0x00);
		}

		// Make sure to add a the last bit if the sprite goes over!
		data = 0x00 | carry_data;

		render_buf_index = (pos.y + y - HEIGHT_OFFSET) * NUM_TILES + (((pos.x - WIDTH_OFFSET) / TILE_BYTE_SIZE) + w / TILE_BYTE_SIZE);

		render_buf->buf[render_buf_index] &= ~data;
		render_buf->buf[render_buf_index] |= (data & 0x00);
	}
}

void char_draw(struct render_buf_t *render_buf, struct u_vec2 pos, char ch, struct vec2 char_offset, struct cbt_sprite *ui_char_sheet)
{
	if (is_null(render_buf) || is_null(ui_char_sheet)) return;

	uint8_t n_char = ch - char_offset.x;

	struct vec2 offset = { (n_char % 8) * 8,
						((n_char / 8) + char_offset.y) * 8 };

	cbt_sprite_draw(render_buf, (struct u_vec2) { pos.x, pos.y }, ui_char_sheet, (struct u_vec2) { 8, 8 }, 1, 0, offset);
}

void string_draw(struct render_buf_t *render_buf, struct u_vec2 pos, const char* str, uint8_t len, struct cbt_sprite *ui_char_sheet)
{
	if (is_null(render_buf) || is_null((char *)str) || is_null(ui_char_sheet)) return;

	for (uint8_t i = 0; i < len; ++i)
	{
		struct vec2 char_offset = { 0, 0 };

		switch (str[i]) {
		case '!':
			cbt_sprite_draw(render_buf, pos, ui_char_sheet, (struct u_vec2) { 8, 8 }, 1, 0, (struct vec2) { 32, 32 });
			break;
		case '?':
			cbt_sprite_draw(render_buf, pos, ui_char_sheet, (struct u_vec2) { 8, 8 }, 1, 0, (struct vec2) { 40, 32 });
			break;
		case '.':
			cbt_sprite_draw(render_buf, pos, ui_char_sheet, (struct u_vec2) { 8, 8 }, 1, 0, (struct vec2) { 48, 32 });
			break;
		case ',':
			cbt_sprite_draw(render_buf, pos, ui_char_sheet, (struct u_vec2) { 8, 8 }, 1, 0, (struct vec2) { 56, 32 });
			break;
		default:
			if (str[i] > 64 && str[i] < 91)
			{
				char_offset = (struct vec2) { 65 - 2, 1 };
				char_draw(render_buf, pos, str[i], char_offset, ui_char_sheet);
			}
			else if (str[i] > 47 && str[i] < 58)
			{
				char_offset = (struct vec2) { 48, 0 };
				char_draw(render_buf, pos, str[i], char_offset, ui_char_sheet);
			}
			break;
		}
		pos.x += 8;
	}
}

void uint_internal_draw(struct render_buf_t *render_buf, struct u_vec2 pos, uint32_t num, uint8_t* buf, uint8_t buf_len, struct cbt_sprite *ui_char_sheet)
{
	if (is_null(render_buf) || is_null(buf) || is_null(ui_char_sheet)) return;

	if (num < 10)
	{
		char_draw(render_buf, pos, num, (struct vec2) { 0, 0 }, ui_char_sheet);
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
			char_draw(render_buf, pos, buf[i], (struct vec2) { 0, 0 }, ui_char_sheet);
			pos.x += 8;
		}
	}
}

void uint8_draw(struct render_buf_t *render_buf, struct u_vec2 pos, uint8_t num, struct cbt_sprite *ui_char_sheet)
{
	if (is_null(render_buf) || is_null(ui_char_sheet)) return;

	uint8_t buf[3] = { 255, 255, 255 };
	uint_internal_draw(render_buf, pos, num, buf, 3, ui_char_sheet);
}

void uint32_draw(struct render_buf_t *render_buf, struct u_vec2 pos, uint32_t num, struct cbt_sprite *ui_char_sheet)
{
	if (is_null(render_buf) || is_null(ui_char_sheet)) return;

	uint8_t buf[10] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };
	uint_internal_draw(render_buf, pos, num, buf, 10, ui_char_sheet);
}

void panel_draw(struct render_buf_t *render_buf, struct u_vec2 pos, uint8_t width, uint8_t height)
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

/*void panel_draw(struct render_buf_t *render_buf, struct u_vec2 pos, uint8_t width, uint8_t height)
{
	if (is_null(render_buf)) return;

	uint8_t top_bottom[3] = { 0b00111111, 0b11111111, 0b11111100 };
	uint8_t middle[3] = { 0b01111111, 0b11111111, 0b11111110 };

	for (int y = 0; y < height; ++y)
	{
		uint8_t data = 0x00;
		uint8_t prev_data = 0x00;
		uint8_t carry_data = 0x00;

		uint8_t mask = 0x00;
		uint8_t prev_mask = 0x00;
		uint8_t carry_mask = 0x00;

		for (uint8_t x = 0; x < width / TILE_BYTE_SIZE; ++x)
		{
			if (y > 0 && y < height - 1)
			{
				if (y == 1 || y == height - 2)
				{
					if (x == 0) prev_data = top_bottom[0];
					else if (x == width / TILE_BYTE_SIZE - 1) prev_data = top_bottom[2];
					else  prev_data = top_bottom[1];
				}
				else
				{
					if (x == 0) prev_data = middle[0];
					else if (x == width / TILE_BYTE_SIZE - 1) prev_data = middle[2];
					else  prev_data = middle[1];
				}
			}

			// Set up mask.
			prev_mask = 0b11111111;

			data = prev_data >> (pos.x % TILE_BYTE_SIZE) | carry_data;
			mask = prev_mask >> (pos.x % TILE_BYTE_SIZE) | carry_mask;

			carry_data = prev_data << (8 - (pos.x % TILE_BYTE_SIZE));
			carry_mask = prev_mask << (8 - (pos.x % TILE_BYTE_SIZE));

			if (!IN_Y_RANGE(pos.y + y * TILE_BYTE_SIZE) || !IN_X_RANGE(pos.x + x * TILE_BYTE_SIZE)) continue;

			uint8_t new_x = x;
			if (pos.x < WIDTH_OFFSET)
				new_x = x - 1;

			render_buf->buf[(pos.y + y - HEIGHT_OFFSET) * NUM_TILES + (((pos.x - WIDTH_OFFSET) / TILE_BYTE_SIZE) + (new_x))] &= ~mask;
			render_buf->buf[(pos.y + y - HEIGHT_OFFSET) * NUM_TILES + (((pos.x - WIDTH_OFFSET) / TILE_BYTE_SIZE) + (new_x))] |= (data & mask);
		}

		if (pos.x < WIDTH_OFFSET)
		{
			data = 0x00 | carry_data;
			mask = 0x00 | carry_mask;
			render_buf->buf[(pos.y + y - HEIGHT_OFFSET) * NUM_TILES + (((pos.x - WIDTH_OFFSET) / TILE_BYTE_SIZE) + (width / TILE_BYTE_SIZE - 1))] &= ~mask;
			render_buf->buf[(pos.y + y - HEIGHT_OFFSET) * NUM_TILES + (((pos.x - WIDTH_OFFSET) / TILE_BYTE_SIZE) + (width / TILE_BYTE_SIZE - 1))] |= (data & mask);
		}
		
	}
}*/
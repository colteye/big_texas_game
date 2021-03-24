#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

struct cbt
{
	uint8_t width;
	uint8_t height;
	uint32_t data_size;
	uint8_t *data;
};

struct cbta
{
	uint8_t width;
	uint8_t height;
	uint32_t data_size;
	uint8_t *data;
	uint8_t *mask;
};


uint8_t reverseBits(uint8_t num)
{
	unsigned int count = sizeof(num) * 8 - 1;
	uint8_t reverse_num = num;

	num >>= 1;
	while (num)
	{
		reverse_num <<= 1;
		reverse_num |= num & 1;
		num >>= 1;
		count--;
	}
	reverse_num <<= count;
	return reverse_num;
}

uint8_t *invert_mirror_y(uint8_t *input, size_t size, uint8_t w, uint8_t h)
{
	uint8_t *reversed = malloc(size);
	uint8_t w_f = w / (sizeof(uint8_t) * 8);

	for (int x = 0; x < w_f; ++x)
	{
		for (int y = 0; y < h; ++y)
		{
			reversed[y * w_f + x] = ~input[(h - y) * w_f + x];
		}
	}
	return reversed;
}

struct cbt bmp_to_cbt(char *fname)
{
	uint8_t head[54];
	FILE *f = fopen(fname, "rb");

	printf("file: %d", f);

	// BMP header is 54 bytes
	fread(head, 1, 54, f);

	uint32_t w = head[18] + (((uint32_t)head[19]) << 8) + (((uint32_t)head[20]) << 16) + (((uint32_t)head[21]) << 24);
	uint32_t h = head[22] + (((uint32_t)head[23]) << 8) + (((uint32_t)head[24]) << 16) + (((uint32_t)head[25]) << 24);

	// lines are aligned on 4-byte boundary
	uint32_t lineSize = (w / 8 + (w / 8) % 4);
	uint32_t fileSize = lineSize * h;

	uint8_t *data = malloc(fileSize);


	// skip the header
	fseek(f, 54, SEEK_SET);

	// skip palette - two rgb quads, 8 bytes
	fseek(f, 8, SEEK_CUR);

	// read data
	fread(data, 1, fileSize, f);

	uint8_t *data_flip = invert_mirror_y(data, fileSize, w, h);

	// Reverse endianness MAY BE NECESSARY!!!!.
	/*for (int i = 0; i < fileSize; ++i)
	{
		data_flip[i] = reverseBits(data_flip[i]);
	}*/

	struct cbt output = { w, h, fileSize, data_flip };

	return output;
}	

struct cbta bmp_to_cbta(char *fname, char *fname_a)
{
	struct cbt color = bmp_to_cbt(fname);
	struct cbt mask = bmp_to_cbt(fname_a);

	struct cbta output = { color.width, color.height, color.data_size, color.data, mask.data };

	return output;
}

void main()
{
	struct cbt out_img = bmp_to_cbt("big_texas_game_scene.bmp");

	// Convert to char array for fprintf.
	FILE *fp = fopen("big_texas_game_scene.cbt", "wb");

	fwrite(&out_img.width, sizeof(uint8_t), 1, fp);
	fwrite(&out_img.height, sizeof(uint8_t), 1, fp);
	fwrite(out_img.data, sizeof(uint8_t), out_img.data_size, fp);
	fclose(fp);

	out_img = bmp_to_cbt("big_texas_game_scene_2.bmp");

	// Convert to char array for fprintf.
	fp = fopen("big_texas_game_scene_2.cbt", "wb");

	fwrite(&out_img.width, sizeof(uint8_t), 1, fp);
	fwrite(&out_img.height, sizeof(uint8_t), 1, fp);
	fwrite(out_img.data, sizeof(uint8_t), out_img.data_size, fp);
	fclose(fp);

	out_img = bmp_to_cbt("big_texas_game_scene_troll.bmp");

	// Convert to char array for fprintf.
	fp = fopen("big_texas_game_scene_troll.cbt", "wb");

	fwrite(&out_img.width, sizeof(uint8_t), 1, fp);
	fwrite(&out_img.height, sizeof(uint8_t), 1, fp);
	fwrite(out_img.data, sizeof(uint8_t), out_img.data_size, fp);
	fclose(fp);

	struct cbta out_img_a = bmp_to_cbta("big_texas_game_player.bmp", "big_texas_game_player_alpha.bmp");

	// Convert to char array for fprintf.
	fp = fopen("big_texas_game_player.cbta", "wb");

	fwrite(&out_img_a.width, sizeof(uint8_t), 1, fp);
	fwrite(&out_img_a.height, sizeof(uint8_t), 1, fp);
	fwrite(out_img_a.data, sizeof(uint8_t), out_img_a.data_size, fp);
	fwrite(out_img_a.mask, sizeof(uint8_t), out_img_a.data_size, fp);
	fclose(fp);
}
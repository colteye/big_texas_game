#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include "vec.h"
#include "scene.h"
#include "define.h"
#include "sprite.h"
#include "object.h"

struct render_buf_t
{
	uint8_t buf[NUM_TILES * NUM_TILES * TILE_BYTE_SIZE];
};

void render_buf_init(struct render_buf_t *render_buf);

void scene_buf_draw(struct render_buf_t *render_buf, struct scene_buf_t *scene_buf);

void object_draw(struct render_buf_t *render_buf, struct object_t *obj);

void obj_buf_draw(struct render_buf_t *render_buf, struct obj_buf_t *obj_buf);

void cbt_sprite_draw(struct render_buf_t *render_buf, struct u_vec2 pos, struct cbt_sprite *sprite, struct u_vec2 size, struct vec2 offset);

void char_draw(struct render_buf_t *render_buf, struct u_vec2 pos, char ch, struct vec2 char_offset, struct cbt_sprite *ui_char_sheet);

void string_draw(struct render_buf_t *render_buf, struct u_vec2 pos, const char* str, uint8_t len, struct cbt_sprite *ui_char_sheet);

void uint8_draw(struct render_buf_t *render_buf, struct u_vec2 pos, uint8_t num, struct cbt_sprite *ui_char_sheet);

void uint32_draw(struct render_buf_t *render_buf, struct u_vec2 pos, uint32_t num, struct cbt_sprite *ui_char_sheet);

void panel_draw(struct render_buf_t *render_buf, struct u_vec2 pos, uint8_t width, uint8_t height);

#endif /* RENDER_H */
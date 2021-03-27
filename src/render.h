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

void init_render_buf(struct render_buf_t *render_buf);

void draw_scene(struct render_buf_t *render_buf, struct scene_buf_t *scene_buf);

void draw_obj(struct render_buf_t *render_buf, struct object_t *obj);

void draw_all_obj(struct render_buf_t *render_buf, struct obj_buf_t *obj_buf);

void draw_sprite_offset(struct render_buf_t *render_buf, struct u_vec2 pos, struct cbt_sprite *sprite, struct u_vec2 size, struct u_vec2 offset);

void draw_char(struct render_buf_t *render_buf, struct u_vec2 pos, char ch, struct u_vec2 char_offset, struct cbt_sprite *ui_char_sheet);

void draw_str(struct render_buf_t *render_buf, struct u_vec2 pos, const char* str, uint8_t len, struct cbt_sprite *ui_char_sheet);

void draw_uint_internal(struct render_buf_t *render_buf, struct u_vec2 pos, uint32_t num, uint8_t* buf, uint8_t buf_len, struct cbt_sprite *ui_char_sheet);

void draw_uint8(struct render_buf_t *render_buf, struct u_vec2 pos, uint8_t num, struct cbt_sprite *ui_char_sheet);

void draw_uint32(struct render_buf_t *render_buf, struct u_vec2 pos, uint32_t num, struct cbt_sprite *ui_char_sheet);

void draw_panel(struct render_buf_t *render_buf, struct u_vec2 pos, uint8_t width, uint8_t height);

#endif /* RENDER_H */
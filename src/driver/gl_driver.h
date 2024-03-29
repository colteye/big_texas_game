#ifndef GL_DRIVER_H
#define GL_DRIVER_H

#include "../render.h"
#include "../input.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

uint8_t gl_init();

void gl_render(struct render_buf_t *render_buf);

void gl_check_keys(struct key_status_t *key_s);

uint8_t gl_is_open();

void gl_delay(uint32_t ms);

#endif /* GL_DRIVER_H */
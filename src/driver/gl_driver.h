#ifndef GL_DRIVER_H
#define GL_DRIVER_H

#include "../render.h"
#include "../input.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

uint8_t GL_init();

void GL_render(struct render_buf_t *render_buf);

struct key_status_t GL_check_keys();

uint8_t GL_is_open();

#endif /* GL_DRIVER_H */
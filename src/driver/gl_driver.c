#include "gl_driver.h"
#include <stdio.h>
#include <time.h>
#include "../define.h"

// Global variables just to make life easier.
GLuint shader;
GLuint v_buf, uv_buf;
GLuint r_tex;
GLuint sampler;
GLFWwindow *window;

GLuint gl_load_shader() {

	// Create the shaders
	GLuint vert_shader_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	const char * vert_shader_code = 
	"#version 330 core\n\
	layout(location = 0)\
	in vec3 vertexPosition_modelspace;layout(location = 1)\
	in vec2 vertexUV;out vec2 UV;\
	void main(){  gl_Position.xyz = vertexPosition_modelspace;gl_Position.w = 1.0;UV = vertexUV;}";
	
	const char * frag_shader_code = 
	"#version 330 core\n\
	in vec2 UV;out vec3 color;\
	uniform sampler2D myTextureSampler;\
	void main() {color = texture( myTextureSampler, UV ).rgb;}";

	glShaderSource(vert_shader_id, 1, &vert_shader_code, NULL);
	glCompileShader(vert_shader_id);

	glShaderSource(frag_shader_id, 1, &frag_shader_code, NULL);
	glCompileShader(frag_shader_id);

	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vert_shader_id);
	glAttachShader(program_id, frag_shader_id);
	glLinkProgram(program_id);

	glDetachShader(program_id, vert_shader_id);
	glDetachShader(program_id, frag_shader_id);

	glDeleteShader(vert_shader_id);
	glDeleteShader(frag_shader_id);

	return program_id;
}

uint8_t gl_init()
{
	// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

	if (!glfwInit())
	{
		return 0;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(768, 768, "Big Texas", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return 0;
	}

	// Make the window's context current.
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Open GLAD loader.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return 0;
	}
	
	glGenTextures(1, &r_tex);
	glBindTexture(GL_TEXTURE_2D, r_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint vert_arr;
	glGenVertexArrays(1, &vert_arr);
	glBindVertexArray(vert_arr);

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	static const GLfloat g_uv_buffer_data[] = {
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f,  0.0f,
		0.0f,  0.0f,
		1.0f,  1.0f,
		0.0f,  1.0f,
	};

	glGenBuffers(1, &v_buf);
	glBindBuffer(GL_ARRAY_BUFFER, v_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &uv_buf);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	shader = gl_load_shader();

	sampler = glGetUniformLocation(shader, "myTextureSampler");

	return 1;
}

void gl_render(struct render_buf_t *render_buf)
{
	GLubyte image[HEIGHT][WIDTH][3];

	// OpenGL doesn't like 1 bit buffers, need to convert into 24 bit.
	for (int x = 0; x < NUM_TILES; ++x)
	{
		for (int y = 0; y < HEIGHT; ++y)
		{
			for (int j = 0; j < BIT_PER_BYTE; ++j)
			{
				uint8_t current_color = N_BIT(render_buf->buf[y * NUM_TILES + x], 7 - j) * 255;
				image[x * TILE_BYTE_SIZE + j][y][0] = current_color;
				image[x * TILE_BYTE_SIZE + j][y][1] = current_color;
				image[x * TILE_BYTE_SIZE + j][y][2] = current_color;
			}
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, r_tex);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, &image[0]);
	glUniform1i(sampler, 0);


	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, v_buf);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buf);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

uint8_t gl_is_key_down(uint32_t keyCode)
{
	return (glfwGetKey(window, keyCode) == GLFW_PRESS);
};

struct key_status_t gl_check_keys()
{
	struct key_status_t key_s;
	key_s.left_key_s = gl_is_key_down(GLFW_KEY_A);
	key_s.right_key_s = gl_is_key_down(GLFW_KEY_D);
	key_s.jump_key_s = gl_is_key_down(GLFW_KEY_SPACE);

	return key_s;
}

uint8_t gl_is_open()
{
	return (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
}

void gl_delay(uint32_t ms)
{
	clock_t start = clock();
	uint32_t elapsed_ms = 0;

	while (elapsed_ms < ms)
	{
		clock_t end = clock();
		elapsed_ms = (end - start);
	}
}

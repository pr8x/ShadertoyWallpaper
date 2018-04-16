#include "module.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

static const GLchar* module_vertex_source = R"(
#version 130
in vec3 vertex;
in vec2 uv;
out vec2 _stw_uv;

uniform vec2 iResolution;

void main() {
	_stw_uv = uv * iResolution;
	gl_Position = vec4(vertex, 1);
}
)";

static const GLchar* module_frag_source = R"(
#version 130
in vec2 _stw_uv;
out vec4 _stw_color;

uniform float iTime;
uniform vec2 iResolution;
)";

static const GLchar* module_frag_body_source = R"(
void main() {
	vec4 _stw_out;
	mainImage(_stw_out, _stw_uv);
	_stw_color = _stw_out;
}
)";

static const GLchar* screen_vertex_source = R"(
#version 130
in vec3 vertex;
in vec2 uv;
out vec2 fragUV;

void main() {
	fragUV = uv;
	gl_Position = vec4(vertex, 1);
}
)";

static const GLchar* screen_frag_source = R"(
#version 130
in vec2 fragUV;
out vec4 color;

uniform sampler2D rt;

void main() {
	color = texture(rt, fragUV);
}
)";


stw::Module::Module(const Resolution& res, const std::string& file) : _res(res) {

	std::ifstream ifs(file);
	std::stringstream buffer;
	buffer << ifs.rdbuf();

	std::string fragSource;
	fragSource += module_frag_source;
	fragSource += buffer.str();
	fragSource += module_frag_body_source;
	
	_module_prog = load_program(module_vertex_source, fragSource.c_str());
	_screen_prog = load_program(screen_vertex_source, screen_frag_source);

	init_vbo();
	init_fbo();

	std::cout << "module " << file << " loaded" <<  std::endl;
}

stw::Module::~Module() {
	glDeleteProgram(_module_prog);
}

void stw::Module::render(const Uniform& data) const {

	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, _res.x, _res.y);
	{
		glUseProgram(_module_prog);
		GLint tsLoc = glGetUniformLocation(_module_prog, "iTime");
		glUniform1f(tsLoc, data.timestamp);
		GLint rsLoc = glGetUniformLocation(_module_prog, "iResolution");
		glUniform2f(rsLoc, _res.x, _res.y);

		glBindVertexArray(_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(_screen_prog);
	glBindTexture(GL_TEXTURE_2D, _rt);
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

GLuint stw::Module::load_program(const char* vertex, const char* fragment) {
	auto prog = glCreateProgram();
	auto vs = glCreateShader(GL_VERTEX_SHADER);
	auto fs = glCreateShader(GL_FRAGMENT_SHADER);
	GLint success;
	char infoLog[512];

	glShaderSource(vs, 1, &vertex, NULL);
	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vs, sizeof(infoLog), NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	glShaderSource(fs, 1, &fragment, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fs, sizeof(infoLog), NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	glAttachShader(prog, vs);
	glAttachShader(prog, fs);
	glLinkProgram(prog);

	glGetProgramiv(_module_prog, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(prog, sizeof(infoLog), NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
	return prog;
}

void stw::Module::init_vbo() {
	GLfloat vertices[] = {
		// positions         // texture coords
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
	};

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
}

void stw::Module::init_fbo() {
	
	glGenTextures(1, &_rt);
	glBindTexture(GL_TEXTURE_2D, _rt);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _res.x, _res.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _rt, 0);

	GLenum db[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, db);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("failed to create FBO");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

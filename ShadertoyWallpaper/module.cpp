#include "module.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

static const GLchar* kVertexSource = R"(
varying out vec2 fragUV;
uniform vec2 iResolution;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	fragUV = gl_MultiTexCoord0.xy * iResolution;
}
)";

static const GLchar* kFragmentSource_Header = R"(
varying in vec2 fragUV;
varying out vec4 outColor;

uniform float iTime;
uniform vec2 iResolution;
)";

static const GLchar* kFragmentSource_Body = R"(
void main()
{
	vec4 _stout;
	mainImage(_stout, fragUV);
	outColor = _stout;
}
)";

stw::Module::Module(const std::string& file) {
	_handle = glCreateProgram();
	_vs = glCreateShader(GL_VERTEX_SHADER);
	_fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(_vs, 1, &kVertexSource, NULL);
	glCompileShader(_vs);

	std::ifstream stream(file);
	std::stringstream buffer;
	buffer << stream.rdbuf();
	auto bufferStr = buffer.str();

	const GLchar* frag_parts[] = { kFragmentSource_Header,  bufferStr.c_str(),  kFragmentSource_Body };
	glShaderSource(_fs, 3, frag_parts, NULL);
	glCompileShader(_fs);

	glAttachShader(_handle, _vs);
	glAttachShader(_handle, _fs);
	glLinkProgram(_handle);

	GLint success;
	char infoLog[512];
	glGetProgramiv(_handle, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(_handle, sizeof(infoLog), NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	std::cout << "Module (" << file << ") loaded." << std::endl;
}

stw::Module::~Module() {
	glDetachShader(_handle, _vs);
	glDetachShader(_handle, _fs);
	glDeleteShader(_vs);
	glDeleteShader(_fs);
	glDeleteProgram(_handle);
}

void stw::Module::load_sampler(const Sampler& sampler) {
	
}

void stw::Module::render(const Uniform& data) const {
	glUseProgram(_handle);

	GLint tsLoc = glGetUniformLocation(_handle, "iTime");
	glUniform1f(tsLoc, data.timestamp);
	
	GLint rsLoc = glGetUniformLocation(_handle, "iResolution");
	glUniform2f(rsLoc, data.vpx, data.vpy);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glOrtho(0, 1, 0, 1, 0.1f, 2);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0, 0, -1.0);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(0, 0, 0);

	glTexCoord2f(1, 0);
	glVertex3f(1, 0, 0);

	glTexCoord2f(1, 1);
	glVertex3f(1, 1, 0);

	glTexCoord2f(0, 1);
	glVertex3f(0, 1, 0);

	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

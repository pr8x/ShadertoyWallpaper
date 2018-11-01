#pragma once
#include <glew.h>
#include <string>

namespace stw
{
	struct Resolution {
		int x, y;
	};

	struct Uniform {
		float timestamp;
	};

	class Module {
	public:
		Module(const Resolution& res, const std::string& file);
		~Module();
		
		void render(const Uniform& data) const;

	private:
		GLuint load_program(const GLchar* vertex, const GLchar* fragment);

		void init_vbo();
		void init_fbo();

		GLuint _module_prog;
		GLuint _screen_prog;

		GLuint _vbo;
		GLuint _vao;

		GLuint _fbo;
		GLuint _rt;

		Resolution _res;
	};
}
#pragma once
#include <glew.h>
#include <string>

namespace stw
{
	struct Resolution {
		int x, y;
	};

	class Module {
	public:
		struct Uniform {
			float timestamp;
			//TODO: mouse coords
		};

		Module(const Resolution& res, const std::string& file);
		~Module();
		
		void render(const Uniform& data) const;

	private:
		GLuint load_program(const char* vertex, const char* fragment);

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
#pragma once
#include <glew.h>
#include <string>
#include "sampler.h"

namespace stw
{
	class Module {
	public:
		struct Uniform {
			float timestamp;
			float vpx, vpy;
		};

		Module(const std::string& file);
		~Module();
		
		void load_sampler(const Sampler& sampler);
		void render(const Uniform& data) const;

	private:
		GLuint _handle;
		GLuint _fs;
		GLuint _vs;
	};
}
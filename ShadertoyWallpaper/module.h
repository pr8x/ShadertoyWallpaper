#pragma once
#include <glew.h>
#include <string>

#include "api.h"

namespace stw
{
	class Module {
	public:
		struct Uniform {
			float timestamp;
			float vpx, vpy;
		};

		Module(const Api& api, const std::string& shaderId);
		~Module();
		
		void render(const Uniform& data) const;

	private:
		GLuint _handle;
		GLuint _fs;
		GLuint _vs;
	};
}
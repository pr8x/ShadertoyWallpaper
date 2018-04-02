#pragma once
#include <string>
#include <glew.h>

namespace stw
{
	class Sampler {
	public:
		Sampler(const std::string& file);
		~Sampler();

	private:
		GLuint _texture;
	};
}
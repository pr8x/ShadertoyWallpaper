#include "sampler.h"

stw::Sampler::Sampler(const std::string& file) {
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	//TODO

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

stw::Sampler::~Sampler() {
}

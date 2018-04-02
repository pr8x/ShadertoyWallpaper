#include "api.h"

stw::Api::Api(const std::string& token) : _token(token) {

}

stw::Api::~Api() {
}

stw::Api::Shader stw::Api::load_shader(const std::string & id) const {

	//https://www.shadertoy.com/api/v1/shaders/shaderID?key=appkey

	std::string ss(kBaseUrl);
	ss += id + "?key=" + _token;

	//MAKE REQUEST AND GET BUFFER

	
	return {};
}
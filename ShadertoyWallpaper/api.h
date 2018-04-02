#pragma once
#include <string>
#include <vector>

#include <sstream>

namespace stw
{
	class Api {
	public:
		Api(const std::string& token);
		~Api();

		static constexpr auto kBaseUrl = "https://www.shadertoy.com/api/v1/shaders/";

		struct Shader {
			std::string author;
			std::string buffer;

			std::vector<std::string> textures;
		};
		Shader load_shader(const std::string& id) const;

	private:
		std::string _token;

	};
}
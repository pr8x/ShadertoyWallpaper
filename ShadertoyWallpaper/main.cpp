#include "graphics.h"

#include <iostream>

int main(int argc, const char* argv[]) {

	if (argc == 1) {
		std::cout << "Usage: stw.exe [Shadertoy ID]" << std::endl;
		return 0;
	}

	stw::Graphics graphics;
	stw::Api stapi("Nt8tW4");

	stw::Module module(stapi, argv[1]);

	graphics.run(module);

	return 0;
}
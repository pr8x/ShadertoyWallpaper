#include "graphics.h"

#include <iostream>

int main(int argc, const char* argv[]) {

	if (argc != 6) {
		std::cout << "Usage: stw.exe -rx [1920] -ry [1080] [file]" << std::endl;
		return 0;
	}

	try {
		stw::Graphics graphics;

		int x = atoi(argv[2]);
		int y = atoi(argv[4]);
		stw::Module module({x, y}, argv[5]);

		graphics.run(module);
	}
	catch (const std::exception& e) {
		std::cout << "error: " << e.what() << std::endl;
	}

	return 0;
}
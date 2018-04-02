#include "graphics.h"

#include <iostream>
int main() {

	stw::Graphics graphics;
	stw::Module module("C:\\Users\\luis9\\Desktop\\test.frag");

	graphics.run(module);

	return 0;
}
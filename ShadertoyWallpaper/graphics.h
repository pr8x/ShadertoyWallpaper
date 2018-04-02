#pragma once
#include <Windows.h>
#include <glew.h>
#include <wglew.h>

#include <chrono>
#include "module.h"

namespace stw
{	class Graphics {
	public:
		Graphics();
		~Graphics();

		void run(const Module& module);

	private:
		void init_opengl();
		//void blit();

		std::chrono::high_resolution_clock::time_point _timestamp;

		HWND _worker;
		HWND _progman;
		HDC _context;

		static BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);
	};
}
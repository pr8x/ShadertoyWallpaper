#pragma once
#include <Windows.h>
#include <Dwmapi.h>

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
		bool detect_fs_window();

		//void blit();

		std::chrono::high_resolution_clock::time_point _timestamp;
		std::chrono::high_resolution_clock::time_point _lastFrame;

		HWND _worker;
		HWND _progman;
		HDC _context;
		HGLRC _glContext;

		static BOOL CALLBACK FindShellDefView(HWND, LPARAM);
		static BOOL CALLBACK DetectFSWindow(HWND, LPARAM);
	};
}
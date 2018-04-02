#include "graphics.h"

#include <stdexcept>
#include <iostream>

stw::Graphics::Graphics() {
	_progman = FindWindow("Progman", NULL);
	if (_progman == NULL)
		throw std::runtime_error("Couln't find progman window");

	SendMessageTimeout(_progman, 0x052C, NULL, NULL, SMTO_NORMAL, 1000, NULL);

	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));
	if (_worker == NULL)
		throw std::runtime_error("Failed to spawn worker");

	_context = GetDCEx(_worker, NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE); //0x403
	if (_context == NULL)
		throw std::runtime_error("Device context is null");

	init_opengl();
}

stw::Graphics::~Graphics() {
	ReleaseDC(_worker, _context);
}

void stw::Graphics::run(const Module& module) {
	_timestamp = std::chrono::high_resolution_clock::now();

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	for (;;) {
		glClear(GL_COLOR_BUFFER_BIT);

		auto current = std::chrono::high_resolution_clock::now();
		auto span = current - _timestamp;
		auto fsec = std::chrono::duration_cast<std::chrono::duration<float>>(span);

		Module::Uniform data{
			fsec.count(), //timestamp,
			vp[2], vp[3] //viewport
		};
		module.render(data);

		if (!SwapBuffers(_context))
			throw std::runtime_error("SwapBuffers() failed");
	}
}

void stw::Graphics::init_opengl() {

	PIXELFORMATDESCRIPTOR pfd{};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pxFormat = ChoosePixelFormat(_context, &pfd);
	if (!::SetPixelFormat(_context, pxFormat, &pfd))
		throw std::runtime_error("SetPixelFormat() failed");

	HGLRC tmpCtx = wglCreateContext(_context);
	if (!wglMakeCurrent(_context, tmpCtx))
		throw std::runtime_error("wglMakeCurrent() failed");

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		throw std::runtime_error(
			reinterpret_cast<const char*>(glewGetErrorString(err)));
	}
}

//void stw::Graphics::blit() {
//	HDC memDC = CreateCompatibleDC(_context);
//
//	RECT clientRect;
//	GetClientRect(_worker, &clientRect);
//	LONG width = clientRect.right - clientRect.left;
//	LONG height = clientRect.bottom - clientRect.top;
//
//	HBITMAP bmp = CreateCompatibleBitmap(_context, width, height);
//
//	HBRUSH brush = CreateSolidBrush(RGB(0, 0xff, 0));
//	SelectObject(memDC, bmp);
//	SelectObject(memDC, brush);
//	Rectangle(memDC, 0, 0, width, height);
//
//	BitBlt(_context, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
//
//	DeleteObject(bmp);
//	DeleteObject(brush);
//	DeleteDC(memDC);
//}

BOOL CALLBACK stw::Graphics::EnumWindowsProc(HWND hwnd, LPARAM lparam) {
	Graphics* instance = reinterpret_cast<Graphics*>(lparam);

	HWND shellView = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
	if (shellView != NULL) {
		instance->_worker = FindWindowEx(NULL, hwnd, "WorkerW", NULL);
		return FALSE;
	}

	return TRUE;
}
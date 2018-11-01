#include "graphics.h"

#include <stdexcept>
#include <iostream>

stw::Graphics::Graphics() {
	_progman = FindWindow("Progman", NULL);
	if (_progman == NULL)
		throw std::runtime_error("Couln't find progman window");

	SendMessageTimeout(_progman, 0x052C, NULL, NULL, SMTO_NORMAL, 1000, NULL);

	EnumWindows(FindShellDefView, reinterpret_cast<LPARAM>(this));
	if (_worker == NULL)
		throw std::runtime_error("Failed to spawn worker");

	_context = GetDCEx(_worker, NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE); //0x403
	if (_context == NULL)
		throw std::runtime_error("Device context is null");

	init_opengl();
}

stw::Graphics::~Graphics() {
	ReleaseDC(_worker, _context);
	wglDeleteContext(_glContext);
	wglMakeCurrent(NULL, NULL);
}

void stw::Graphics::run(const Module& module) {
	_timestamp = std::chrono::high_resolution_clock::now();
	_lastFrame = std::chrono::high_resolution_clock::now();

	for (;;) {
		if (detect_fs_window()) {
			//don't render when fs app is running
			Sleep(100);
			continue;
		}

		glClear(GL_COLOR_BUFFER_BIT);

		auto current = std::chrono::high_resolution_clock::now();
		auto time_span = current - _timestamp;
		auto time = std::chrono::duration_cast<std::chrono::duration<float>>(time_span);

		Uniform data{
			time.count()
		};
		module.render(data);

		if (!SwapBuffers(_context))
			throw std::runtime_error("SwapBuffers() failed");

		auto delta_span = current - _lastFrame;
		auto delta = std::chrono::duration_cast<std::chrono::duration<float>>(delta_span);
		std::cout << 1.0f / delta.count() << "fps\r" << std::flush;

		_lastFrame = current;
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

	_glContext = wglCreateContext(_context);
	if (!wglMakeCurrent(_context, _glContext))
		throw std::runtime_error("wglMakeCurrent() failed");

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		throw std::runtime_error(
			reinterpret_cast<const char*>(glewGetErrorString(err)));
	}
}

bool stw::Graphics::detect_fs_window() {
	bool fsWindow = false;
	EnumWindows(DetectFSWindow, reinterpret_cast<LPARAM>(&fsWindow));
	return fsWindow;
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

BOOL CALLBACK stw::Graphics::FindShellDefView(HWND hwnd, LPARAM lparam) {
	Graphics* instance = reinterpret_cast<Graphics*>(lparam);

	HWND shellView = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
	if (shellView != NULL) {
		instance->_worker = FindWindowEx(NULL, hwnd, "WorkerW", NULL);
		return FALSE;
	}

	return TRUE;
}

BOOL CALLBACK stw::Graphics::DetectFSWindow(HWND hwnd, LPARAM lparam) {

	if (!IsWindowVisible(hwnd) || IsIconic(hwnd))
		return TRUE;

	//LONG wl = GetWindowLong(hwnd, -8); //GWL_HWNDPARENT
	//if (wl != 0 && wl != reinterpret_cast<LONG>(GetDesktopWindow()))
	//	return TRUE;

	int cloakedVal;
	//https://stackoverflow.com/questions/32149880/how-to-identify-windows-10-background-store-processes-that-have-non-displayed-wi
	HRESULT hRes = DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloakedVal, sizeof(cloakedVal));
	if (hRes != S_OK || cloakedVal)
		return TRUE;

	char windowTitle[100];
	GetWindowText(hwnd, windowTitle, sizeof(windowTitle));
	if (strlen(windowTitle) == 0 || strcmp(windowTitle, "Program Manager") == 0)
		return TRUE;

	const int sw = GetSystemMetrics(SM_CXSCREEN) - 200;
	const int sh = GetSystemMetrics(SM_CYSCREEN) - 200;

	RECT appBounds;
	GetWindowRect(hwnd, &appBounds);
	auto appHeight = appBounds.bottom - appBounds.top;
	auto appWidth = appBounds.right - appBounds.left;

	if (appWidth >= sw && appHeight >= sh) {
		*reinterpret_cast<bool*>(lparam) = true;
		return FALSE;
	}

	return TRUE;
}

#include "mWindow.h"
#include <assert.h>
#include <windowsx.h>

mKeyState mWindow::keyState(0x7B);
mMouseState mWindow::mouseState;

LRESULT CALLBACK mWindow::mMassageProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	//case WM_QUIT:

	case WM_CLOSE:
		PostQuitMessage(0);
		//return 0;
		break;


	case WM_KEYDOWN:
		if (wparam <= VK_F12 && wparam >= 0x0)
			keyState[wparam] = true;
		break;
	case WM_KEYUP:
		if (wparam <= VK_F12 && wparam >= 0x0)
			keyState[wparam] = false;
		break;


	case WM_LBUTTONDOWN:
		mouseState.setButtomState(true, false);
		break;
	case WM_RBUTTONDOWN:
		mouseState.setButtomState(false, true);
		break;
	case WM_LBUTTONUP:
		mouseState.setButtomState(false, false);
		break;
	case WM_RBUTTONUP:
		mouseState.setButtomState(false, false);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}



mWindow::mWindow()
	:hinstance{GetModuleHandle(nullptr)}
{
	WNDCLASSW wnc{};
	wnc.style = CS_OWNDC;
	wnc.lpfnWndProc = mMassageProc;
	wnc.cbClsExtra = 0;
	wnc.cbWndExtra = 0;
	wnc.hInstance = hinstance;
	wnc.hIcon = nullptr;
	wnc.hCursor = nullptr;
	wnc.hbrBackground = nullptr;
	wnc.lpszClassName = nullptr;
	wnc.lpszClassName = pClassName;
	RegisterClassW(&wnc);

	RECT winSize;
	winSize.left = 0;
	winSize.top = 0;
	winSize.right = 800;
	winSize.bottom = 600;
	AdjustWindowRect(&winSize, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,FALSE);
	mhwnd = CreateWindow(pClassName, L"mWindowName", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 
		winSize.right - winSize.left, winSize.bottom - winSize.top, nullptr, nullptr,hinstance, this);

	ShowWindow(mhwnd, SW_SHOWDEFAULT);
	

	pGraphic = std::make_shared<mGraphic>(mhwnd);
}

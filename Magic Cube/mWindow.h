#pragma once

#include <Windows.h>
#include "mGraphic.h"
#include <memory>
#include <vector>

#include "mInputState.h"

class mWindow
{
	static LRESULT CALLBACK mMassageProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	
	const wchar_t* pClassName = L"Magic Cube";

	HINSTANCE hinstance;
public:
	HWND mhwnd;
	

public:	
	mWindow();
	std::shared_ptr<mGraphic> pGraphic;

public:
	static mKeyState keyState;
	static mMouseState mouseState;
};


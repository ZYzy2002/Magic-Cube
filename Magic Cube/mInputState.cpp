#include "mInputState.h"
#include <Windows.h>

mKeyState::mKeyState(size_t i)
	:isKeyDown{ new bool[i] }
{
	memset(isKeyDown, 0, i);
}

mKeyState::~mKeyState()
{
	delete[] isKeyDown;
}

void mKeyState::SetKeyDown(int virtualKeyCode, bool keyState)
{
	isKeyDown[virtualKeyCode] = keyState;
}

bool& mKeyState::operator[](size_t i)
{
	return isKeyDown[i];
}

mMouseState::mMouseState()
	:isLButtonDown{false},isRButtonDown{false}
	
{
	POINT p{  };
	GetCursorPos(&p);
	cursorPosX = p.x;
	cursorPosY = p.y;
}

void mMouseState::setNewCursorPos(int x, int y)
{
	lastCursorPosX = cursorPosX;
	lastCursorPosY = cursorPosY;
	cursorPosX = x;
	cursorPosY = y;
}

void mMouseState::setButtomState(bool isLButtonDown, bool isRButtonDown)
{
	this->isLButtonDown = isLButtonDown;
	this->isRButtonDown = isRButtonDown;
}

void mMouseState::getMoveVector(_Out_ int& vectorX, _Out_ int& vectorY)
{
	vectorX = cursorPosX - lastCursorPosX;
	vectorY = cursorPosY - lastCursorPosY;
}

void mMouseState::getRButtonDownStateMoveVector(_Out_ int& vectorX, _Out_ int& vectorY)
{
	if (isRButtonDown)
	{
		getMoveVector(vectorX, vectorY);
	}
	else
	{
		vectorX = 0;
		vectorY = 0;
	}
}

void mMouseState::getLButtomDownStateMoveVector(_Out_ int& vectorX, _Out_ int& vectorY)
{
	if (isLButtonDown)
	{
		getMoveVector(vectorX, vectorY);
	}
	else
	{
		vectorX = 0;
		vectorY = 0;
	}
}

bool mMouseState::IsLButtonDown()
{
	return isLButtonDown;
}

void mMouseState::updateCursorPos()
{
	POINT p{  };
	GetCursorPos(&p);
	setNewCursorPos(p.x, p.y);
}


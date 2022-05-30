#pragma once
#include <vector>

class mKeyState
{
	bool* isKeyDown;	//array[ virtualKeyCodeMaxNum ]

public:
	mKeyState(size_t virtualKeyCodeMaxNum);
	~mKeyState();
	void SetKeyDown(int virtualKeyCode, bool keyState);
	bool& operator[](size_t i);
};

class mMouseState
{
public:
	mMouseState();

private:
	void setNewCursorPos(int x, int y);
public:
	void setButtomState(bool isLButtonDown, bool isRButtonDown);

	void getMoveVector(_Out_ int& vectorX, _Out_ int& vectorY);
	void getRButtonDownStateMoveVector(_Out_ int& vectorX, _Out_ int& vectorY);
	void getLButtomDownStateMoveVector(_Out_ int& vectorX, _Out_ int& vectorY);
	bool IsLButtonDown();


	void updateCursorPos();	//显示器坐标系下
private:
	bool isLButtonDown;
	bool isRButtonDown;
	int cursorPosX;			//显示器坐标
	int cursorPosY;
	int lastCursorPosX;
	int lastCursorPosY;
};


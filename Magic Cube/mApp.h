#pragma once

#include "mWindow.h"
#include <memory>
#include <string>

#include "mTimer.h"
#include "OneCube.h"
#include "mCamera.h"
#include "DirectXMathExtend.h"

using namespace std;

class mApp
{
private:
	mWindow	 mwin;
public:
	mApp();
	int Go();

	//Game object
	shared_ptr<OneCube> oneCubes[27];
	/*shared_ptr<OneCube> testCube;*/

	shared_ptr<mCamera> MainCamera;
	mTimer mtime;

private:
	void Doframe();
};


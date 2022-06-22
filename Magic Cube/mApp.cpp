#include "mApp.h"
#include <map>  //用于排序左键鼠标移动向量的分量

mApp::mApp()
{
	for (int i = 0; i < 27; ++i)
	{
		int x = i / 9;
		int y = (i - x * 9) / 3;
		int z = i % 3;
		oneCubes[i] = make_shared<OneCube>(mwin.pGraphic);
		oneCubes[i]->translation = XMFLOAT3((x - 1) * 0.4f, (y - 1) * 0.4f, (z - 1) * 0.4f);
	}
// 	testCube = make_shared<OneCube>(mwin.pGraphic);
// 	testCube->translation = XMFLOAT3{ 0.4, -0, 0 };


	MainCamera = make_shared<mCamera>(mwin.pGraphic);
	MainCamera->Bind();
}

int mApp::Go()
{
	MSG msg;
	while (true)
	{
		PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
		{
			return 0;
		}
		Doframe();
	}
	return 0;
}

void mApp::Doframe()
{
	//清空 渲染目标
	mwin.pGraphic->Clear();



	//相机位置
	float cameraVelosity = 0.02;
	if (mwin.keyState[0x57])		//w
	{
		MainCamera->CameraMove(MainCamera->getCameraForwardVector());
	}
	if (mwin.keyState[0x53])		//s
	{
		XMFLOAT3 fwdVector = MainCamera->getCameraForwardVector();
		MainCamera->CameraMove(XMFLOAT3{ -fwdVector.x, -fwdVector.y,-fwdVector.z });
	}
	if (mwin.keyState[0x41])		//a
	{
		MainCamera->CameraMove(MainCamera->getCameraLeftVector());
	}
	if (mwin.keyState[0x44])		//d
	{
		XMFLOAT3 leftVector = MainCamera->getCameraLeftVector();
		MainCamera->CameraMove(XMFLOAT3{ -leftVector.x, -leftVector.y,-leftVector.z });
	}
	if (mwin.keyState[0x45])		//e
	{
		MainCamera->CameraMove(MainCamera->getCameraUpVector());
	}
	if (mwin.keyState[0x51])		//q
	{
		XMFLOAT3 UpVector = MainCamera->getCameraUpVector();
		MainCamera->CameraMove(XMFLOAT3{ -UpVector.x, -UpVector.y,-UpVector.z });
	}
	if (mwin.keyState[0x46])	// key "F" 恢复默认位置  正面观察
	{
		MainCamera->translation = XMFLOAT3{ 0.0f, 0.0f, -3 };
		MainCamera->rotation = XMFLOAT3{ 0.0f , 0.0f, 0.0f };
	}
	if (mwin.keyState[0x42])	// key "B"   背面观察
	{
		MainCamera->translation = XMFLOAT3{ 0.0f, 0.0f, 3 };
		MainCamera->rotation = XMFLOAT3{ 0.0f , 3.14159, 0.0f };
	}


	// 鼠标光标
	float mouseMoveVelosity = 0.0015;
	mwin.mouseState.updateCursorPos();//更新显示器坐标（1920* 1080）下 的鼠标光标位置
	//右键调整摄像机视角方向
	int rButtomMoveX, rButtomMoveY;
	mwin.mouseState.getRButtonDownStateMoveVector(rButtomMoveX, rButtomMoveY);
	MainCamera->CameraLookUp(mouseMoveVelosity * rButtomMoveY);
	MainCamera->CamaraLookRight(mouseMoveVelosity * rButtomMoveX);




	//左键旋转OneCube
	POINT clientCenterCursorPos; GetCursorPos(&clientCenterCursorPos);
	ScreenToClient(mwin.mhwnd, &clientCenterCursorPos);
	clientCenterCursorPos.x -= 400; clientCenterCursorPos.y -= 300;
	XMFLOAT3 mouseCursorVector = {
		MainCamera->getCameraForwardVector()
		- clientCenterCursorPos.y / 300.f * (float)tan(MainCamera->FovAngleY / 2) * MainCamera->getCameraUpVector()		//clientCenterCursorPos.y 是 向下是正方向
		- clientCenterCursorPos.x / 400.f * (float)tan(MainCamera->FovAngleY / 2) * MainCamera->AspectRatio * MainCamera->getCameraLeftVector(),
	};

	int nearestCubeIndex = -1;
	float minDistance = 1e+6;
	for (int i = 0; i < 27; ++i)
	{
		bool isClickCube = oneCubes[i]->IsRayCrossCollisionBox(/*MainCamera->getCameraForwardVector()*/ mouseCursorVector, MainCamera->translation);
		if (isClickCube)
		{
			float currentCubeDistance = MainCamera->WorldDistance(oneCubes[i]->translation);
			if (minDistance > currentCubeDistance)		//寻找 与 光标方向最近的cube
			{
				nearestCubeIndex = i;
				minDistance = currentCubeDistance;
			}
		}
	}
 	static shared_ptr<OneCube> p[9] = { NULL };	//正在旋转的cube
	
	bool isCollidedCubeOneOfTheNotAlignedCubes = false;
	if (nearestCubeIndex != -1)
	{
		for (int i = 0; i < 9; i++)
		{
			isCollidedCubeOneOfTheNotAlignedCubes = isCollidedCubeOneOfTheNotAlignedCubes || (p[i] == oneCubes[nearestCubeIndex]);
		}
	}
	
	enum class Plane {X, Y, Z};
	static Plane RotationPlane = Plane::X;
	int lButtomMoveX, lButtomMoveY;
	mwin.mouseState.getLButtomDownStateMoveVector(lButtomMoveX, lButtomMoveY);
	XMFLOAT3 LMouseMoveVector =
	{
		-lButtomMoveX / 100.0f * MainCamera->getCameraLeftVector() + lButtomMoveY / 100.0f * MainCamera->getCameraUpVector()
	};
	//enum class RotationDirection { Clockwise, AntiClockwise};
	std::map<float, Plane> sort;
	sort[fabsf(LMouseMoveVector.x)] = Plane::X;
	sort[fabsf(LMouseMoveVector.y)] = Plane::Y;
	sort[fabsf(LMouseMoveVector.z)] = Plane::Z;

		
	if (isCollidedCubeOneOfTheNotAlignedCubes && nearestCubeIndex != -1)	
	{
		XMFLOAT3 rotator{ 0.f ,0.f ,0.0f };
		XMFLOAT3 RotateDirection;
		XMStoreFloat3(&RotateDirection, XMVector3Cross(XMLoadFloat3(&oneCubes[nearestCubeIndex]->translation), XMLoadFloat3(&LMouseMoveVector)));
		GetComponentProduct(rotator, size_t(RotationPlane)) = GetComponentProduct(RotateDirection, size_t(RotationPlane));

		for (auto i : p)
		{
			i->RotateAroundWorldCenter(rotator);
		}
	}
	if (!isCollidedCubeOneOfTheNotAlignedCubes && nearestCubeIndex != -1 && mwin.mouseState.IsLButtonDown())
	{
		for (auto i : p)							//对齐原有的 9Cubes
		{
			if (i != nullptr)
			{
				i->RotationApproachPIDIV2();
			}
		}
		RotationPlane = sort.begin()->second;		//选定新的 9Cubes
		int cubeBeFound = 0;
		for (auto i : oneCubes)
		{
			if ( fabsf(GetComponentProduct(oneCubes[nearestCubeIndex]->translation, size_t(RotationPlane)) - GetComponentProduct(i->translation, size_t(RotationPlane)))<0.01f)
			{
				p[cubeBeFound] = i;
				cubeBeFound++;
			}
		}
		assert(cubeBeFound == 9);					//****************
									
		XMFLOAT3 rotator{ 0.f, 0.f, 0.f };
		XMFLOAT3 RotateDirection;
		XMStoreFloat3(&RotateDirection, XMVector3Cross(XMLoadFloat3(&oneCubes[nearestCubeIndex]->translation), XMLoadFloat3(&LMouseMoveVector)));
		GetComponentProduct(rotator, size_t(RotationPlane)) = GetComponentProduct(RotateDirection, size_t(RotationPlane));

		for (auto i : p)
		{
			i->RotateAroundWorldCenter(rotator);
		}
	}




	//修改标题信息
	wstring title;
	{
		title += L"运行时间: ";
		double time = mtime.getTime() / 1000000000.0;
		title += to_wstring(time);
	}
	{
		title += L"  鼠标位置：";
		title += to_wstring(clientCenterCursorPos.x);
		title += L" ";
		title += to_wstring(clientCenterCursorPos.y);
	}
	{
		title += L"  左键速度：";
		title += to_wstring(lButtomMoveX);
		title += L" ";
		title += to_wstring(lButtomMoveY);
	}
	SetWindowTextW(mwin.mhwnd, title.c_str());

	//绘制
	MainCamera->Updata();
	
	for (int i = 0; i < 27; ++i)
	{
		oneCubes[i]->UpDate();
		oneCubes[i]->Draw();
	}
	/*testCube->UpDate(); testCube->Draw();*/

	mwin.pGraphic->Swap();
}

#include "mApp.h"
#include <map>  //���������������ƶ������ķ���

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
	//��� ��ȾĿ��
	mwin.pGraphic->Clear();



	//���λ��
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
	if (mwin.keyState[0x46])	// key "F" �ָ�Ĭ��λ��  ����۲�
	{
		MainCamera->translation = XMFLOAT3{ 0.0f, 0.0f, -3 };
		MainCamera->rotation = XMFLOAT3{ 0.0f , 0.0f, 0.0f };
	}
	if (mwin.keyState[0x42])	// key "B"   ����۲�
	{
		MainCamera->translation = XMFLOAT3{ 0.0f, 0.0f, 3 };
		MainCamera->rotation = XMFLOAT3{ 0.0f , 3.14159, 0.0f };
	}


	// �����
	float mouseMoveVelosity = 0.0015;
	mwin.mouseState.updateCursorPos();//������ʾ�����꣨1920* 1080���� �������λ��
	//�Ҽ�����������ӽǷ���
	int rButtomMoveX, rButtomMoveY;
	mwin.mouseState.getRButtonDownStateMoveVector(rButtomMoveX, rButtomMoveY);
	MainCamera->CameraLookUp(mouseMoveVelosity * rButtomMoveY);
	MainCamera->CamaraLookRight(mouseMoveVelosity * rButtomMoveX);




	//�����תOneCube
	POINT clientCenterCursorPos; GetCursorPos(&clientCenterCursorPos);
	ScreenToClient(mwin.mhwnd, &clientCenterCursorPos);
	clientCenterCursorPos.x -= 400; clientCenterCursorPos.y -= 300;
	XMFLOAT3 mouseCursorVector = {
		MainCamera->getCameraForwardVector()
		- clientCenterCursorPos.y / 300.f * (float)tan(MainCamera->FovAngleY / 2) * MainCamera->getCameraUpVector()		//clientCenterCursorPos.y �� ������������
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
			if (minDistance > currentCubeDistance)		//Ѱ�� �� ��귽�������cube
			{
				nearestCubeIndex = i;
				minDistance = currentCubeDistance;
			}
		}
	}
 	static shared_ptr<OneCube> p[9] = { NULL };	//������ת��cube
	
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
		for (auto i : p)							//����ԭ�е� 9Cubes
		{
			if (i != nullptr)
			{
				i->RotationApproachPIDIV2();
			}
		}
		RotationPlane = sort.begin()->second;		//ѡ���µ� 9Cubes
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




	//�޸ı�����Ϣ
	wstring title;
	{
		title += L"����ʱ��: ";
		double time = mtime.getTime() / 1000000000.0;
		title += to_wstring(time);
	}
	{
		title += L"  ���λ�ã�";
		title += to_wstring(clientCenterCursorPos.x);
		title += L" ";
		title += to_wstring(clientCenterCursorPos.y);
	}
	{
		title += L"  ����ٶȣ�";
		title += to_wstring(lButtomMoveX);
		title += L" ";
		title += to_wstring(lButtomMoveY);
	}
	SetWindowTextW(mwin.mhwnd, title.c_str());

	//����
	MainCamera->Updata();
	
	for (int i = 0; i < 27; ++i)
	{
		oneCubes[i]->UpDate();
		oneCubes[i]->Draw();
	}
	/*testCube->UpDate(); testCube->Draw();*/

	mwin.pGraphic->Swap();
}

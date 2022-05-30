#pragma once
#include <memory>
#include <DirectXMath.h>

#include "mGraphic.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

class mCamera
{
public:
	mCamera() = delete;
	mCamera(shared_ptr<mGraphic> gfx);

	XMFLOAT3 translation{ 0.f, 0.0f, -2 };
	XMFLOAT3 rotation{ 0.0 , 0.0f, 0.0f };
	XMFLOAT3 scale{ 1.0f, 1.0f, 1.0f };
	XMFLOAT3 getCameraForwardVector();
	XMFLOAT3 getCameraLeftVector();
	XMFLOAT3 getCameraUpVector();
	float WorldDistance(const XMFLOAT3& otherObjectWorldPos);

 	float cameraVelocity = 0.02;
 	void CameraMove(XMFLOAT3 DirectionVector);		//向单位向量上，移动 cameraVelocity
	void CameraLookUp(float angle);
	void CamaraLookRight(float angle);

	float FovAngleY = 3.1415927f /2.f;	
	float AspectRatio = 800.f / 600.f;
	float NearZ = 0.5f;
	float FarZ = 10.f;

	void Updata();
	void Bind();

private:
	shared_ptr<mGraphic> gfx;

	struct VConstBuffer01
	{
		XMMATRIX MATRIX_V;
		XMMATRIX MATRIX_P;
		XMFLOAT4 WorldSpaceViewPos;
	};

	ComPtr<ID3D11Buffer> pVConstBuffer01;

};


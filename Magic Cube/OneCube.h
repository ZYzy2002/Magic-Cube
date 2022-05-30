#pragma once
#include <memory>
#include <DirectXMath.h>
#include <vector>
#include "mGraphic.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class OneCube
{
public:
	OneCube() = delete;
	OneCube(std::shared_ptr<mGraphic>& gfx);

	void Draw();
	void UpDate();

public:
	XMFLOAT3 translation{ 0, 0, 0 };
	XMFLOAT3 rotation{ 0, 0, 0 };
	XMFLOAT3 scale{ 0.2, 0.2, 0.2 };
	void ChangeTransform(XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale);
	void RotateAroundWorldCenter(XMFLOAT3 DeltaRotate);

	bool RotationApproachPIDIV2();		//使旋转 是PI/2的倍数

	//collision and bound
	bool IsRayCrossCollisionBox(XMFLOAT3 directionWorldSpace, XMFLOAT3 pointWorldSpace);
private:
	float centerBoxSideLength = 2.0f;

private:
	std::shared_ptr<mGraphic> gfx;

	char modleFileName[30]= "Modle\\OneCube.fbx";

	struct VertexInfo
	{
		DirectX::XMFLOAT3 vertex;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};
	struct VConstBuffer
	{
 		DirectX::XMMATRIX Object_To_World;
 		DirectX::XMMATRIX World_To_Object;
 		DirectX::XMMATRIX MATRIX_V;
	};


	std::vector<VertexInfo> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	UINT VBstrides;
	UINT VBoffsets;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVConstBuffer;

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	wchar_t vertexShaderFileName[30] = L"testVertexShader.cso";
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	wchar_t pixelShaderFileName[30] = L"testPixelShader.cso";
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;

	wchar_t texture2DFileName[30] = L"Image\\OneMap2.png";
	ComPtr<ID3D11ShaderResourceView> pShaderview;
};


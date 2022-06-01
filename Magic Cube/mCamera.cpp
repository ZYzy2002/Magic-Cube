#include "mCamera.h"

mCamera::mCamera(shared_ptr<mGraphic> gfx)
	:gfx{gfx}
{
	VConstBuffer01 vertexCBuffer
	{

		// MATRIX_V 矩阵
		XMMatrixTranspose(
			/*XMMatrixScaling(scale.x, scale.y, scale.z)* */			// 相机不需要 缩放，此时可以不转置，以得到 逆矩阵（VSpace到WS的逆）
			XMMatrixInverse(nullptr,
				XMMatrixRotationX(rotation.x)
				* XMMatrixRotationY(rotation.y)
				* XMMatrixRotationZ(rotation.z)
				* XMMatrixTranslation(translation.x, translation.y, translation.z))
		),

		// MATIRX_P 矩阵
		XMMatrixTranspose(
			XMMatrixPerspectiveFovLH(FovAngleY,AspectRatio,NearZ,FarZ)
		),
		XMFLOAT4{translation.x, translation.y, translation.z, 1.0f}
	};
	
	D3D11_BUFFER_DESC VSCBufferDesc{};
	VSCBufferDesc.ByteWidth = sizeof(VConstBuffer01);
	VSCBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	VSCBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VSCBufferDesc.MiscFlags = 0u;
	VSCBufferDesc.StructureByteStride = 0u;
	VSCBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	D3D11_SUBRESOURCE_DATA initiaVCBuffer;
	initiaVCBuffer.pSysMem = &vertexCBuffer;
	gfx->pDevice->CreateBuffer(&VSCBufferDesc, &initiaVCBuffer, &pVConstBuffer01);
}

DirectX::XMFLOAT3 mCamera::getCameraForwardVector()
{
	XMFLOAT3 ret;
	XMStoreFloat3(&ret, XMVector3Transform(FXMVECTOR{ 0, 0, 1 }, XMMatrixRotationX(rotation.x)
		* XMMatrixRotationY(rotation.y)
		* XMMatrixRotationZ(rotation.z)
		));			//stroe 和 load 用于vector转float3 和float3 转 vector  
	return ret;
}

DirectX::XMFLOAT3 mCamera::getCameraLeftVector()
{
	XMFLOAT3 ret;
	XMStoreFloat3(&ret, XMVector3Transform(FXMVECTOR{ -1, 0, 0 }, XMMatrixRotationX(rotation.x)
		* XMMatrixRotationY(rotation.y)
		* XMMatrixRotationZ(rotation.z)
	));
	return ret;
}

DirectX::XMFLOAT3 mCamera::getCameraUpVector()
{
	XMFLOAT3 ret;
	XMStoreFloat3(&ret, XMVector3Transform(FXMVECTOR{ 0, 1, 0 }, XMMatrixRotationX(rotation.x)
		* XMMatrixRotationY(rotation.y)
		* XMMatrixRotationZ(rotation.z)
	));
	return ret;
}

float mCamera::WorldDistance(const XMFLOAT3& otherObjectWorldPos)
{
	XMVECTOR cameraPos = XMLoadFloat3(&translation), otherObjectPos = XMLoadFloat3(&otherObjectWorldPos);
	return XMVectorGetX(XMVector3Length(XMVectorSubtract(cameraPos, otherObjectPos)));
}

void mCamera::CameraMove(XMFLOAT3 DirectionVector)
{
	translation += DirectionVector* cameraVelocity;
}

void mCamera::CameraLookUp(float angle)
{
	rotation.x += angle;
}

void mCamera::CamaraLookRight(float angle)
{
	rotation.y += angle;
}

void mCamera::Bind()
{
	gfx->pContext->VSSetConstantBuffers(1u, 1U, pVConstBuffer01.GetAddressOf());
}

void mCamera::Updata()
{
	VConstBuffer01 newBuffer01
	{
		XMMatrixTranspose(
			/*XMMatrixScaling(scale.x, scale.y, scale.z)* */			// 相机不需要 缩放，此时可以不转置，以得到 逆矩阵（VSpace到WS的逆）
			XMMatrixInverse(nullptr,
				XMMatrixRotationX(rotation.x)
				* XMMatrixRotationY(rotation.y)
				* XMMatrixRotationZ(rotation.z)
				* XMMatrixTranslation(translation.x, translation.y, translation.z))
		),

		// MATIRX_P 矩阵
		XMMatrixTranspose(
			XMMatrixPerspectiveFovLH(FovAngleY,AspectRatio,NearZ,FarZ)
		),
		XMFLOAT4{translation.x, translation.y, translation.z, 1.0f}
	};
	D3D11_MAPPED_SUBRESOURCE destination{};
	gfx->pContext->Map(pVConstBuffer01.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &destination);
	memcpy(destination.pData, &newBuffer01, sizeof(VConstBuffer01));
	gfx->pContext->Unmap(pVConstBuffer01.Get(), 0u);
}



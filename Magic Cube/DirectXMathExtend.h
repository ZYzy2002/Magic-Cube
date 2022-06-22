#pragma once
#include <DirectXMath.h>

namespace DirectX
{
	const XMFLOAT3 operator*(XMFLOAT3 float3_1, float float_2);
	const XMFLOAT3 operator*(float float_1, XMFLOAT3 float3_2);
	const XMFLOAT3 operator+(XMFLOAT3 float3_1, XMFLOAT3 float3_2);
	const XMFLOAT3 operator-(XMFLOAT3 float3_1, XMFLOAT3 float3_2);
	XMFLOAT3& operator+=(XMFLOAT3& float3_1, const XMFLOAT3 float3_2);
	float& GetComponentProduct(XMFLOAT3& in,size_t i);
}
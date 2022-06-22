#include "DirectXMathExtend.h"

const DirectX::XMFLOAT3 DirectX::operator*(XMFLOAT3 float3_1, float float_2)
{
	return XMFLOAT3{ float3_1.x * float_2, float3_1.y * float_2, float3_1.z * float_2 };
}

const DirectX::XMFLOAT3 DirectX::operator*(float float_1, XMFLOAT3 float3_2)
{
	return float3_2 * float_1;
}

const DirectX::XMFLOAT3 DirectX::operator+(XMFLOAT3 float3_1, XMFLOAT3 float3_2)
{
	return XMFLOAT3{ float3_1.x + float3_2.x, float3_1.y + float3_2.y, float3_1.z + float3_2.z };
}

const DirectX::XMFLOAT3 DirectX::operator-(XMFLOAT3 float3_1, XMFLOAT3 float3_2)
{
	return XMFLOAT3{ float3_1.x - float3_2.x, float3_1.y - float3_2.y, float3_1.z - float3_2.z };
}

DirectX::XMFLOAT3& DirectX::operator+=(XMFLOAT3& float3_1, const XMFLOAT3 float3_2)
{
	float3_1 = float3_1 + float3_2;
	return float3_1;
}

float& DirectX::GetComponentProduct(XMFLOAT3& in, size_t i)
{
	switch (i)
	{
	case 0:
		return in.x;
		break;
	case 1:
		return in.y;
		break;
	case 2:
		return in.z;
		break;
	}
}


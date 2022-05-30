

cbuffer transform:register(b0)
{
	matrix Object_To_World;
	matrix World_To_Object;
}
cbuffer camera:register(b1)
{
	matrix MATRIX_V;
	matrix MATRIX_P;
	float4 WorldSpaceViewPos;
}

struct v2f
{
	float2 uv:TEXCOORD;
	float4 pos:SV_POSITION;
};



v2f main( float3 pos : POSITION, float2 texcoord:TEXCOORD) 
{

	//matrix a = {float4(1,2,3,4),float4(5,6,7,8),float4(9,10,11,12),float4(13,14,15,16)};
	
	matrix MATRIX_MV = mul(Object_To_World , MATRIX_V);
	matrix MATRIX_MVP = mul(MATRIX_MV, MATRIX_P);

	v2f o;

	
	
	//o.abc = mul( float4(pos,1),a);

	o.pos = mul( float4(pos,1),MATRIX_MVP);
	//o.pos = float4(pos.xy,0.5,1);
	
	
	o.uv = texcoord;
	o.uv.y = 1 - o.uv.y;	//maya 做出来的模型 的uv空间的原点在左下方，v正向朝上
	return o;
}


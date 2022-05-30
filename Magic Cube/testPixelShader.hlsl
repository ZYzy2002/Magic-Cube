


Texture2D _BaseColor:register(t0);
SamplerState defaultState
{
	
};


struct v2f
{
	float2 uv:TEXCOORD;
	float4 pos:SV_POSITION;
};


float4 main(v2f i) : SV_TARGET
{
	
	return _BaseColor.Sample(defaultState, i.uv);
	return float4(i.uv, 0, 1);

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
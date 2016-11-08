cbuffer cbGlobal : register(b0)
{
	column_major  float4x4 World;
	column_major  float4x4 View;
	column_major  float4x4 Projection;
	column_major  float4x4 WVP;
};

struct VS_INPUT
{
	float3 Pos : POSITION;
	float4 Color : COLOR0;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.Pos = mul(float4(input.Pos, 1), WVP);
	output.Color = input.Color;
	return output;
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
	return input.Color;
//	return diffuse;
}
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer cbMaterial : register(b3)
{
	float4 g_Ambient;
	float4 g_Diffuse;
	float4 g_Specular;
	float4 g_Emmisive;
	float  g_specularPower;
	float  g_transparency;
	float  dammy;
	float  dammy2;
};

struct PS_INPUT
{
	float4 Pos	: SV_POSITION;//位置
	float3 Norm : NORMAL;//頂点法線
	float2 Tex	: TEXCOORD;//テクスチャー座標
	float4 Color : COLOR0;//最終カラー（頂点シェーダーにおいての）
};

float4 PS(PS_INPUT input) : SV_Target
{
	float4 tex = txDiffuse.Sample(samLinear, input.Tex);
	return input.Color * tex;
}
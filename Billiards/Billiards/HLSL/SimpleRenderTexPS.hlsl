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
	float4 Pos	: SV_POSITION;//�ʒu
	float3 Norm : NORMAL;//���_�@��
	float2 Tex	: TEXCOORD;//�e�N�X�`���[���W
	float4 Color : COLOR0;//�ŏI�J���[�i���_�V�F�[�_�[�ɂ����Ắj
};

float4 PS(PS_INPUT input) : SV_Target
{
	float4 tex = txDiffuse.Sample(samLinear, input.Tex);
	return input.Color * tex;
}
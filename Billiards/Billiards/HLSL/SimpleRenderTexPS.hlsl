Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer cbMaterial : register(b0)
{
	float4 ambient;
	float4 diffuse;
	float3 specular;
	float power;
	float4 emmisive;
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
	return diffuse * tex;
}
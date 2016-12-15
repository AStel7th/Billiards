cbuffer global_0:register(b0)
{
	float4 g_vLight;  //���C�g�̕����x�N�g��
	float4 g_vEye;//�J�����ʒu
};

cbuffer global_1:register(b1)
{
	matrix World;
	matrix View;
	matrix Projection;
	matrix WVP;
};

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

//�o�[�e�b�N�X�o�b�t�@�[�̓���
struct VS_INPUT
{
	float3 Pos	: POSITION;//�ʒu   
	float3 Norm : NORMAL;//���_�@��
	float2 Tex	: TEXCOORD;//�e�N�X�`���[���W
};
//�s�N�Z���V�F�[�_�[�̓��́i�o�[�e�b�N�X�o�b�t�@�[�̏o�́j�@
struct PSSkinIn
{
	float4 Pos	: SV_POSITION;//�ʒu
	float3 Norm : NORMAL;//���_�@��
	float2 Tex	: TEXCOORD;//�e�N�X�`���[���W
	float4 Color : COLOR0;//�ŏI�J���[�i���_�V�F�[�_�[�ɂ����Ắj
};

//
//PSSkinIn VSSkin(VSSkinIn input )
//�o�[�e�b�N�X�V�F�[�_�[
PSSkinIn vs_main(VS_INPUT input)
{
	PSSkinIn output;

	output.Pos = mul(float4(input.Pos, 1.0f), WVP);
	output.Norm = mul(input.Norm, (float3x3)World);
	output.Tex = input.Tex;
	float3 LightDir = normalize((float3)g_vLight);
	float3 PosWorld = mul((float3)output.Pos, World);
	float3 ViewDir = normalize((float3)g_vEye - PosWorld);
	float3 Normal = normalize(output.Norm);
	float4 NL = saturate(dot(Normal, LightDir));

	float3 Reflect = normalize(2 * NL * Normal - LightDir);
	float4 specular = pow(saturate(dot(Reflect, ViewDir)), 4);



	output.Color = g_Diffuse * NL + specular*g_Specular;
	output.Color.w = g_transparency;

	return output;
}
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
	float4 Pos		: SV_POSITION;//�ʒu
	float3 Norm		: NORMAL;//���_�@��
	float4 Color	: COLOR0;//�ŏI�J���[�i���_�V�F�[�_�[�ɂ����Ắj
	float2 Tex		: TEXCOORD0;//�e�N�X�`���[���W
	float3 L		: TEXCOORD1;   //���_ -> ���C�g�ʒu �x�N�g��
	float3 E		: TEXCOORD2;   //���_ -> ���_ �x�N�g��
};

float4 PS(PS_INPUT input) : SV_Target
{
	float4 Out;

	//�@���x�N�g���𐳋K������
	float3 N = normalize(input.Norm);
	//���_ -> ���C�g�ʒu�x�N�g�� + ���_ -> ���_�x�N�g��(���ӂP)
	float3 H = normalize(input.L + normalize(input.E));
	//�X�y�L�����[�J���[���v�Z����(���ӂQ)
	float S = pow(max(0.0f, dot(N, H)), g_specularPower) * g_Specular;

	//�X�y�L�����[�J���[�����Z����
	Out = input.Color + S;

	return Out;
}
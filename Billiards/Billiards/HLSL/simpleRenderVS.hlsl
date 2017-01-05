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
//�o�[�e�b�N�X�o�b�t�@�[�̏o��
struct VS_OUTPUT
{
	float4 Pos		: SV_POSITION;//�ʒu
	float3 Norm		: NORMAL;//���_�@��
	float4 Color	: COLOR0;//�ŏI�J���[�i���_�V�F�[�_�[�ɂ����Ắj
	float2 Tex		: TEXCOORD0;//�e�N�X�`���[���W
	float3 L		: TEXCOORD1;   //���_ -> ���C�g�ʒu �x�N�g��
	float3 E		: TEXCOORD2;   //���_ -> ���_ �x�N�g��
};

//
//PSSkinIn VSSkin(VSSkinIn input )
//�o�[�e�b�N�X�V�F�[�_�[
VS_OUTPUT vs_main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.Pos = mul(float4(input.Pos, 1.0f), WVP);
	output.Norm = input.Norm;
	output.Tex = input.Tex;

	//���C�g�����œ��͂����̂ŁA���_ -> ���C�g�ʒu�Ƃ��邽�߂ɋt�����ɕϊ�����B�Ȃ��A�v���P�[�V�����ŕK�����K�����邱��
	output.L = -g_vLight.xyz;

	//���C�g�x�N�g���Ɩ@���x�N�g���̓��ς��v�Z���A�v�Z���ʂ̐F�̍Œ�l������( m_Ambient )�ɐ�������
	output.Color = g_Diffuse * min( max(dot(output.Norm, output.L) ,g_Ambient) + g_Emmisive, 1.0f );
	//output.Color = min(max(g_Ambient, dot(output.Norm, output.L)), 1.0f);

	//���_ -> ���_ �ւ̃x�N�g�����v�Z
	output.E = g_vEye.xyz - input.Pos.xyz;
	
	return output;
}
struct PerInstanceData
{
	matrix instanceMat;
};

StructuredBuffer<PerInstanceData>	g_pInstanceData :register( t0 );

cbuffer global_0:register(b0)
{
	float4 g_vLight;  //���C�g�̕����x�N�g��
	float4 g_vEye;//�J�����ʒu
};

cbuffer global_1:register(b1)
{
	matrix View;
	matrix Projection;
	matrix Local;
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

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Nor : NORMAL;
	float2 Tex : TEXCOORD;
};

//�s�N�Z���V�F�[�_�[�̓��́i�o�[�e�b�N�X�o�b�t�@�[�̏o�́j�@
struct PSSkinIn
{
	float4 Pos	: SV_POSITION;//�ʒu
	float3 Norm : NORMAL;//���_�@��
	float2 Tex	: TEXCOORD;//�e�N�X�`���[���W
	float4 Color : COLOR0;//�ŏI�J���[�i���_�V�F�[�_�[�ɂ����Ắj
};

PSSkinIn vs_main(VS_INPUT input, uint instanceID : SV_InstanceID)
{
	PSSkinIn output;

	matrix instanceWVP = mul(Projection, View);
	instanceWVP = mul(instanceWVP, g_pInstanceData[instanceID].instanceMat);
	instanceWVP = mul(instanceWVP, Local);

	instanceWVP = transpose(instanceWVP);

	output.Pos = mul(input.Pos, instanceWVP);
	output.Norm = mul(input.Nor, (float3x3)mul(g_pInstanceData[instanceID].instanceMat, Local));
	output.Tex = input.Tex;
	float3 LightDir = normalize(g_vLight);
	float3 PosWorld = mul(output.Pos, mul(Local,g_pInstanceData[instanceID].instanceMat));
	float3 ViewDir = normalize(g_vEye - PosWorld);
	float3 Normal = normalize(output.Norm);
	float4 NL = saturate(dot(Normal, LightDir));

	float3 Reflect = normalize(2 * NL * Normal - LightDir);
	float4 specular = pow(saturate(dot(Reflect, ViewDir)), 4);


	output.Color = g_Diffuse * NL + specular*g_Specular;
	output.Color.w = g_transparency;

	return output;
}
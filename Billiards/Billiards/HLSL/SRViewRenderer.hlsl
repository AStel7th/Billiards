// �萔�o�b�t�@
cbuffer CBuffer : register(b0)
{
	int4  g_SrcRGBA     : packoffset(c0);
	int   g_Dimension : packoffset(c1.x);
	float g_VolumeLevel : packoffset(c1.y);
};

// �e�N�X�`���[
Texture2D g_Tex0 : register(t0);
Texture2DArray g_Tex1 : register(t1);
Texture3D g_Tex2 : register(t2);

// �T���v���[�X�e�[�g
SamplerState  g_Sampler : register(s0);

// ���_�V�F�[�_�[�̓��̓p�����[�^
struct VS_IN
{
	float3 pos   : POSITION;   // ���_���W
	float2 texel : TEXCOORD0;  // �e�N�Z��
};

// ���_�V�F�[�_�[�̏o�̓p�����[�^
struct VS_OUT_PS_IN
{
	float4 pos   : SV_POSITION;
	float2 texel : TEXCOORD0;
};

// ���_�V�F�[�_�[
VS_OUT_PS_IN SRViewRenderer_VS_Main(VS_IN In)
{
	VS_OUT_PS_IN Out;

	Out.pos = float4(In.pos, 1);
	Out.texel = In.texel;

	return Out;
}

// �s�N�Z���V�F�[�_
float4 SRViewRenderer_PS_Main(VS_OUT_PS_IN In) : SV_TARGET
{
	float4 col = 0;
	float4 colRet = 0;
	switch (g_Dimension)
	{
	case 4:
		col = g_Tex0.Sample(g_Sampler, In.texel);
		break;
	case 5:
		col = g_Tex1.Sample(g_Sampler, float3(In.texel, g_VolumeLevel));
		break;
	case 8:
		col = g_Tex2.Sample(g_Sampler, float3(In.texel, g_VolumeLevel));
		break;
	}
	for (int i = 0; i<4; i++)
		colRet[i] = col[g_SrcRGBA[i]];
	return colRet;
}
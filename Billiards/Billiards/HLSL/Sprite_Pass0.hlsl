cbuffer ObjBuffer : register(b0)
{
	matrix g_world;
	matrix g_proj;
	float g_uvLeft;
	float g_uvTop;
	float g_uvW;
	float g_uvH;
	float4 color;
}


// テクスチャー
Texture2D g_Tex : register(t0);

// サンプラーステート
SamplerState  g_Sampler : register(s0);



// 頂点シェーダーの入力パラメータ
struct VS_IN
{
	float3 pos   : POSITION;   // 頂点座標
	float2 uv : TEXCOORD0;  // テクセル
};

// 頂点シェーダーの出力パラメータ
struct VS_OUT_PS_IN
{
	float4 pos   : SV_POSITION;
	float2 uv : TEXCOORD0;
};

// 頂点シェーダー
VS_OUT_PS_IN Sprite_Pass0_VS_Main(VS_IN In)
{
	VS_OUT_PS_IN Out = (VS_OUT_PS_IN)0;

	Out.pos = mul(float4(In.pos, 1.0f), g_world);
	Out.pos = mul(Out.pos, g_proj);
	Out.uv = In.uv *float2(g_uvW, g_uvH) + float2(g_uvLeft, g_uvTop);


	return Out;
}

// ピクセルシェーダ
float4 Sprite_Pass0_PS_Main(VS_OUT_PS_IN In) : SV_TARGET
{
	float4 col = g_Tex.Sample(g_Sampler, In.uv);
	col  *= color;
	return col;
}
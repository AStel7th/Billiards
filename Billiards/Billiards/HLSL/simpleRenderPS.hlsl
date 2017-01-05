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
	float4 Pos		: SV_POSITION;//位置
	float3 Norm		: NORMAL;//頂点法線
	float4 Color	: COLOR0;//最終カラー（頂点シェーダーにおいての）
	float2 Tex		: TEXCOORD0;//テクスチャー座標
	float3 L		: TEXCOORD1;   //頂点 -> ライト位置 ベクトル
	float3 E		: TEXCOORD2;   //頂点 -> 視点 ベクトル
};

float4 PS(PS_INPUT input) : SV_Target
{
	float4 Out;

	//法線ベクトルを正規化する
	float3 N = normalize(input.Norm);
	//頂点 -> ライト位置ベクトル + 頂点 -> 視点ベクトル(注意１)
	float3 H = normalize(input.L + normalize(input.E));
	//スペキュラーカラーを計算する(注意２)
	float S = pow(max(0.0f, dot(N, H)), g_specularPower) * g_Specular;

	//スペキュラーカラーを加算する
	Out = input.Color + S;

	return Out;
}
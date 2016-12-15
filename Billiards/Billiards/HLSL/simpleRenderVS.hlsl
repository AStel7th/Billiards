cbuffer global_0:register(b0)
{
	float4 g_vLight;  //ライトの方向ベクトル
	float4 g_vEye;//カメラ位置
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

//バーテックスバッファーの入力
struct VS_INPUT
{
	float3 Pos	: POSITION;//位置   
	float3 Norm : NORMAL;//頂点法線
	float2 Tex	: TEXCOORD;//テクスチャー座標
};
//ピクセルシェーダーの入力（バーテックスバッファーの出力）　
struct PSSkinIn
{
	float4 Pos	: SV_POSITION;//位置
	float3 Norm : NORMAL;//頂点法線
	float2 Tex	: TEXCOORD;//テクスチャー座標
	float4 Color : COLOR0;//最終カラー（頂点シェーダーにおいての）
};

//
//PSSkinIn VSSkin(VSSkinIn input )
//バーテックスシェーダー
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
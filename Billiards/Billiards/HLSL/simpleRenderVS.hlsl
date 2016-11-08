//定義
#define MAX_BONE_MATRICES 255

//グローバル
Texture2D g_texDecal: register(t0);//テクスチャーは レジスターt(n)
SamplerState g_samLinear : register(s0);//サンプラーはレジスターs(n)

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

cbuffer global_bones:register(b2)//ボーンのポーズ行列が入る
{
	matrix g_mConstBoneWorld[MAX_BONE_MATRICES];
};

cbuffer cbMaterial : register(b3)
{
	float4 g_Ambient = float4(0, 0, 0, 0);
	float4 g_Diffuse = float4(1, 0, 0, 0);
	float4 g_Specular = float4(1, 1, 1, 1);
	float g_Power;
	float4 g_Emmisive;
};


//スキニング後の頂点・法線が入る
struct Skin
{
	float4 Pos;
	float3 Norm;
};
//バーテックスバッファーの入力
struct VSSkinIn
{
	float3 Pos	: POSITION;//位置   
	float3 Norm : NORMAL;//頂点法線
	float2 Tex	: TEXCOORD;//テクスチャー座標
	uint4  Bones : BONE_INDEX;//ボーンのインデックス
	float4 Weights : BONE_WEIGHT;//ボーンの重み
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
PSSkinIn vs_main(VSSkinIn input)
{
	PSSkinIn output;

	output.Pos = mul(float4(input.Pos, 1.0f), WVP);
	output.Norm = input.Norm;
	output.Tex = input.Tex;
	float3 LightDir = normalize(g_vLight);
	float3 PosWorld = mul(input.Pos, World);
	float3 ViewDir = normalize(g_vEye - PosWorld);
	float3 Normal = normalize(output.Norm);
	float4 NL = saturate(dot(Normal, LightDir));

	float3 Reflect = normalize(2 * NL * Normal - LightDir);
	float4 specular = pow(saturate(dot(Reflect, ViewDir)), 4);



	output.Color = g_Diffuse * NL + specular*g_Specular;

	return output;
}
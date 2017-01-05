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
//バーテックスバッファーの出力
struct VS_OUTPUT
{
	float4 Pos		: SV_POSITION;//位置
	float3 Norm		: NORMAL;//頂点法線
	float4 Color	: COLOR0;//最終カラー（頂点シェーダーにおいての）
	float2 Tex		: TEXCOORD0;//テクスチャー座標
	float3 L		: TEXCOORD1;   //頂点 -> ライト位置 ベクトル
	float3 E		: TEXCOORD2;   //頂点 -> 視点 ベクトル
};

//
//PSSkinIn VSSkin(VSSkinIn input )
//バーテックスシェーダー
VS_OUTPUT vs_main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.Pos = mul(float4(input.Pos, 1.0f), WVP);
	output.Norm = input.Norm;
	output.Tex = input.Tex;

	//ライト方向で入力されるので、頂点 -> ライト位置とするために逆向きに変換する。なおアプリケーションで必ず正規化すること
	output.L = -g_vLight.xyz;

	//ライトベクトルと法線ベクトルの内積を計算し、計算結果の色の最低値を環境光( m_Ambient )に制限する
	output.Color = g_Diffuse * min( max(dot(output.Norm, output.L) ,g_Ambient) + g_Emmisive, 1.0f );
	//output.Color = min(max(g_Ambient, dot(output.Norm, output.L)), 1.0f);

	//頂点 -> 視点 へのベクトルを計算
	output.E = g_vEye.xyz - input.Pos.xyz;
	
	return output;
}
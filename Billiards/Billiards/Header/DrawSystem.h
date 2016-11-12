#ifndef DRAWSYSTEM_H
#define DRAWSYSTEM_H

#include <map>
#include <memory>
#include "Sprite.h"
#include "Shader.h"
#include "GameObject.h"

const UINT MAX_BONES = 255;

enum struct SHADER_ID
{
	VERTEX,
	PIXEL,
	GEOMETORY,
	COMPUTE,
	HULL,
	DOMEIN,
	ALL
};

struct SHADER_GLOBAL
{
	XMFLOAT4 lightDir;
	XMFLOAT4 eye;
};

//シェーダーに渡すボーン行列配列
struct SHADER_GLOBAL_BONES
{
	XMFLOAT4X4 mBone[MAX_BONES];
	SHADER_GLOBAL_BONES()
	{
		for (int i = 0; i<MAX_BONES; i++)
		{
			XMStoreFloat4x4(&mBone[i], XMMatrixIdentity());
		}
	}
};

struct CBMATRIX
{
	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 mWVP;
};

/////////////////////////////////////////////////
// DrawSystem
//
//    シングルトン　描画管理
////////////////////////////////////////
class DrawSystem
{

private:
	Direct3D11 &d3d11 = Direct3D11::Instance();

	ComPtr<ID3D11RasterizerState> pRS;
	ComPtr<ID3D11RasterizerState> pRSnoCull;
	ComPtr<ID3D11BlendState> pBlendState;
	ComPtr<ID3D11DepthStencilState>	pDepthStencilState;

	unique_ptr<VertexShader> pvsFBX = nullptr;
	unique_ptr<VertexShader> pvsFBXInstancing = nullptr;
	unique_ptr<VertexShader> pvsVertexColor = nullptr;
	unique_ptr<VertexShader> pvsFBXAnimation = nullptr;
	unique_ptr<VertexShader> pPlaneVS_UV = nullptr;
	unique_ptr<PixelShader> pPlanePS_UV = nullptr;
	unique_ptr<PixelShader> ppsFBX = nullptr;
	unique_ptr<PixelShader> ppsVertexColor = nullptr;
	unique_ptr<PixelShader> ppsFBXAnimation = nullptr;

	ComPtr<ID3D11InputLayout> pInputLayoutUV;
	ComPtr<ID3D11InputLayout> pInputLayoutUV1;
	ComPtr<ID3D11InputLayout> pInputLayoutUV2;
	ComPtr<ID3D11InputLayout> pInputLayoutColor;

	ComPtr<ID3D11Buffer> pcBuffer0 = nullptr;
	ComPtr<ID3D11Buffer> pcBuffer1 = nullptr;
	ComPtr<ID3D11Buffer> pcBoneBuffer = nullptr;


protected:
	// シングルトンなのでコンストラクタをプロトタイプ宣言します
	DrawSystem();
public:
	virtual ~DrawSystem();

	// シングルトンオブジェクトを取得
	static DrawSystem &Instance()
	{
		static DrawSystem inst;
		return inst;
	}

	// システムを初期化する
	virtual HRESULT Init(unsigned int msaaSamples);

	// リストに登録されたタスクを描画する
	virtual bool Draw();

	ID3D11Buffer* GetCBuffer(int i);
	void SetCBufferVS(int i);
};

#endif //DRAWSYSTEM_H
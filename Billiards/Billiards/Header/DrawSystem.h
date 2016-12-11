#pragma once
#include <unordered_map>
#include <memory>
#include "Sprite.h"
#include "Shader.h"
#include "GameObject.h"

class GraphicsComponent;
struct MESH;

const UINT MAX_BONES = 255;
const UINT MAX_INSTANCE = 255;

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

enum struct DRAW_PRIOLITY
{
	Opaque,		// 不透明オブジェクト
	Alpha,		// 半透明オブジェクト
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

struct CBMATRIX_INSTANCING
{
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 mLocal;
};

struct SRVPerInstanceData
{
	XMFLOAT4X4 mWorld;
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

	ComPtr<ID3D11RasterizerState>	pRS = nullptr;
	ComPtr<ID3D11RasterizerState>	pRSnoCull = nullptr;
	ComPtr<ID3D11BlendState>		pBlendState = nullptr;
	ComPtr<ID3D11DepthStencilState>	pDepthStencilState = nullptr;

	VertexShader*	pvsFBX = nullptr;
	VertexShader*	pvsFBXInstancing = nullptr;
	VertexShader*	pvsFBXAnimation = nullptr;
	VertexShader*	pvsFBXAnimInstancing = nullptr;
	PixelShader*	ppsFBX = nullptr;
	PixelShader*	ppsVertexColor = nullptr;
	PixelShader*	ppsFBXAnimation = nullptr;

	ComPtr<ID3D11InputLayout> pInputLayoutStaticMesh = nullptr;
	ComPtr<ID3D11InputLayout> pInputLayoutAnimation = nullptr;
	ComPtr<ID3D11InputLayout> pInputLayoutStaticMeshInstance = nullptr;
	ComPtr<ID3D11InputLayout> pInputLayoutAnimationInstance = nullptr;

	ComPtr<ID3D11Buffer> pcBuffer0 = nullptr;
	ComPtr<ID3D11Buffer> pcBuffer1 = nullptr;
	ComPtr<ID3D11Buffer> pcBufferInstance = nullptr;
	ComPtr<ID3D11Buffer> pcBoneBuffer = nullptr;
	ComPtr<ID3D11Buffer> pTransformStructuredBuffer = nullptr;
	ComPtr<ID3D11ShaderResourceView> pTransformSRV = nullptr;

	unordered_map<string, vector<GraphicsComponent*>> opaqueDrawList;
	unordered_map<string, vector<GraphicsComponent*>> alphaDrawList;

	XMFLOAT4X4 view;
	XMFLOAT4X4 proj;

	void Render(GraphicsComponent* pGC,bool isAnim);
	void RenderInstancing(vector<GraphicsComponent*>& pGClist, int refCnt, bool isAnim);

	void SetMatrix(vector<GraphicsComponent*>& pGClist);

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

	void AddDrawList(DRAW_PRIOLITY priolity, const string& tag,GraphicsComponent* pGC);
};
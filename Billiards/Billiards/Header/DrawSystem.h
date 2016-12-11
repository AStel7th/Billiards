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
	Opaque,		// �s�����I�u�W�F�N�g
	Alpha,		// �������I�u�W�F�N�g
};

struct SHADER_GLOBAL
{
	XMFLOAT4 lightDir;
	XMFLOAT4 eye;
};

//�V�F�[�_�[�ɓn���{�[���s��z��
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
//    �V���O���g���@�`��Ǘ�
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
	// �V���O���g���Ȃ̂ŃR���X�g���N�^���v���g�^�C�v�錾���܂�
	DrawSystem();
public:
	virtual ~DrawSystem();

	// �V���O���g���I�u�W�F�N�g���擾
	static DrawSystem &Instance()
	{
		static DrawSystem inst;
		return inst;
	}

	// �V�X�e��������������
	virtual HRESULT Init(unsigned int msaaSamples);

	// ���X�g�ɓo�^���ꂽ�^�X�N��`�悷��
	virtual bool Draw();

	void AddDrawList(DRAW_PRIOLITY priolity, const string& tag,GraphicsComponent* pGC);
};
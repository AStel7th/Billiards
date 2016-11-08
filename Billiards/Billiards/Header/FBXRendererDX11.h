#pragma once

#include "FBXLoader.h"
#include "../DirectXTex/WICTextureLoader.h"
#include "../DirectXTex/DirectXTex.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "DrawSystem.h"

using namespace DirectX;

struct	VERTEX_DATA
{
	XMFLOAT3	vPos;
	XMFLOAT3	vNor;
	XMFLOAT2	vTexcoord;
	UINT boneIndex[4];	//ボーン番号
	float boneWeight[4];  //ボーン重み

	VERTEX_DATA()
	{
		ZeroMemory(this, sizeof(VERTEX_DATA));
	}
};

//ボーン構造体
struct BONE
{
	XMFLOAT4X4 mBindPose;//初期ポーズ（ずっと変わらない）
	XMFLOAT4X4 mNewPose;//現在のポーズ（その都度変わる）

	BONE()
	{
		XMStoreFloat4x4(&mBindPose, XMMatrixIdentity());
		XMStoreFloat4x4(&mNewPose, XMMatrixIdentity());
	}
};

struct MATERIAL_CONSTANT_DATA
{
	XMFLOAT4	ambient;
	XMFLOAT4	diffuse;
	XMFLOAT4	specular;
	XMFLOAT4	emmisive;
};

struct MATERIAL_DATA
{
	XMFLOAT4	ambient;
	XMFLOAT4	diffuse;
	XMFLOAT4	specular;
	XMFLOAT4	emmisive;
	float specularPower;
	float TransparencyFactor;		// 透過度

	DWORD faceCount;//そのマテリアルであるポリゴン数

	MATERIAL_CONSTANT_DATA materialConstantData;

	ComPtr<ID3D11ShaderResourceView> pSRV;
	ComPtr<ID3D11SamplerState> pSampler;
	ComPtr<ID3D11Buffer> pMaterialCb;

	MATERIAL_DATA()
	{
		pSRV = nullptr;
		pSampler = nullptr;
		pMaterialCb = nullptr;
	}
};

struct	MESH_NODE
{
	ComPtr<ID3D11Buffer> m_pVB;
	vector<ComPtr<ID3D11Buffer>> m_pIB;
	/*ID3D11InputLayout*	m_pInputLayout;*/

	XMFLOAT4X4 mLocal;

	DWORD	vertexCount;
	DWORD   uvCount;
	DWORD	indexCount;
	DWORD   materialCount;

	vector<MATERIAL_DATA> materialData;

	float	mat4x4[16];

	int boneCount;
	vector<BONE> boneArray;

	// INDEX BUFFERのBIT
	enum INDEX_BIT
	{
		INDEX_NOINDEX = 0,
		INDEX_16BIT,		// 16bitインデックス
		INDEX_32BIT,		// 32bitインデックス
	};
	INDEX_BIT	m_indexBit;

	MESH_NODE()
	{
		m_pVB = nullptr;
		m_pIB.clear();
		boneArray.clear();
		materialData.clear();
		//m_pInputLayout = nullptr;
		m_indexBit = INDEX_NOINDEX;
		vertexCount = 0;
		uvCount = 0;
		indexCount = 0;
		materialCount = 0;
		boneCount = 0;
	}

	void SetIndexBit(const size_t indexCount)
	{
#if 0
		if (indexCount == 0)
			m_indexBit = INDEX_NOINDEX;
		else if (indexCount < 0xffff)
			m_indexBit = INDEX_16BIT;
		else if (indexCount >= 0xffff)
			m_indexBit = INDEX_32BIT;
#else
		// 現状、16bitインデックス対応はまだ
		m_indexBit = INDEX_NOINDEX;
		if (indexCount != 0)
			m_indexBit = INDEX_32BIT;
#endif
	};
};

class FBXRenderDX11
{
	Direct3D11 &d3d11 = Direct3D11::Instance();

	DrawSystem &drawSystem = DrawSystem::Instance();

	FBXLoader*	m_pFBX;

	std::vector<MESH_NODE>	m_meshNodeArray;

	XMFLOAT4X4 m_World, m_View, m_Proj;

	HRESULT CreateNodes();
	HRESULT VertexConstruction(FBX_MESH_NODE &fbxNode, MESH_NODE& meshNode, vector<VERTEX_DATA>& vData);
	HRESULT MaterialConstruction(FBX_MESH_NODE &fbxNode, MESH_NODE& meshNode);
	HRESULT BoneConstruction(FBX_MESH_NODE &fbxNode, MESH_NODE& meshNode, vector<VERTEX_DATA>& vData);

	HRESULT CreateVertexBuffer(ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount);
	HRESULT CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer);

	XMFLOAT4X4 GetCurrentPoseMatrix(MESH_NODE* node, int index);
	void SetNewPoseMatrix(FBX_MESH_NODE &fbxNode, MESH_NODE* meshNode, int frame = 0);

public:
	FBXRenderDX11();
	~FBXRenderDX11();

	void Release();

	HRESULT LoadFBX(const char* filename);
	HRESULT LoadFBX(FBXLoader* loader);
	/*HRESULT CreateInputLayout(ID3D11Device*	pd3dDevice, const void* pShaderBytecodeWithInputSignature, size_t BytecodeLength, D3D11_INPUT_ELEMENT_DESC* pLayout, unsigned int layoutSize);*/

	HRESULT RenderAll();
	HRESULT RenderNode(const size_t nodeId);
	HRESULT RenderNodeInstancing(const size_t nodeId, const uint32_t InstanceCount);
	HRESULT RenderNodeInstancingIndirect(const size_t nodeId, ID3D11Buffer* pBufferForArgs, const uint32_t AlignedByteOffsetForArgs);

	void Draw(int frame = 0, bool bInstance = false);

	void SetMatrix(XMFLOAT4X4& world, XMFLOAT4X4& view, XMFLOAT4X4& proj);

	size_t GetNodeCount() { return m_meshNodeArray.size(); }

	MESH_NODE& GetNode(const int id) { return m_meshNodeArray[id]; };
	void	GetNodeMatrix(const int id, float* mat4x4) { memcpy(mat4x4, m_meshNodeArray[id].mat4x4, sizeof(float) * 16); };
	MATERIAL_DATA& GetNodeMaterial(const size_t id, const size_t no) { return m_meshNodeArray[id].materialData[no]; };
};
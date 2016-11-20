#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <wrl/client.h>
#include "func.h"

#ifndef FBXSDK_NEW_API
#define FBXSDK_NEW_API	// 新しいバージョン使うとき用
#endif

#include <fbxsdk.h>
#include <Windows.h>
#include <string>
#include "../DirectXTex/WICTextureLoader.h"
#include "../DirectXTex/DirectXTex.h"
#include "FBXInitialize.h"
#include "MeshData.h"

using namespace std;
using namespace Microsoft::WRL;

// UVSet名, 頂点内のUVセット順序
typedef std::tr1::unordered_map<std::string, int> UVsetID;
// UVSet名, テクスチャパス名(１つのUVSetに複数のテクスチャがぶら下がってることがある)
typedef std::tr1::unordered_map<std::string, std::vector<std::string>> TextureSet;

struct FBX_VERTEX_DATA
{
	FbxVector4 pos;
	FbxVector4 nor;
	FbxVector2 uv;
	UINT boneIndex[4];	//ボーン番号
	float boneWeight[4];  //ボ
};

struct FBX_MATRIAL_ELEMENT
{
	enum MATERIAL_ELEMENT_TYPE
	{
		ELEMENT_NONE = 0,
		ELEMENT_COLOR,
		ELEMENT_TEXTURE,
		ELEMENT_BOTH,
		ELEMENT_MAX,
	};

	MATERIAL_ELEMENT_TYPE type;
	float r, g, b, a;
	TextureSet			textureSetArray;

	FBX_MATRIAL_ELEMENT() :r(), g(), b(), a()
	{
		type = ELEMENT_NONE;
		textureSetArray.clear();
	}

	~FBX_MATRIAL_ELEMENT()
	{
		Release();
	}

	void Release()
	{
		for (TextureSet::iterator it = textureSetArray.begin(); it != textureSetArray.end(); ++it)
		{
			it->second.clear();
		}

		textureSetArray.clear();
	}
};

struct FBX_MATERIAL_NODE
{
	// FBXのマテリアルはLambertとPhongしかない
	enum eMATERIAL_TYPE
	{
		MATERIAL_LAMBERT = 0,
		MATERIAL_PHONG,
	};

	eMATERIAL_TYPE type;
	FBX_MATRIAL_ELEMENT ambient;
	FBX_MATRIAL_ELEMENT diffuse;
	FBX_MATRIAL_ELEMENT emmisive;
	FBX_MATRIAL_ELEMENT specular;

	float shininess;
	float TransparencyFactor;		// 透過度

	int indexCount;
	vector<int> pIndex;

	FBX_MATERIAL_NODE() :ambient(), diffuse(), emmisive(), specular(), shininess(), TransparencyFactor()
	{
		type = MATERIAL_LAMBERT;
		indexCount = 0;
	}
};

// メッシュ構成要素
struct MESH_ELEMENTS
{
	unsigned int	numPosition;		// 頂点座標のセットをいくつ持つか
	unsigned int	numNormal;			//
	unsigned int	numUVSet;			// UVセット数

	MESH_ELEMENTS() :numPosition(), numNormal(), numUVSet()
	{

	}
};

//１頂点の共有　最大20ポリゴンまで
struct POLY_TABLE
{
	int PolyIndex[20];//ポリゴン番号 
	int Index123[20];//3つの頂点のうち、何番目か
	int NumRef;//属しているポリゴン数

	POLY_TABLE()
	{
		ZeroMemory(this, sizeof(POLY_TABLE));
	}
};

//
struct FBX_MESH_NODE
{
	std::string		name;			// ノード名
	std::string		parentName;		// 親ノード名(親がいないなら"null"という名称が入る.rootノードの対応)

	FbxMesh* m_pMesh;

	MESH_ELEMENTS	elements;		// メッシュが保持するデータ構造
	std::vector<FBX_MATERIAL_NODE> m_materialArray;		// マテリアル
	UVsetID		uvsetID;

	std::vector<FBX_VERTEX_DATA>	m_vertexDataArray;
	std::vector<unsigned int>		m_indexArray;				// インデックス配列
	std::vector<FbxCluster*>		m_boneArray;			// ボーン配列

	DWORD faceCount;//そのマテリアルであるポリゴン数
	DWORD UVCount;
	DWORD vertexCount;

	std::vector<POLY_TABLE> polyTable;

	float	mat4x4[16];	// Matrix

	bool anim;

	FBX_MESH_NODE() :name(), parentName(), elements(), faceCount(), UVCount(), vertexCount()
	{
		m_pMesh = nullptr;
		Release();
	}

	~FBX_MESH_NODE()
	{
		Release();
	}

	void Release()
	{
		polyTable.clear();
		uvsetID.clear();
		m_vertexDataArray.clear();
		m_materialArray.clear();
		m_indexArray.clear();
	}
};

class FBXLoader
{
public:
	
private:
	FBX* pFbx;
	int faceCount;

	std::vector<FBX_MESH_NODE>		m_meshNodeArray;

	vector<VERTEX_DATA> vDataArray;
	vector<BONE_DATA_PER_VERTEX> vbDataArray;
	vector<int> indexes;

	void SetupNode(FbxNode* pNode, std::string parentName, MeshData& mData);
	void Setup(MeshData& mData);

	void CopyVertexData(FbxMesh*	pMesh, MESH* mesh, FbxDouble3 translation, FbxDouble3 scale, FbxDouble3 rotation);
	void CopyMatrialData(FbxSurfaceMaterial* mat, MESH* mesh, int indexCnt,vector<int>& indexArray,MaterialData& data);
	void CopyBoneData(FbxMesh*	pMesh, MESH* mesh, vector<POLY_TABLE>& pPolyTable);

	XMFLOAT3& GetNormal(FbxMesh*	pMesh, int index);
	XMFLOAT2& GetUV(FbxMesh*	pMesh, int index, MESH* mesh, int polyNum, int inPolyPos);
	vector<POLY_TABLE>& GetPolyTable(FbxMesh*	pMesh, MESH* mesh, vector<POLY_TABLE>& outArray);

	HRESULT CreateVertexBuffer(ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount);
	HRESULT CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer);

	void ComputeNodeMatrix(FbxNode* pNode, MESH* meshNode);

	void SetFbxColor(XMFLOAT4& data, const FbxDouble3 srcColor);
	FbxDouble3 GetMaterialProperty(
		const FbxSurfaceMaterial * pMaterial,
		const char * pPropertyName,
		const char * pFactorPropertyName,
		FBX_MATRIAL_ELEMENT*			pElement);

public:
	FBXLoader();
	~FBXLoader();

	void Release();

	// 読み込み
	MeshData LoadFBX(const char* filename, string modelName);
	FbxNode&	GetRootNode();

	size_t GetNodesCount() { return m_meshNodeArray.size(); };		// ノード数の取得

	FBX_MESH_NODE&	GetNode(const unsigned int id);

	//void SetNewPoseMatrices(int frame);
};
#pragma once

#include <fbxsdk.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "FBXInitialize.h"

#ifndef FBXSDK_NEW_API
#define FBXSDK_NEW_API	// 新しいバージョン使うとき用
#endif

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

struct	COLLISION_VERTEX_DATA
{
	XMFLOAT3	vPos;
	XMFLOAT3	vNor;
};

struct FBX_VERTEX_DATA_COL
{
	FbxVector4 pos;
	FbxVector4 nor;
};

struct POLYGON_DATA
{
	COLLISION_VERTEX_DATA vertexArray[3];	//3角形ポリゴンのみ対応
	XMFLOAT3 normal;			//ポリゴンの法線
};

struct NODE_COLLISION
{
	vector<POLYGON_DATA> m_polyDataArray;
	float	mat4x4[16];
};

struct FBX_COLLISION_NODE
{
	std::vector<FBX_VERTEX_DATA_COL>	m_vertexDataArray;
	std::vector<unsigned int>		m_indexArray;				// インデックス配列

	DWORD faceCount;//そのマテリアルであるポリゴン数
	DWORD vertexCount;

	float	mat4x4[16];	// Matrix

	FBX_COLLISION_NODE() :faceCount(), vertexCount()
	{
		Release();
	}

	~FBX_COLLISION_NODE()
	{
		Release();
	}

	void Release()
	{
		m_vertexDataArray.clear();
		m_indexArray.clear();
	}
};

class CollisionFromFBX
{
public:

	CollisionFromFBX();
	~CollisionFromFBX();

	HRESULT LoadFBX(const char* filename);

	vector<NODE_COLLISION> GetMeshData();
private:
	unique_ptr<FBX> fbx;

	vector<FBX_COLLISION_NODE> m_fbxNodeArray;
	vector<NODE_COLLISION> m_nodeArray;

	void Setup();
	void SetupNode(FbxNode* pNode);
	void CopyVertexData(FbxMesh*	pMesh, FBX_COLLISION_NODE* meshNode, FbxDouble3 translation, FbxDouble3 scale, FbxDouble3 rotation);
	void ComputeNodeMatrix(FbxNode* pNode, FBX_COLLISION_NODE* meshNode);
	
	HRESULT CreateNodes();
	HRESULT VertexConstruction(FBX_COLLISION_NODE &fbxNode, NODE_COLLISION& meshNode);
};
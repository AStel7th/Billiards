#pragma once
#include <vector>
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>
#ifndef FBXSDK_NEW_API
#define FBXSDK_NEW_API	// �V�����o�[�W�����g���Ƃ��p
#endif

#include <fbxsdk.h>

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

class MaterialData;

struct	VERTEX_DATA
{
	XMFLOAT3	vPos;
	XMFLOAT3	vNor;
	XMFLOAT2	vTexcoord;

	VERTEX_DATA() : vPos(0.0f, 0.0f, 0.0f), vNor(0.0f, 0.0f, 0.0f), vTexcoord(0.0f, 0.0f)
	{

	}
};

struct BONE_DATA_PER_VERTEX
{
	UINT boneIndex[4];	//�{�[���ԍ�
	float boneWeight[4];  //�{�[���d��
};


//�{�[���\����
struct BONE
{
	XMFLOAT4X4 mBindPose;//�����|�[�Y�i�����ƕς��Ȃ��j
	XMFLOAT4X4 mNewPose;//���݂̃|�[�Y�i���̓s�x�ς��j

	BONE()
	{
		XMStoreFloat4x4(&mBindPose, XMMatrixIdentity());
		XMStoreFloat4x4(&mNewPose, XMMatrixIdentity());
	}
};

struct MESH
{
	string name;
	string parentName;
	ComPtr<ID3D11Buffer> pVB;
	ComPtr<ID3D11Buffer> pVB_Bone;
	vector<ComPtr<ID3D11Buffer>> pIB;
	XMFLOAT4X4 mLocal;
	vector<MaterialData*> materialData;
	vector<BONE> boneArray;
	vector<FbxCluster*>	fbx_boneArray;			// �{�[���s��X�V�p

	MESH()
	{
		name = "";
		parentName = "";
		pVB = nullptr;
		pVB_Bone = nullptr;
		pIB.clear();
		boneArray.clear();
		materialData.clear();
	}
};

class MeshData
{
private:
	string modelName;
	vector<MESH> meshList;
	int			 meshCnt;
public:
	bool isAnimation = false;

	MeshData();
	virtual ~MeshData();

	vector<MESH>& GetMeshList();

	int GetMeshCount();

	void SetName(string name);

	string& GetName();

	void SetAnimationFrame(int index, int frame);

	XMFLOAT4X4 GetCurrentPose(MESH* mesh, int index);
};
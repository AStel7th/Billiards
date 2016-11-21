#include "../Header/MeshData.h"

MeshData::MeshData()
{
}

MeshData::~MeshData()
{
}

vector<MESH>& MeshData::GetMeshList()
{
	return meshList;
}

int MeshData::GetMeshCount()
{
	return meshList.size();
}

void MeshData::SetName(string name)
{
	modelName = name;
}

string& MeshData::GetName()
{
	return modelName;
}

void MeshData::SetAnimationFrame(int index,int frame)
{
	XMMATRIX m(-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	FbxTime time;
	time.SetTime(0, 0, 0, frame, 0, FbxTime::eFrames60);//30�t���[��/�b�@�Ɛ���@�����ɂ͏󋵂��Ƃɒ��ׂ�K�v����

	for (unsigned int i = 0; i< meshList[index].boneArray.size(); i++)
	{
		FbxAMatrix mat = meshList[index].fbx_boneArray[i]->GetLink()->EvaluateGlobalTransform(time);

		FbxMatrix newMat = FbxMatrix(mat);

		for (int x = 0; x<4; x++)
		{
			for (int y = 0; y<4; y++)
			{
				meshList[index].boneArray[i].mNewPose.m[y][x] = (float)newMat.Get(y, x);
			}
		}

		/*XMMATRIX newPose = XMLoadFloat4x4(&meshNode->boneArray[i].mNewPose);
		XMStoreFloat4x4(&meshNode->boneArray[i].mNewPose, XMMatrixMultiply(newPose, m));*/
	}
}

XMFLOAT4X4 MeshData::GetCurrentPose(MESH* mesh,int index)
{
	XMFLOAT4X4 result;
	XMMATRIX inv;
	XMMATRIX bindPose = XMLoadFloat4x4(&mesh->boneArray[index].mBindPose);
	XMMATRIX newPose = XMLoadFloat4x4(&mesh->boneArray[index].mNewPose);

	inv = XMMatrixInverse(0, bindPose);//FBX�̃o�C���h�|�[�Y�͏����p���i��΍��W�j
	XMStoreFloat4x4(&result, inv * newPose); //�o�C���h�|�[�Y�̋t�s��ƃt���[���p���s���������B�Ȃ��A�o�C���h�|�[�Y���̂����ɋt�s��ł���Ƃ���l��������B�iFBX�̏ꍇ�͈Ⴄ���j

	return result;
}

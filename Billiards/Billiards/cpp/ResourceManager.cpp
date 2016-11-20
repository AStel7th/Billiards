#include "../Header/ResourceManager.h"
#include "../Header/FBXRendererDX11.h"
#include "../Header/FBXLoader.h"

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
	modelList.clear();
}

bool ResourceManager::Init()
{
	return true;
}

MeshData* ResourceManager::GetResource(const string & name, const char* filename)
{
	// �I�u�W�F�N�gID�`�F�b�N
	map<string, ModelData>::iterator modelName = modelList.find(name);

	// ID���Ȃ���Βǉ�
	if (modelName == modelList.end())
	{
		if (filename == nullptr)
			return nullptr;
		
		FBXLoader* modelData = NEW FBXLoader();
		MeshData data = modelData->LoadFBX(filename,name);

		ModelData model;
		model.meshData = data;
		model.refCnt = 1;

		// �V����ID���v�f�}�b�v���쐬
		modelList.insert(make_pair(name, model));

		SAFE_DELETE(modelData);

		return &modelList[name].meshData;
	}
	else
	{
		modelName->second.refCnt++;
	}

	return &modelName->second.meshData;
}

int ResourceManager::GetRefCount(const string & name)
{
	return modelList[name].refCnt;
}

void ResourceManager::ReduceReference(const string & name)
{
	modelList[name].refCnt--;
}


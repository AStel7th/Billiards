#include "../Header/ResourceManager.h"
#include "../Header/FBXRendererDX11.h"
#include "../Header/FBXLoader.h"

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
	for each (pair<string, FBXLoader*> var in modelList)
	{
		SAFE_DELETE(var.second);
	}
	modelList.clear();
}

bool ResourceManager::Init()
{
	return true;
}

MeshData* ResourceManager::GetResource(const string & name, const char* filename)
{
	// �I�u�W�F�N�gID�`�F�b�N
	map<string, FBXLoader*>::iterator modelName = modelList.find(name);

	// ID���Ȃ���Βǉ�
	if (modelName == modelList.end())
	{
		if (filename == nullptr)
			return nullptr;
		
		FBXLoader* modelData = NEW FBXLoader();
		modelData->LoadFBX(filename,name);

		// �V����ID���v�f�}�b�v���쐬
		modelList.insert(make_pair(name, modelData));

		return modelList[name]->GetMeshData();
	}
	
	return modelName->second->GetMeshData();
}
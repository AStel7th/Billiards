#include "../Header/ResourceManager.h"

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

void ResourceManager::GetResource(FBXRenderDX11& outModel, const string & name, const char* filename)
{
	// �I�u�W�F�N�gID�`�F�b�N
	map<string, shared_ptr<FBXLoader>>::iterator modelName = modelList.find(name);

	// ID���Ȃ���Βǉ�
	if (modelName == modelList.end())
	{
		if (filename == nullptr)
			return;

		shared_ptr<FBXLoader> modelData = make_shared<FBXLoader>();
		modelData->LoadFBX(filename);
		//outModel = NEW FBXRenderDX11();
		outModel.LoadFBX(modelData.get());

		// �V����ID���v�f�}�b�v���쐬
		modelList.insert(make_pair(name, modelData));
	}
	else
	{
		// ID������Ί����̂��̂�n��
		outModel.LoadFBX(modelName->second.get());
	}
}


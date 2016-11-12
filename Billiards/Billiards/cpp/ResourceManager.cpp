#include "../Header/ResourceManager.h"
#include "../Header/FBXRendererDX11.h"
#include "../Header/FBXLoader.h"

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
	for each (pair<string,FBXLoader*> var in modelList)
	{
		SAFE_DELETE(var.second);
	}

	modelList.clear();
}

bool ResourceManager::Init()
{
	return true;
}

void ResourceManager::GetResource(FBXRenderDX11& outModel, const string & name, const char* filename)
{
	// オブジェクトIDチェック
	map<string, FBXLoader*>::iterator modelName = modelList.find(name);

	// IDがなければ追加
	if (modelName == modelList.end())
	{
		if (filename == nullptr)
			return;

		FBXLoader* modelData = NEW FBXLoader();
		modelData->LoadFBX(filename);
		outModel.LoadFBX(modelData);

		// 新しいID内要素マップを作成
		modelList.insert(make_pair(name, modelData));
	}
	else
	{
		// IDがあれば既存のものを渡す
		outModel.LoadFBX(modelName->second);
	}
}


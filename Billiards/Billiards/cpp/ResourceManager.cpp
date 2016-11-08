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
	// オブジェクトIDチェック
	map<string, shared_ptr<FBXLoader>>::iterator modelName = modelList.find(name);

	// IDがなければ追加
	if (modelName == modelList.end())
	{
		if (filename == nullptr)
			return;

		shared_ptr<FBXLoader> modelData = make_shared<FBXLoader>();
		modelData->LoadFBX(filename);
		//outModel = NEW FBXRenderDX11();
		outModel.LoadFBX(modelData.get());

		// 新しいID内要素マップを作成
		modelList.insert(make_pair(name, modelData));
	}
	else
	{
		// IDがあれば既存のものを渡す
		outModel.LoadFBX(modelName->second.get());
	}
}


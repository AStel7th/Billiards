#pragma once
#include <map>
#include <string>
#include "FBXRendererDX11.h"

using namespace std;

class ResourceManager
{
private:
	map<string, shared_ptr<FBXLoader>> modelList;
protected:
	ResourceManager();
public:
	virtual ~ResourceManager();

	// シングルトンオブジェクトを取得
	static ResourceManager &Instance()
	{
		static ResourceManager inst;
		return inst;
	}

	bool Init();

	void GetResource(FBXRenderDX11& outModel, const string & name, const char* filename = nullptr);
};
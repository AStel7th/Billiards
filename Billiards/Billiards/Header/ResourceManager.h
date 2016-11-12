#pragma once
#include <map>
#include <string>

using namespace std;

class FBXRenderDX11;
class FBXLoader;

class ResourceManager
{
private:
	map<string, FBXLoader*> modelList;
protected:
	ResourceManager();
public:
	virtual ~ResourceManager();

	// �V���O���g���I�u�W�F�N�g���擾
	static ResourceManager &Instance()
	{
		static ResourceManager inst;
		return inst;
	}

	bool Init();

	void GetResource(FBXRenderDX11& outModel, const string & name, const char* filename = nullptr);
};
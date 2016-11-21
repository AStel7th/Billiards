#pragma once
#include <map>
#include <string>
#include "FBXLoader.h"

using namespace std;

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

	MeshData* GetResource(const string & name, const char* filename = nullptr);
};
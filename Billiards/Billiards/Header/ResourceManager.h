#pragma once
#include <map>
#include <string>
#include "MeshData.h"

using namespace std;

struct ModelData
{
	MeshData meshData;
	int refCnt;

	ModelData() : refCnt(0){}
};

class ResourceManager
{
private:
	map<string, ModelData> modelList;
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

	MeshData* GetResource(const string & name, const char* filename = nullptr);

	int GetRefCount(const string & name);

	void ReduceReference(const string & name);
};
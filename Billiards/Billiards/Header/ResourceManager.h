#pragma once
#include <map>
#include <string>

using namespace std;

class FBXLoader;
class MeshData;
class WaveFileLoader;

class ResourceManager
{
private:
	map<string, FBXLoader> modelList;
	map<string, WaveFileLoader> waveFileList;

	MeshData* GetModelData(const string & name, const char* filename = nullptr);

	WaveFileLoader* GetWaveData(const string & name, const char* filename = nullptr);

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

	void GetResource(MeshData** data,const string & name, const char* filename = nullptr);
	void GetResource(WaveFileLoader** data,const string & name, const char* filename = nullptr);
};
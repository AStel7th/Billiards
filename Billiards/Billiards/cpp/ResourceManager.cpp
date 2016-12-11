#include "../Header/ResourceManager.h"
#include "../Header/FBXLoader.h"
#include "../Header/WaveFileLoader.h"

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
	modelList.clear();
	waveFileList.clear();
}

bool ResourceManager::Init()
{
	return true;
}

void ResourceManager::GetResource(MeshData** data, const string & name, const char* filename)
{
	*data = GetModelData(name, filename);
}

void ResourceManager::GetResource(WaveFileLoader** data, const string & name, const char* filename)
{
	*data = GetWaveData(name, filename);
}


MeshData * ResourceManager::GetModelData(const string & name, const char * filename)
{
	// �I�u�W�F�N�gID�`�F�b�N
	map<string, FBXLoader>::iterator modelName = modelList.find(name);

	// ID���Ȃ���Βǉ�
	if (modelName == modelList.end())
	{
		if (filename == nullptr)
			return nullptr;

		modelList[name].LoadFBX(filename, name);

		return modelList[name].GetMeshData();
	}

	return modelName->second.GetMeshData();
}

WaveFileLoader * ResourceManager::GetWaveData(const string & name, const char * filename)
{
	// �I�u�W�F�N�gID�`�F�b�N
	map<string, WaveFileLoader>::iterator waveFileName = waveFileList.find(name);

	// ID���Ȃ���Βǉ�
	if (waveFileName == waveFileList.end())
	{
		if (filename == nullptr)
			return nullptr;

		LPWSTR str;
		StringToWideChar(&str, filename);
		waveFileList[name].Open(str, nullptr, WAVEFILE_READ);

		SAFE_DELETE(str);

		return &waveFileList[name];
	}

	return &waveFileName->second;
}
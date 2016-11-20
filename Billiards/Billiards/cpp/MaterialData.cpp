#include "../Header/MaterialData.h"

MaterialData::MaterialData()
{
	ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	emmisive = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	specularPower = 0.0f;
	transparencyFactor = 0.0f;
	pSRV = nullptr;
	pSampler = nullptr;
	pMaterialCb = nullptr;
}



MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

MaterialData * MaterialManager::CreateMaterial(string matName)
{
	// オブジェクトIDチェック
	unordered_map<string, MaterialData>::iterator materialName = materialList.find(matName);

	if (materialName == materialList.end())
	{
		MaterialData matData;
		
		materialList.insert(make_pair(matName, matData));

		return &materialList[matName];
	}

	return &materialName->second;
}

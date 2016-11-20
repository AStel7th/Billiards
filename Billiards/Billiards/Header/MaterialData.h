#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <unordered_map>
#include <string>
#include "func.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;


struct MATERIAL_CONSTANT_DATA
{
	XMFLOAT4	ambient;
	XMFLOAT4	diffuse;
	XMFLOAT4	specular;
	XMFLOAT4	emmisive;
};

class MaterialData
{
private:
	
public:
	XMFLOAT4	ambient;
	XMFLOAT4	diffuse;
	XMFLOAT4	specular;
	XMFLOAT4	emmisive;
	float specularPower;
	float transparencyFactor;		// ���ߓx
	ComPtr<ID3D11ShaderResourceView> pSRV;
	ComPtr<ID3D11SamplerState> pSampler;
	ComPtr<ID3D11Buffer> pMaterialCb;
	DWORD faceCount;//���̃}�e���A���ł���|���S����
	MATERIAL_CONSTANT_DATA materialConstantData;

	MaterialData();
	virtual ~MaterialData() {}

	void SetTexture();
};

class MaterialManager
{
private:
	unordered_map<string, MaterialData> materialList;
protected:
	MaterialManager();
public:
	static MaterialManager& Instance()
	{
		static MaterialManager inst;
		return inst;
	}

	virtual ~MaterialManager();

	MaterialData* CreateMaterial(string matName);
};
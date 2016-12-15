#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <unordered_map>
#include <string>
#include "../DirectXTex/WICTextureLoader.h"
#include "../DirectXTex/DirectXTex.h"
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
	float		specularPower;
	float		transparency;
	float		dammy;
	float		dammy2;
};

class MaterialData
{
private:
	
public:
	string name;
	XMFLOAT4	ambient;
	XMFLOAT4	diffuse;
	XMFLOAT4	specular;
	XMFLOAT4	emmisive;
	float specularPower;
	float transparencyFactor;		// 透過度
	ComPtr<ID3D11ShaderResourceView> pSRV;
	ComPtr<ID3D11SamplerState> pSampler;
	ComPtr<ID3D11Buffer> pMaterialCb;
	DWORD faceCount;//そのマテリアルであるポリゴン数
	MATERIAL_CONSTANT_DATA materialConstantData;

	MaterialData();
	virtual ~MaterialData() {}

	void SetTexture(const string& path);
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
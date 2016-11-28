#include "../Header/MaterialData.h"
#include "../Header/Direct3D11.h"

//拡張子取得関数
inline string GetExtension(const string &path)
{
	string ext;
	size_t pos1 = path.rfind('.');
	if (pos1 != string::npos) {
		ext = path.substr(pos1 + 1, path.size() - pos1);
		string::iterator itr = ext.begin();
		while (itr != ext.end()) {
			*itr = tolower(*itr);
			itr++;
		}
		itr = ext.end() - 1;
		while (itr != ext.begin()) {    // パスの最後に\0やスペースがあったときの対策
			if (*itr == 0 || *itr == 32) {
				ext.erase(itr--);
			}
			else {
				itr--;
			}
		}
	}

	return ext;
}

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

void MaterialData::SetTexture(const string& path)
{
	HRESULT hr;

	TexMetadata metadata;
	ScratchImage image;

	std::string ext = GetExtension(path);

	WCHAR	wstr[512];
	size_t wLen = 0;
	mbstowcs_s(&wLen, wstr, path.size() + 1, path.c_str(), _TRUNCATE);

	//Resourceからのパスを追加
	TCHAR res[512] = L"Resource/";
	wcscat_s(res, wstr);

	if (ext == "tga")
	{
		hr = LoadFromTGAFile(res, &metadata, image);
		if (FAILED(hr)) {
			return;
		}
	}
	else
	{
		hr = LoadFromWICFile(res, 0, &metadata, image);
		if (FAILED(hr)) {
			return;
		}
	}


	// シェーダーリソースビューの作成
	hr = CreateShaderResourceView(Direct3D11::Instance().pD3DDevice.Get(), image.GetImages(), image.GetImageCount(), metadata, &pSRV);
	if (FAILED(hr)) {
		return;
	}
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
		matData.name = matName;
		
		materialList.insert(make_pair(matName, matData));

		return &materialList[matName];
	}

	return &materialName->second;
}

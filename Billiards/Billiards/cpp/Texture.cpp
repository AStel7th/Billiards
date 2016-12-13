#include "..\Header\Texture.h"

void Texture::Create(const string & path)
{
	HRESULT hr;

	string ext = GetExtension(path);

	WCHAR	wstr[512];
	size_t wLen = 0;
	mbstowcs_s(&wLen, wstr, path.size() + 1, path.c_str(), _TRUNCATE);

	if (ext == "tga")
	{
		hr = LoadFromTGAFile(wstr, &metadata, image);
		if (FAILED(hr))
			return;
	}
	else
	{
		hr = LoadFromWICFile(wstr, 0, &metadata, image);
		if (FAILED(hr))
		{
			DWORD a = GetLastError();
			return;
		}
	}


	// シェーダーリソースビューの作成
	hr = CreateShaderResourceView(Direct3D11::Instance().pD3DDevice.Get(), image.GetImages(), image.GetImageCount(), metadata, &pSRV);
	if (FAILED(hr))
		return;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = Direct3D11::Instance().pD3DDevice->CreateSamplerState(&sampDesc, &pSampler);
	if (FAILED(hr))
		return;
}

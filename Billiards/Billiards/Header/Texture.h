#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "func.h"
#include "Direct3D11.h"
#include "../DirectXTex/DirectXTex.h"
#include "../DirectXTex/WICTextureLoader.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

class Texture
{
private:
	TexMetadata metadata;
	ScratchImage image;
	ComPtr<ID3D11ShaderResourceView> pSRV;
	ComPtr<ID3D11SamplerState> pSampler;
	XMFLOAT4 uv;
public:
	Texture() : pSRV(nullptr),pSampler(nullptr),uv(0.0f,0.0f,1.0f,1.0f){}

	virtual ~Texture() {}

	void Create(const string& path);

	void SetUV(int left, int top, int width, int height)
	{
		uv.z = (float)width / metadata.width;
		uv.x = (float)left / metadata.width;
		uv.y = (float)top / metadata.height;
		uv.w = (float)height / metadata.height;
	}

	TexMetadata* GetMetadata()
	{
		return &metadata;
	}

	ScratchImage* GetImage()
	{
		return &image;
	}

	ComPtr<ID3D11ShaderResourceView> GetSRV()
	{
		return pSRV;
	}

	ComPtr<ID3D11SamplerState> GetSampler()
	{
		return pSampler;
	}
};
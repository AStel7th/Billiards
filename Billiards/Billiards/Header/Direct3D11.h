#pragma once
#include <crtdbg.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "func.h"
#include "Exception.h"
#include "../DirectXTex/DirectXTex.h"
#include "../DirectXTex/WICTextureLoader.h"

#define UD3D11_FORMAT        DXGI_FORMAT_R8G8B8A8_UNORM

// �[�x�X�e���V���r���[�ƃV�F�[�_�[���\�[�X�r���[�̂ǂ���Ƃ��Ă��g�p�ł���悤�ɂ��邽�ߌ^���w�肵�Ȃ�
// �t�H�[�}�b�g�ɂ��Ă�DXGI_FORMAT�Q��
#define UD3D11_DEPTH_FORMAT  DXGI_FORMAT_R32_TYPELESS

#define DDS_MAGIC 0x20534444 // "DDS "

#define DDS_HEADER_FLAGS_TEXTURE        0x00001007  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT 
#define DDS_HEADER_FLAGS_MIPMAP         0x00020000  // DDSD_MIPMAPCOUNT
#define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH
#define DDS_HEADER_FLAGS_PITCH          0x00000008  // DDSD_PITCH
#define DDS_HEADER_FLAGS_LINEARSIZE     0x00080000  // DDSD_LINEARSIZE

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_RGBA        0x00000041  // DDPF_RGB | DDPF_ALPHAPIXELS
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

using namespace Microsoft::WRL;

class Maps
{
private:
	struct DDS_PIXELFORMAT
	{
		DWORD dwSize;
		DWORD dwFlags;
		DWORD dwFourCC;
		DWORD dwRGBBitCount;
		DWORD dwRBitMask;
		DWORD dwGBitMask;
		DWORD dwBBitMask;
		DWORD dwABitMask;
	};

	typedef struct
	{
		DWORD dwSize;
		DWORD dwHeaderFlags;
		DWORD dwHeight;
		DWORD dwWidth;
		DWORD dwPitchOrLinearSize;
		DWORD dwDepth;
		DWORD dwMipMapCount;
		DWORD dwReserved1[11];
		DDS_PIXELFORMAT ddspf;
		DWORD dwSurfaceFlags;
		DWORD dwCubemapFlags;
		DWORD dwReserved2[3];
	} DDS_HEADER;

	Maps();
	bool static IsBitMask(DDS_HEADER* pHeader, DWORD r, DWORD g, DWORD b, DWORD a);

public:
	//WIC�t�@�C���ǂݍ��݂���̃e�N�X�`���[�쐬
	void static CreateSRViewFromWICFile(ID3D11Device* pD3DDevice, const WCHAR* szFileName, ID3D11ShaderResourceView** ppSRV);
	void static CreateSRViewFromWICFileArray(ID3D11Device* pD3DDevice, WCHAR* szFileName[], UINT NumTextures, ID3D11ShaderResourceView** ppSRV);

	// DDS�t�@�C�������[�h���ăe�N�X�`���[���쐬����
	// R8G8B8A8�܂���R8G8B8X8�t�H�[�}�b�g�݂̂Ń~�b�v�}�b�v��1�̂݁A�{�����[���e�N�X�`���[�ƃL���[�u�}�b�v�ƈ��k�t�H�[�}�b�g�͖��Ή�
	void static CreateSRViewFromDDSFile(ID3D11Device* pD3DDevice, const WCHAR* szFileName, ID3D11ShaderResourceView** ppSRV);

	// �����_�[�^�[�Q�b�g�r���[���쐬����
	void static CreateRenderTargetView(ID3D11Device* pD3DDevice,
		DXGI_FORMAT format, UINT Width, UINT Height,
		ID3D11RenderTargetView** ppRTView, ID3D11ShaderResourceView** ppSRView,
		UINT ArraySize = 1,
		UINT MipmapCount = 1);

	// �{�����[���e�N�X�`���[�p�̃����_�[�^�[�Q�b�g�r���[���쐬����
	void static CreateRenderTargetViewOfVolumeTexture(ID3D11Device* pD3DDevice,
		DXGI_FORMAT format, UINT Width, UINT Height, UINT Depth,
		ID3D11RenderTargetView** ppRTView, ID3D11ShaderResourceView** ppSRView);

	// �����_�[�^�[�Q�b�g�r���[����V�F�[�_�[���\�[�X�r���[���쐬����
	void static CreateSRViewFromRTView(ID3D11Device* pD3DDevice, ID3D11RenderTargetView* pRTView, ID3D11ShaderResourceView** ppSRView);

	// �A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
	void static CreateRenderTargetViewOfRWTexture(ID3D11Device* pD3DDevice,
		DXGI_FORMAT format, UINT Width, UINT Height,
		ID3D11UnorderedAccessView** ppUAView, ID3D11ShaderResourceView** ppSRView,
		UINT ArraySize = 1);

	// �A���I�[�_�[�h�A�N�Z�X�r���[����V�F�[�_�[���\�[�X�r���[���쐬����
	void static CreateSRViewFromUAView(ID3D11Device* pD3DDevice, ID3D11UnorderedAccessView* pUAView, ID3D11ShaderResourceView** ppSRView);

	// �[�x�X�e���V���r���[���쐬����
	void static CreateDepthStencilView(ID3D11Device* pD3DDevice,
		UINT Width, UINT Height,
		ID3D11DepthStencilView** ppDSView, ID3D11ShaderResourceView** ppSRView,
		UINT ArraySize = 1);

	// �[�x�X�e���V���r���[����V�F�[�_�[���\�[�X�r���[���쐬����
	void static CreateSRViewFromDSView(ID3D11Device* pD3DDevice, ID3D11DepthStencilView* pDSView, ID3D11ShaderResourceView** ppSRView);

	// �m�C�Y�}�b�v���쐬����
	void static CreateSRViewOfNoiseMap(ID3D11Device* pD3DDevice, UINT Width, UINT Height, DXGI_FORMAT format, ID3D11ShaderResourceView** ppSRView);
};

class Buffers
{
private:
	Buffers();
public:
	// �o�b�t�@���쐬���鋤�ʊ֐�
	ComPtr<ID3D11Buffer> static CreateBuffer(ID3D11Device* pD3DDevice, void* pData, size_t size, UINT CPUAccessFlag, D3D11_BIND_FLAG BindFlag, UINT miscFlag = 0U);

	// ���_�o�b�t�@���쐬����
	inline ComPtr<ID3D11Buffer> static CreateVertexBuffer(ID3D11Device* pD3DDevice, void* pData, size_t size, UINT CPUAccessFlag)
	{
		return CreateBuffer(pD3DDevice, pData, size, CPUAccessFlag, D3D11_BIND_VERTEX_BUFFER);
	}

	// �C���f�b�N�X�o�b�t�@���쐬����
	inline ComPtr<ID3D11Buffer> static CreateIndexBuffer(ID3D11Device* pD3DDevice, void* pData, size_t size, UINT CPUAccessFlag)
	{
		return CreateBuffer(pD3DDevice, pData, size, CPUAccessFlag, D3D11_BIND_INDEX_BUFFER);
	}
};

template <typename T>
class StructuredBuffer
{
public:
	StructuredBuffer(ID3D11Device* d3dDevice, int elements,
		UINT bindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		bool dynamic = false);

	ComPtr<ID3D11Buffer> GetBuffer() { return mBuffer; }
	ComPtr<ID3D11UnorderedAccessView> GetUnorderedAccess() { return mUnorderedAccess; }
	ComPtr<ID3D11ShaderResourceView> GetShaderResource() { return mShaderResource; }

	T* MapDiscard(ID3D11DeviceContext* d3dDeviceContext);
	void Unmap(ID3D11DeviceContext* d3dDeviceContext);

private:
	StructuredBuffer(const StructuredBuffer&);
	StructuredBuffer& operator=(const StructuredBuffer&);

	int mElements;
	ComPtr<ID3D11Buffer> mBuffer;
	ComPtr<ID3D11ShaderResourceView> mShaderResource;
	ComPtr<ID3D11UnorderedAccessView> mUnorderedAccess;
};

template <typename T>
StructuredBuffer<T>::StructuredBuffer(ID3D11Device* d3dDevice, int elements,
	UINT bindFlags, bool dynamic)
	: mElements(elements), mShaderResource(0), mUnorderedAccess(0)
{
	CD3D11_BUFFER_DESC desc(sizeof(T) * elements, bindFlags,
		dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
		dynamic ? D3D11_CPU_ACCESS_WRITE : 0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof(T));

	d3dDevice->CreateBuffer(&desc, 0, &mBuffer);

	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS) {
		d3dDevice->CreateUnorderedAccessView(mBuffer.Get(), 0, &mUnorderedAccess);
	}

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE) {
		d3dDevice->CreateShaderResourceView(mBuffer.Get(), 0, &mShaderResource);
	}
}



template <typename T>
T* StructuredBuffer<T>::MapDiscard(ID3D11DeviceContext* d3dDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	d3dDeviceContext->Map(mBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	return static_cast<T*>(mappedResource.pData);
}


template <typename T>
void StructuredBuffer<T>::Unmap(ID3D11DeviceContext* d3dDeviceContext)
{
	d3dDeviceContext->Unmap(mBuffer.Get(), 0);
}

class Samplers
{
private:
	Samplers();
public:
	// �T���v���[�X�e�[�g���쐬����
	ComPtr<ID3D11SamplerState> static CreateSamplerState(ID3D11Device* pD3DDevice,
		D3D11_TEXTURE_ADDRESS_MODE TextureAddressMode,
		D3D11_FILTER Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_COMPARISON_FUNC = D3D11_COMPARISON_ALWAYS);
};

class Direct3D11
{
private:
	BOOL m_UseDepthBuffer, m_UseMultiSample;
	
	ComPtr<IDXGIAdapter> pAdapter;							// �A�_�v�^�[
	ComPtr<ID3D11RenderTargetView>	pRenderTargetView;		// �����_�����O�^�[�Q�b�g�r���[
	ComPtr<ID3D11DepthStencilView>	pDepthStencilView;		// �f�v�X�X�e���V���r���[
	ComPtr<IDXGISwapChain>			pSwapChain;				// �X���b�v�`�F�[��
public:
	HWND hWnd;												// �E�B���h�E�n���h��
	ComPtr<ID3D11Device> pD3DDevice;						// �f�o�C�X
	ComPtr<ID3D11DeviceContext> pD3DDeviceContext;			// �f�o�C�X�R���e�L�X�g
protected:
	//�V���O���g���Ȃ̂ŃR���X�g���N�^���v���g�^�C�v�錾���܂�
	Direct3D11();
private:
	// �E�B���h�E�쐬
	void InitWindow(TCHAR* AppName, HINSTANCE hInstance, WNDPROC WndProc, DXGI_MODE_DESC* pDisplayMode);

	// �A�_�v�^�[���擾
	void CreateAdapter();

	// �f�o�C�X�쐬
	void CreateDevice();

	// �X���b�v�`�F�[���̍쐬
	void CreateSwapChain(DXGI_MODE_DESC* pDisplayMode);

	// �����_�����O�^�[�Q�b�g�r���[�̍쐬
	void CreateRenderTargetView();

	// �[�x�X�e���V���r���[�̍쐬
	void CreateDepthStencilView();

	// �r���[�|�[�g�̍쐬
	void CreateViewport();

	// �E�B���h�E�A�\�V�G�[�V����
	void SetWindowAssociation();

	// �f�t�H���g�̃��X�^���C�U��ݒ�
	void SetDefaultRasterize();

	// �f�t�H���g�̐[�x�X�e���V���X�e�[�g��ݒ�
	void SetDefaultDepthStencilState();
public:
	virtual ~Direct3D11();

	// �V���O���g���I�u�W�F�N�g���擾
	static Direct3D11 &Instance()
	{
		static Direct3D11 inst;
		return inst;
	}

	// �f�B�X�v���C���[�h�ꗗ���쐬
	void GetDisplayMode(DXGI_MODE_DESC* pModeDesc, UINT* pNum);

	// Direct3D�쐬
	void CreateDirect3D11(TCHAR* AppName, HINSTANCE hInstance, WNDPROC WndProc, DXGI_MODE_DESC* pDisplayMode,
		BOOL UseMultisample, BOOL UseDepthBuffer);

	inline void ClearBackBuffer(const FLOAT ColorRGBA[4])
	{
		pD3DDeviceContext->ClearRenderTargetView(pRenderTargetView.Get(), ColorRGBA);
	}
	inline void ClearDepthStencilView(UINT ClearFlags, FLOAT Depth, UINT8 Stencil)
	{
		if (pDepthStencilView) pD3DDeviceContext->ClearDepthStencilView(pDepthStencilView.Get(), ClearFlags, Depth, Stencil);
	}

	HRESULT Present(UINT SyncInterval, UINT Flags);

	void SetHWND(HWND hwnd)
	{
		hWnd = hwnd;
	}

	HWND& GetHWND()
	{
		return hWnd;
	}
};
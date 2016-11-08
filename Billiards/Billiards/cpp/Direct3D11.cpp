#include "../Header/Direct3D11.h"

bool Maps::IsBitMask(DDS_HEADER* pHeader, DWORD r, DWORD g, DWORD b, DWORD a)
{
	return pHeader->ddspf.dwRBitMask == r && pHeader->ddspf.dwGBitMask == g && pHeader->ddspf.dwBBitMask == b && pHeader->ddspf.dwABitMask == a;
}

void Maps::CreateSRViewFromWICFile(ID3D11Device* pD3DDevice, const WCHAR* szFileName, ID3D11ShaderResourceView** ppSRV)
{
	HRESULT hr = E_FAIL;

	TexMetadata metadata;
	ScratchImage image;

	hr = LoadFromWICFile(szFileName, 0, &metadata, image);
	hr = CreateShaderResourceView(pD3DDevice, image.GetImages(), image.GetImageCount(), metadata, ppSRV);
}

void Maps::CreateSRViewFromWICFileArray(ID3D11Device* pD3DDevice, WCHAR* szFileName[], UINT NumTextures, ID3D11ShaderResourceView** ppSRV)
{
	HRESULT hr = E_FAIL;

	shared_ptr<TexMetadata> metadata(NEW TexMetadata[NumTextures], default_delete<TexMetadata[]>());
	shared_ptr<ScratchImage> image(NEW ScratchImage[NumTextures], default_delete<ScratchImage[]>());

	for (UINT i = 0; i<NumTextures; i++)
	{
		hr = LoadFromWICFile(szFileName[i], 0, &metadata.get()[i], image.get()[i]);
	}

	hr = CreateShaderResourceView(pD3DDevice, image->GetImages(), image->GetImageCount(), *metadata, ppSRV);
}

void Maps::CreateSRViewFromDDSFile(ID3D11Device* pD3DDevice, const WCHAR* szFileName, ID3D11ShaderResourceView** ppSRV)
{
	HRESULT hr = E_FAIL;
	HANDLE hFile = nullptr;
	unique_ptr<BYTE> pHeapData = nullptr;
	ComPtr<ID3D11Texture2D> pTex2D = nullptr;


	// �t�@�C�����J��
	hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		TCHAR errstr[1024];
		_stprintf_s(errstr, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B�t�@�C�����J���܂���B -> %s"), szFileName);
		throw(UException(HRESULT_FROM_WIN32(GetLastError()), errstr));
	}

	// �t�@�C���̃T�C�Y���擾����
	LARGE_INTEGER FileSize = { 0 };
	GetFileSizeEx(hFile, &FileSize);

	// 32�r�b�g�����Ȃ������𒴂���T�C�Y�̓G���[�Ƃ��ď�������
	if (FileSize.HighPart > 0)
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B32�r�b�g�����Ȃ������𒴂��Ă��܂��B")));

	// �t�@�C���T�C�Y���w�b�_�[+�}�W�b�N�i���o�[�ɖ����Ȃ�
	if (FileSize.LowPart < (sizeof(DDS_HEADER) + sizeof(DWORD)))
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B�t�@�C���T�C�Y���w�b�_�[+�}�W�b�N�i���o�[�ɖ����Ȃ��B")));

	// �w�b�_�[�̈�m��
	pHeapData.reset(NEW BYTE[FileSize.LowPart]);

	DWORD BytesRead = 0;

	// �t�@�C����ǂ�
	if (!ReadFile(hFile, pHeapData.get(), FileSize.LowPart, &BytesRead, nullptr))
		throw(UException(HRESULT_FROM_WIN32(GetLastError()), _T("LoadTextureDataFromFile()�ŃG���[���������܂����B�t�@�C����ǂ߂܂���B")));

	// �ǂݍ��񂾃f�[�^�T�C�Y���s���̂��߃G���[
	if (BytesRead < FileSize.LowPart)
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B�ǂݍ��񂾃f�[�^�T�C�Y���s���B")));

	// �t�@�C���̐擪�ɂ���}�W�b�N�i���o�[�� 'DDS' �ȊO�̂Ƃ��̓t�@�C���t�H�[�}�b�g���قȂ邽�߃G���[
	DWORD dwMagicNumber = *(DWORD*)(pHeapData.get());
	if (dwMagicNumber != DDS_MAGIC)
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B�}�W�b�N�i���o�[�� 'DDS' �ȊO�B")));

	// �w�b�_�[�̈�̃|�C���^���擾
	DDS_HEADER* pHeader = reinterpret_cast<DDS_HEADER*>(pHeapData.get() + sizeof(DWORD));

	// �T�C�Y�`�F�b�N
	if (pHeader->dwSize != sizeof(DDS_HEADER) || pHeader->ddspf.dwSize != sizeof(DDS_PIXELFORMAT))
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B�w�b�_�̈�ɐݒ肳��Ă���T�C�Y���s���B")));

	// RGB�܂���RGBA �̔񈳏k�t�H�[�}�b�g�ȊO�͖��Ή�
	if (!(pHeader->ddspf.dwFlags & DDS_RGB))
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����BRGB�܂���RGBA�t�H�[�}�b�g�ȊO�͖��Ή��B")));

	if (pHeader->dwHeaderFlags & DDS_HEADER_FLAGS_VOLUME)
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B�{�����[���e�N�X�`���[�͖��Ή��B")));

	if (pHeader->dwCubemapFlags != 0)
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B�L���[�u�}�b�v�͖��Ή��B")));

	// �}�W�b�N�i���o�[ + �w�b�_�[�T�C�Y
	INT offset = sizeof(DWORD) + sizeof(DDS_HEADER);
	// �f�[�^�̈�̐擪�|�C���^���擾
	BYTE* pBitData = pHeapData.get() + offset;

	UINT iWidth = pHeader->dwWidth;
	UINT iHeight = pHeader->dwHeight;
	UINT iMipCount = pHeader->dwMipMapCount;
	if (iMipCount == 0)
		iMipCount = 1;
	else if (iMipCount > 1)
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B�~�b�v�}�b�v�͖��Ή��B")));

	D3D11_TEXTURE2D_DESC desc = { 0 };

	desc.ArraySize = 1;

	if (pHeader->ddspf.dwRGBBitCount != 32)
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B32bit�t�H�[�}�b�g�̂ݑΉ��B")));

	// R8G8B8A8�܂���R8G8B8X8�̂݃T�|�[�g����
	if (IsBitMask(pHeader, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	else if (IsBitMask(pHeader, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	else
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B�s���ȃt�H�[�}�b�g�B")));

	desc.Width = iWidth;
	desc.Height = iHeight;
	desc.MipLevels = iMipCount;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;

	// �Ŋ��̃o�C�g�ɐ؂�グ
	UINT RowBytes = (iWidth * 32 + 7) / 8;

	// �f�[�^�Z�b�g
	InitData.pSysMem = (void*)pBitData;
	// 1�s������̃o�C�g��
	InitData.SysMemPitch = RowBytes;

	if (FAILED(hr = pD3DDevice->CreateTexture2D(&desc, &InitData, &pTex2D)))
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B�e�N�X�`���[�̍쐬�Ɏ��s�����B")));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = desc.MipLevels;
	if (FAILED(hr = pD3DDevice->CreateShaderResourceView(pTex2D.Get(), &SRVDesc, ppSRV)))
		throw(UException(-1, _T("LoadTextureDataFromFile()�ŃG���[���������܂����B�V�F�[�_�[���\�[�X�r���[�̍쐬�Ɏ��s�����B")));

	if (hFile)
		CloseHandle(hFile);
}

// �����_�[�^�[�Q�b�g�r���[���쐬����
void Maps::CreateRenderTargetView(ID3D11Device* pD3DDevice,
	DXGI_FORMAT format, UINT Width, UINT Height,
	ID3D11RenderTargetView** ppRTView, ID3D11ShaderResourceView** ppSRView,
	UINT ArraySize,
	UINT MipmapCount)
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11Texture2D> pTex2D = nullptr;


	D3D11_TEXTURE2D_DESC Tex2DDesc;
	ZeroMemory(&Tex2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	Tex2DDesc.ArraySize = ArraySize;
	Tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	if (ppSRView) Tex2DDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Tex2DDesc.Format = format;
	Tex2DDesc.Width = Width;
	Tex2DDesc.Height = Height;
	Tex2DDesc.MipLevels = MipmapCount;    // �ő�~�b�v�}�b�v���B0�̂Ƃ��͊��S�ȃ~�b�v�}�b�v�`�F�[�����쐬����
	Tex2DDesc.SampleDesc.Count = 1;
	// 1�ȊO�̏ꍇ��ID3D11DeviceContext::GenerateMips()�ɂ��~�b�v�}�b�v�`�F�[�����쐬���邽�߂̃t���O��ݒ肷��
	if (MipmapCount != 1)
		Tex2DDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	if (FAILED(hr = pD3DDevice->CreateTexture2D(&Tex2DDesc, nullptr, &pTex2D)))
		throw(UException(hr, _T("CreateRenderTargetView()�ŃG���[���������܂����B�e�N�X�`���[���쐬�ł��܂���B")));

	if (FAILED(hr = pD3DDevice->CreateRenderTargetView(pTex2D.Get(), nullptr, ppRTView)))
		throw(UException(hr, _T("CreateRenderTargetView()�ŃG���[���������܂����B�����_�[�^�[�Q�b�g�r���[���쐬�ł��܂���B")));

	if (ppSRView != nullptr)
		Maps::CreateSRViewFromRTView(pD3DDevice, *ppRTView, ppSRView);
}

// �{�����[���e�N�X�`���[�p�̃����_�[�^�[�Q�b�g�r���[���쐬����
void Maps::CreateRenderTargetViewOfVolumeTexture(ID3D11Device* pD3DDevice,
	DXGI_FORMAT format, UINT Width, UINT Height, UINT Depth,
	ID3D11RenderTargetView** ppRTView, ID3D11ShaderResourceView** ppSRView)
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11Texture3D> pTex3D = nullptr;


	D3D11_TEXTURE3D_DESC Tex3DDesc;
	ZeroMemory(&Tex3DDesc, sizeof(D3D11_TEXTURE3D_DESC));
	Tex3DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	if (ppSRView) Tex3DDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	Tex3DDesc.Usage = D3D11_USAGE_DEFAULT;
	Tex3DDesc.Format = format;
	Tex3DDesc.Width = Width;
	Tex3DDesc.Height = Height;
	Tex3DDesc.Depth = Depth;
	Tex3DDesc.MipLevels = 1;
	if (FAILED(hr = pD3DDevice->CreateTexture3D(&Tex3DDesc, nullptr, &pTex3D)))
		throw(UException(hr, _T("CreateRenderTargetViewOfVolumeTexture()�ŃG���[���������܂����B�e�N�X�`���[���쐬�ł��܂���B")));

	if (FAILED(hr = pD3DDevice->CreateRenderTargetView(pTex3D.Get(), nullptr, ppRTView)))
		throw(UException(hr, _T("CreateRenderTargetViewOfVolumeTexture()�ŃG���[���������܂����B�����_�[�^�[�Q�b�g�r���[���쐬�ł��܂���B")));

	if (ppSRView != nullptr)
		Maps::CreateSRViewFromRTView(pD3DDevice, *ppRTView, ppSRView);
}

// �����_�[�^�[�Q�b�g�r���[����V�F�[�_�[���\�[�X�r���[���쐬����
void Maps::CreateSRViewFromRTView(ID3D11Device* pD3DDevice, ID3D11RenderTargetView* pRTView, ID3D11ShaderResourceView** ppSRView)
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11Resource> pResource = nullptr;


	// �����_�[�^�[�Q�b�g�r���[����e�N�X�`���[���擾����
	pRTView->GetResource(&pResource);

	// �V�F�[�_�[���\�[�X�r���[���쐬����
	if (FAILED(hr = pD3DDevice->CreateShaderResourceView(pResource.Get(), nullptr, ppSRView)))
		throw(UException(hr, _T("CreateSRViewFromRTView()�ŃG���[���������܂����B�V�F�[�_�[���\�[�X�r���[���쐬�ł��܂���B")));
}

// �A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
void Maps::CreateRenderTargetViewOfRWTexture(ID3D11Device* pD3DDevice,
	DXGI_FORMAT format, UINT Width, UINT Height,
	ID3D11UnorderedAccessView** ppUAView, ID3D11ShaderResourceView** ppSRView,
	UINT ArraySize)
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11Texture2D> pTex2D = nullptr;


	D3D11_TEXTURE2D_DESC Tex2DDesc;
	ZeroMemory(&Tex2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	Tex2DDesc.ArraySize = ArraySize;
	Tex2DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	if (ppSRView) Tex2DDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Tex2DDesc.Format = format;
	Tex2DDesc.Width = Width;
	Tex2DDesc.Height = Height;
	Tex2DDesc.MipLevels = 1;
	Tex2DDesc.SampleDesc.Count = 1;
	if (FAILED(hr = pD3DDevice->CreateTexture2D(&Tex2DDesc, nullptr, &pTex2D)))
		throw(UException(hr, _T("CreateRenderTargetViewOfRWTexture()�ŃG���[���������܂����B�e�N�X�`���[���쐬�ł��܂���B")));

	if (FAILED(hr = pD3DDevice->CreateUnorderedAccessView(pTex2D.Get(), nullptr, ppUAView)))
		throw(UException(hr, _T("CreateRenderTargetViewOfRWTexture()�ŃG���[���������܂����B�A���I�[�_�[�h�A�N�Z�X�r���[���쐬�ł��܂���B")));

	if (ppSRView != nullptr)
		Maps::CreateSRViewFromUAView(pD3DDevice, *ppUAView, ppSRView);
}

// �A���I�[�_�[�h�A�N�Z�X�r���[����V�F�[�_�[���\�[�X�r���[���쐬����
void Maps::CreateSRViewFromUAView(ID3D11Device* pD3DDevice, ID3D11UnorderedAccessView* pUAView, ID3D11ShaderResourceView** ppSRView)
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11Resource> pResource = nullptr;


	// �A���I�[�_�[�h�A�N�Z�X�r���[����e�N�X�`���[���擾����
	pUAView->GetResource(&pResource);

	// �V�F�[�_�[���\�[�X�r���[���쐬����
	if (FAILED(hr = pD3DDevice->CreateShaderResourceView(pResource.Get(), nullptr, ppSRView)))
		throw(UException(hr, _T("CreateSRViewFromUAView()�ŃG���[���������܂����B�V�F�[�_�[���\�[�X�r���[���쐬�ł��܂���B")));
}

// �[�x�X�e���V���r���[���쐬����
void Maps::CreateDepthStencilView(ID3D11Device* pD3DDevice,
	UINT Width, UINT Height,
	ID3D11DepthStencilView** ppDSView, ID3D11ShaderResourceView** ppSRView,
	UINT ArraySize)
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11Texture2D> pTex2D = nullptr;


	D3D11_TEXTURE2D_DESC Tex2DDesc;
	ZeroMemory(&Tex2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	Tex2DDesc.ArraySize = ArraySize;
	Tex2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	if (ppSRView) Tex2DDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Tex2DDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	Tex2DDesc.Width = Width;
	Tex2DDesc.Height = Height;
	Tex2DDesc.MipLevels = 1;
	Tex2DDesc.SampleDesc.Count = 1;

	// �[�x�o�b�t�@�p�̃e�N�X�`���[�쐬
	hr = pD3DDevice->CreateTexture2D(&Tex2DDesc, nullptr, &pTex2D);
	if (FAILED(hr))
		throw(UException(hr, _T("CreateDepthStencilView()�ŃG���[���������܂����B�[�x�o�b�t�@�p�̃e�N�X�`���[�쐬�G���[�ł��B")));

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;

	ZeroMemory(&DSVDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	// �e�N�X�`���[�쐬���Ɏw�肵���t�H�[�}�b�g�ƌ݊���������A�[�x�X�e���V���r���[�Ƃ��Ďw��ł���t�H�[�}�b�g���w�肷��
	switch (Tex2DDesc.Format)
	{
	case DXGI_FORMAT_R16_TYPELESS:
		DSVDesc.Format = DXGI_FORMAT_D16_UNORM;
		break;
	case DXGI_FORMAT_R32_TYPELESS:
		DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
		DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	default:
		DSVDesc.Format = DSVDesc.Format;
	}

	switch (ArraySize)
	{
		// �z��Ȃ�
	case 1:
		DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		DSVDesc.Texture2D.MipSlice = 0;
		break;
		// �z�񂠂�
	default:
		DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		DSVDesc.Texture2DArray.ArraySize = ArraySize;
		DSVDesc.Texture2DArray.MipSlice = 0;
		DSVDesc.Texture2DArray.FirstArraySlice = 0;
		break;
	}
	// �[�x�X�e���V���r���[���쐬
	hr = pD3DDevice->CreateDepthStencilView(pTex2D.Get(), &DSVDesc, ppDSView);
	if (FAILED(hr))
		throw(UException(hr, _T("CreateDepthStencilView()�ŃG���[���������܂����B�[�x�X�e���V���r���[���쐬�ł��܂���B")));

	if (ppSRView != nullptr)
		Maps::CreateSRViewFromDSView(pD3DDevice, *ppDSView, ppSRView);
}

// �[�x�X�e���V���r���[����V�F�[�_�[���\�[�X�r���[���쐬����
void Maps::CreateSRViewFromDSView(ID3D11Device* pD3DDevice, ID3D11DepthStencilView* pDSView, ID3D11ShaderResourceView** ppSRView)
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11Resource> pResource = nullptr;


	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	pDSView->GetDesc(&DSVDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	switch (DSVDesc.ViewDimension)
	{
	case D3D11_DSV_DIMENSION_TEXTURE2D:
		SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
		SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
		SRVDesc.Texture2DArray.ArraySize = DSVDesc.Texture2DArray.ArraySize;
		SRVDesc.Texture2DArray.MipLevels = 1;
		break;
	}

	switch (DSVDesc.Format)
	{
	case DXGI_FORMAT_D16_UNORM:
		SRVDesc.Format = DXGI_FORMAT_R16_FLOAT;
		break;
	case DXGI_FORMAT_D32_FLOAT:
		SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
		break;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	default:
		SRVDesc.Format = DSVDesc.Format;
	}

	// �[�x�X�e���V���r���[����e�N�X�`���[���擾����
	pDSView->GetResource(&pResource);

	// �V�F�[�_�[���\�[�X�r���[���쐬����
	if (FAILED(hr = pD3DDevice->CreateShaderResourceView(pResource.Get(), &SRVDesc, ppSRView)))
		throw(UException(hr, _T("CreateSRViewFromDSView()�ŃG���[���������܂����B�V�F�[�_�[���\�[�X�r���[���쐬�ł��܂���B")));
}

// �m�C�Y�}�b�v���쐬����
void Maps::CreateSRViewOfNoiseMap(ID3D11Device* pD3DDevice, UINT Width, UINT Height, DXGI_FORMAT format, ID3D11ShaderResourceView** ppSRView)
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11Texture2D> pTexture2D = nullptr;
	unique_ptr<float> pBits = nullptr;
	DWORD BitsCount = 0;


	D3D11_SUBRESOURCE_DATA SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	switch (format)
	{
	case DXGI_FORMAT_R32_FLOAT:
		BitsCount = Width * Height;
		pBits.reset(NEW float[BitsCount]);
		for (DWORD i = 0; i<BitsCount; i++)
			pBits.get()[i] = (float)(rand() % 255) / 254.0f;
		SubResourceData.pSysMem = (LPVOID)pBits.get();
		SubResourceData.SysMemPitch = Width * sizeof(float);
		break;

	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		BitsCount = Width * Height * 4;
		pBits.reset(NEW float[BitsCount]);
		for (DWORD i = 0; i<BitsCount; i++)
			pBits.get()[i] = (float)(rand() % 255) / 254.0f;
		SubResourceData.pSysMem = (LPVOID)pBits.get();
		SubResourceData.SysMemPitch = Width * sizeof(float) * 4;
		break;
	default:
		throw(UException(hr, _T("CreateSRViewOfNoiseMap()�ŃG���[���������܂����B���Ή��̃t�H�[�}�b�g���w�肳��܂����B")));
		break;
	}

	D3D11_TEXTURE2D_DESC Tex2DDesc;
	ZeroMemory(&Tex2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	Tex2DDesc.ArraySize = 1;
	Tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Tex2DDesc.Format = format;
	Tex2DDesc.Width = Width;
	Tex2DDesc.Height = Height;
	Tex2DDesc.MipLevels = 1;
	Tex2DDesc.SampleDesc.Count = 1;

	// �e�N�X�`���[�쐬
	if (FAILED(hr = pD3DDevice->CreateTexture2D(&Tex2DDesc, &SubResourceData, &pTexture2D)))
		throw(UException(hr, _T("CreateSRViewOfNoiseMap()�ŃG���[���������܂����B�e�N�X�`���[�̍쐬�Ɏ��s���܂���")));

	// �V�F�[�_�[���\�[�X�r���[���쐬
	if (FAILED(hr = pD3DDevice->CreateShaderResourceView(pTexture2D.Get(), nullptr, ppSRView)))
		throw(UException(hr, _T("CreateSRViewOfNoiseMap()�ŃG���[���������܂����B�V�F�[�_�[���\�[�X�r���[�̍쐬�Ɏ��s���܂���")));
}

// ���_�A�C���f�b�N�X�o�b�t�@���쐬����
ComPtr<ID3D11Buffer> Buffers::CreateBuffer(ID3D11Device* pD3DDevice, void* pData, size_t size, UINT CPUAccessFlag, D3D11_BIND_FLAG BindFlag, UINT miscFlag)
{
	HRESULT hr = E_FAIL;

	ComPtr<ID3D11Buffer> pBuffer = nullptr;

	// �o�b�t�@�[ ���\�[�X�B
	D3D11_BUFFER_DESC BufferDesc = { 0 };

	// �T�u���\�[�X
	unique_ptr<D3D11_SUBRESOURCE_DATA> resource = nullptr;

	D3D11_USAGE Usage = D3D11_USAGE_DEFAULT;
	UINT CPUAccessFlags = 0;


	switch (CPUAccessFlag)
	{
		// CPU�A�N�Z�X�������Ȃ�
	case 0:
		Usage = D3D11_USAGE_DEFAULT;
		CPUAccessFlags = CPUAccessFlag;
		break;
		// CPU�A�N�Z�X��������
	default:
		Usage = D3D11_USAGE_DYNAMIC;
		CPUAccessFlags = CPUAccessFlag;
		break;
	}

	// �����l��ݒ肷��
	if (pData)
	{
		resource = make_unique<D3D11_SUBRESOURCE_DATA>();
		resource->pSysMem = pData;
		resource->SysMemPitch = 0;
		resource->SysMemSlicePitch = 0;
	}

	// �o�b�t�@�̐ݒ�
	BufferDesc.ByteWidth = size;                      // �o�b�t�@�T�C�Y
	BufferDesc.Usage = Usage;                     // ���\�[�X�g�p�@����肷��
	switch (BindFlag)
	{
	case D3D11_BIND_STREAM_OUTPUT:
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
		break;
	default:
		BufferDesc.BindFlags = BindFlag;
		break;
	}
	BufferDesc.CPUAccessFlags = CPUAccessFlags;            // CPU �A�N�Z�X
	BufferDesc.MiscFlags = miscFlag;                         // ���̑��̃t���O���ݒ肵�Ȃ�

	if (miscFlag == D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		BufferDesc.StructureByteStride = size;

	// �o�b�t�@���쐬����
	hr = pD3DDevice->CreateBuffer(&BufferDesc, resource.get(), &pBuffer);
	if (FAILED(hr))
		throw(UException(hr, _T("CreateBuffer()�ŃG���[���������܂����B�o�b�t�@�쐬�G���[�ł��B")));


	return pBuffer;
}

//template <typename T>
//StructuredBuffer<T>::StructuredBuffer(ID3D11Device* d3dDevice, int elements,
//	UINT bindFlags, bool dynamic)
//	: mElements(elements), mShaderResource(0), mUnorderedAccess(0)
//{
//	CD3D11_BUFFER_DESC desc(sizeof(T) * elements, bindFlags,
//		dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
//		dynamic ? D3D11_CPU_ACCESS_WRITE : 0,
//		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
//		sizeof(T));
//
//	d3dDevice->CreateBuffer(&desc, 0, &mBuffer);
//
//	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS) {
//		d3dDevice->CreateUnorderedAccessView(mBuffer, 0, &mUnorderedAccess);
//	}
//
//	if (bindFlags & D3D11_BIND_SHADER_RESOURCE) {
//		d3dDevice->CreateShaderResourceView(mBuffer, 0, &mShaderResource);
//	}
//}
//
//
//template <typename T>
//T* StructuredBuffer<T>::MapDiscard(ID3D11DeviceContext* d3dDeviceContext)
//{
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	d3dDeviceContext->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//	return static_cast<T*>(mappedResource.pData);
//}
//
//
//template <typename T>
//void StructuredBuffer<T>::Unmap(ID3D11DeviceContext* d3dDeviceContext)
//{
//	d3dDeviceContext->Unmap(mBuffer, 0);
//}

// �T���v���[�X�e�[�g���쐬����
ComPtr<ID3D11SamplerState> Samplers::CreateSamplerState(ID3D11Device* pD3DDevice, D3D11_TEXTURE_ADDRESS_MODE TextureAddressMode, D3D11_FILTER Filter, D3D11_COMPARISON_FUNC ComparisonFunc)
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11SamplerState> pSamplerState = nullptr;

	// �T���v���[�X�e�[�g���쐬����
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = Filter;                           // �T���v�����O���Ɏg�p����t�B���^�B
	samplerDesc.AddressU = TextureAddressMode;             // 0 �` 1 �͈̔͊O�ɂ��� u �e�N�X�`���[���W�̕`����@
	samplerDesc.AddressV = TextureAddressMode;             // 0 �` 1 �͈̔͊O�ɂ��� v �e�N�X�`���[���W
	samplerDesc.AddressW = TextureAddressMode;             // 0 �` 1 �͈̔͊O�ɂ��� w �e�N�X�`���[���W
	samplerDesc.MipLODBias = 0;                            // �v�Z���ꂽ�~�b�v�}�b�v ���x������̃o�C�A�X
	samplerDesc.MaxAnisotropy = 1;                         // �T���v�����O�Ɉٕ�����Ԃ��g�p���Ă���ꍇ�̌��E�l�B�L���Ȓl�� 1 �` 16 �B
	samplerDesc.ComparisonFunc = ComparisonFunc;           // ��r�I�v�V�����B
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;                                // �A�N�Z�X�\�ȃ~�b�v�}�b�v�̉����l
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;                // �A�N�Z�X�\�ȃ~�b�v�}�b�v�̏���l
	hr = pD3DDevice->CreateSamplerState(&samplerDesc, &pSamplerState);
	if (FAILED(hr))
		throw(UException(-1, _T("UIMesh;;CreateSamplerState()�ŃG���[���������܂����B�T���v���[�X�e�[�g���쐬�ł��܂���B")));

	return pSamplerState;
}

Direct3D11::Direct3D11()
{
	pD3DDevice = nullptr;
	pD3DDeviceContext = nullptr;
	pAdapter = nullptr;
	pSwapChain = nullptr;
	pRenderTargetView = nullptr;
	pDepthStencilView = nullptr;
	m_UseDepthBuffer = TRUE;
	m_UseMultiSample = TRUE;
}

Direct3D11::~Direct3D11()
{

}

// �f�t�H���g�f�B�X�v���C�̃��[�h��񋓂���
void Direct3D11::GetDisplayMode(DXGI_MODE_DESC* pModeDesc, UINT* pNum)
{
	HRESULT hr = E_FAIL;
	ComPtr<IDXGIOutput> pOutput = nullptr;


	// �A�_�v�^�[���擾�B
	CreateAdapter();

	// �A�_�v�^�[�ɐڑ�����Ă���v���C�}���f�B�X�v���C���擾�B
	hr = pAdapter->EnumOutputs(0, &pOutput);
	if (FAILED(hr))
		throw(UException(hr, _T("UDirect3D11::GetDisplayMode()�ŃG���[���������܂����B�A�_�v�^�[�ɐڑ�����Ă���v���C�}���f�B�X�v���C�̎擾�Ɏ��s���܂����B")));

	// �f�B�X�v���C���[�h�̈ꗗ���擾
	hr = pOutput->GetDisplayModeList(UD3D11_FORMAT, 0, pNum, pModeDesc);
	if (FAILED(hr))
		throw(UException(hr, _T("UDirect3D11::GetDisplayMode()�ŃG���[���������܂����B�f�B�X�v���C���[�h�̈ꗗ�̎擾�Ɏ��s���܂����B")));

	if (pModeDesc == nullptr)
	{
		// 0 ���Ԃ邱�Ƃ����邩�s�������O�̂���
		if (*pNum == 0)
			throw(UException(hr, _T("UDirect3D11::GetDisplayMode()�ŃG���[���������܂����B�f�B�X�v���C���[�h���P���擾�ł��܂���B")));
	}
}

// Direct3D�̏�����
void Direct3D11::CreateDirect3D11(TCHAR* AppName, HINSTANCE hInstance, WNDPROC WndProc, DXGI_MODE_DESC* pDisplayMode,
	BOOL UseMultisample, BOOL UseDepthBuffer)
{
	m_UseMultiSample = UseMultisample;
	m_UseDepthBuffer = UseDepthBuffer;

	// �E�B���h�E���쐬
	InitWindow(AppName, hInstance, WndProc, pDisplayMode);

	// �A�_�v�^�[���擾
	CreateAdapter();

	// �f�o�C�X�̍쐬
	CreateDevice();

	// �X���b�v�`�F�[���̍쐬
	CreateSwapChain(pDisplayMode);

	// �����_�[�^�[�Q�b�g�r���[�̍쐬
	CreateRenderTargetView();

	// �[�x�X�e���V���r���[�̍쐬
	CreateDepthStencilView();

	// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
	pD3DDeviceContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), pDepthStencilView.Get());

	// �r���[�|�[�g�쐬
	CreateViewport();

	// �\�����[�h�̎����؊����𖳌��ɂ���B
	// �����؊�����L���ɂ���ꍇ�͂��̊֐����R�����g�A�E�g����B
	// �������R�����g�A�E�g�����ꍇ�A�����N�����t���X�N���[�����[�h�ɂ����Ƃ����������삵�Ă���Ȃ��B
	SetWindowAssociation();

	// �f�t�H���g�̃��X�^���C�U��ݒ肷��
	SetDefaultRasterize();

	// �f�t�H���g�̐[�x�X�e���V���X�e�[�g��ݒ肷��
	SetDefaultDepthStencilState();

	//CreateSoundDevice();

	//// ���͊֘A�̏�����
	//DInput.Init((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE));// DirectInput8�̏�����
	///* Keyboard Object �̏����� */
	//if (!DInput.InitKeyboard(hWnd)) MSGBOX(_T("not found keyboard"), _T("DirectInput InitKeyboard"));
}

// �E�B���h�E�쐬
void Direct3D11::InitWindow(TCHAR* AppName, HINSTANCE hInstance, WNDPROC WndProc, DXGI_MODE_DESC* pDisplayMode)
{
	// �E�B���h�E���[�h
	DWORD WindowStyle = WS_BORDER | WS_CAPTION | WS_SYSMENU;

	// �E�B���h�E�N���X�o�^
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);					// WNDCLASSEX�\���̂̃T�C�Y
	wcex.style = CS_HREDRAW | CS_VREDRAW;				// �E�C���h�E�X�^�C��	CS_HREDRAW�i���������̃T�C�Y�ύX����������S�̂�`�������j�ƁA CS_VREDRAW�i���������̃T�C�Y�ύX�őS�̂�`�������j���w��
	wcex.lpfnWndProc = WndProc;							// �E�B���h�E�v���V�[�W���̃A�h���X
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;							// WinMain()�̃C���X�^���X�n���h��
	wcex.hIcon = nullptr;								// �A�C�R���ݒ�		LoadIcon()�ŃA�C�R�������[�h
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);		// �}�E�X�J�[�\��
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);	// �E�C���h�E�̃N���C�A���g�̈�̔w�i�F		HBRUSH�^�́A�u���V�̃n���h���f�t�H���g�̐F�ł���A(HBRUSH)(COLOR_WINDOW + 1)�Ƃ���
	wcex.lpszMenuName = nullptr;						// ���j���[
	wcex.lpszClassName = AppName;						// Window�N���X�̖��O
	wcex.hIconSm = nullptr;								// �g�p���鏬�����A�C�R��
	RegisterClassEx(&wcex);					// �E�B���h�E�N���X��o�^		�o�^�����N���X�́ACreateWindow �֐��܂��� CreateWindowEx �֐��Ŏg�p

	RECT rc;
	::SetRect(&rc, 0, 0, pDisplayMode->Width, pDisplayMode->Height);
	// �E�B���h�E���[�h�̂Ƃ��^�C�g���o�[���l�����ĕ\���̈��ݒ肷�邽�߂ɕK�v�ȃT�C�Y���擾����
	::AdjustWindowRect(&rc, WindowStyle, FALSE);

	// �E�B���h�E���쐬
	hWnd = CreateWindow(
		wcex.lpszClassName,		// �o�^����Ă���N���X��
		wcex.lpszClassName,		// �E�B���h�E��
		WindowStyle,			// �E�B���h�E�X�^�C��
		0,						// �E�B���h�E�̉������̈ʒu
		0,						// �E�B���h�E�̏c�����̈ʒu
		rc.right - rc.left,		// �E�B���h�E�̕�
		rc.bottom - rc.top,		// �E�B���h�E�̍���
		nullptr,				// �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
		nullptr,				// ���j���[�n���h���܂��͎q�E�B���h�E ID
		hInstance,				// �A�v���P�[�V�����C���X�^���X�̃n���h��
		nullptr					// �E�B���h�E�쐬�f�[�^
		);

	if (hWnd == nullptr)
		throw(UException(-1, _T("UDirect3D11::InitWindow()�ŃG���[���������܂����B�E�B���h�E���쐬�ł��܂���B")));
}

// �A�_�v�^�[���擾
void Direct3D11::CreateAdapter()
{
	HRESULT hr = E_FAIL;
	ComPtr<IDXGIFactory> pFactory = nullptr;

	if (pAdapter == nullptr)
	{
		// �t�@�N�g�����쐬����
		hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
		if (FAILED(hr))
			throw(UException(hr, _T("Direct3D11::CreateAdapter()�ŃG���[���������܂����B�t�@�N�g���̍쐬�G���[�ł��B")));

		// �f�t�H���g�A�_�v�^�[���擾
		hr = pFactory->EnumAdapters(0, &pAdapter);
		if (FAILED(hr))
			throw(UException(hr, _T("Direct3D11::CreateAdapter()�ŃG���[���������܂����B�f�t�H���g�A�_�v�^�[�̍쐬�G���[�ł��B")));
	}
}

// �f�o�C�X�̍쐬
void Direct3D11::CreateDevice()
{
	HRESULT hr;

	// �f�o�C�X�쐬���Ɏg�p����r�f�I�J�[�h�̐��\�ꗗ
	// Direct3D11.0��SM�@5.0�̂݃T�|�[�g����
	D3D_FEATURE_LEVEL featureLevel[] = {
		D3D_FEATURE_LEVEL_11_0,  // Direct3D11.0 SM 5.0
	};

	UINT createDeviceFlag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	// �f�o�b�O�R���p�C���̏ꍇ�A�f�o�b�O���C���[��L���ɂ���
	// �\�t�g�E�F�A���C���[
	createDeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL level;

	// �f�o�C�X�̍쐬
	hr = D3D11CreateDevice(
		pAdapter.Get(),				// �g�p����A�_�v�^�[��ݒ�Bnullptr�̏ꍇ�̓f�t�H���g�̃A�_�v�^�[�B
		D3D_DRIVER_TYPE_UNKNOWN,	// D3D_DRIVER_TYPE�̂����ꂩ�B�h���C�o�[�̎�ށBpAdapter�� nullptr �ȊO�̏ꍇ�́AD3D_DRIVER_TYPE_UNKNOWN���w�肷��B
		nullptr,					// �\�t�g�E�F�A���X�^���C�U����������DLL�ւ̃n���h���BD3D_DRIVER_TYPE �� D3D_DRIVER_TYPE_SOFTWARE �ɐݒ肵�Ă���ꍇ�� nullptr �ɂł��Ȃ��B
		createDeviceFlag,			// D3D11_CREATE_DEVICE_FLAG�̑g�ݍ��킹�B�f�o�C�X���쐬���Ɏg�p�����p�����[�^�B
		featureLevel,				// D3D_FEATURE_LEVEL�̃|�C���^
		1,							// D3D_FEATURE_LEVEL�z��̗v�f��
		D3D11_SDK_VERSION,			// DirectX SDK�̃o�[�W�����B���̒l�͌Œ�B
		&pD3DDevice,				// ���������ꂽ�f�o�C�X
		&level,						// �̗p���ꂽ�t�B�[�`���[���x��
		&pD3DDeviceContext			// ���������ꂽ�f�o�C�X�R���e�L�X�g
		);

	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::CreateDevice()�ŃG���[���������܂����B�f�o�C�X�̍쐬�G���[�ł��B")));

	// �A�_�v�^�[�[�̏����擾
	DXGI_ADAPTER_DESC desc;
	pAdapter->GetDesc(&desc);
}

// �X���b�v�`�F�[���̍쐬
void Direct3D11::CreateSwapChain(DXGI_MODE_DESC* pDisplayMode)
{
	HRESULT hr = E_FAIL;
	TCHAR s[256];
	ComPtr<IDXGIFactory> pFactory = nullptr;
	DXGI_SWAP_CHAIN_DESC sd;


	ZeroMemory(&sd, sizeof(sd));
	CopyMemory(&(sd.BufferDesc), pDisplayMode, sizeof(DXGI_MODE_DESC));

	sd.BufferDesc.Format = UD3D11_FORMAT;                            // �o�b�N�o�b�t�@�t�H�[�}�b�g
	// �o�b�N�o�b�t�@�̃T�[�t�F�X�g�p�@����� CPU �A�N�Z�X �I�v�V������\�� DXGI_USAGE �񋓌^�̃����o�[
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                // �o�b�N�o�b�t�@�Ƀ����_�����O�\�ɂ���B
	// �X���b�v �`�F�[���̃o�b�t�@�[����\���l�B
	sd.BufferCount = 1;
	// �o�̓E�B���h�E�ւ� HWND �n���h���B���̃����o�[�� nullptr �ɂ͂ł��Ȃ��B
	sd.OutputWindow = hWnd;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;	//1/60 = 60fps
	// �o�͂��E�B���h�E ���[�h�̏ꍇ�� TRUE�B����ȊO�̏ꍇ�� FALSE�B
	// �����N�����̕\�����[�h�̓E�B���h�E���[�h�ɌŒ�Ƃ���B
	sd.Windowed = TRUE;
	// �T�[�t�F�X�̃X���b�v������Ƀo�b�N�o�b�t�@�̓��e��ێ����邩�B
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                        // �o�b�N�o�b�t�@�̓��e��ێ����Ȃ��B
	// �X���b�v �`�F�[���̓���̃I�v�V�����B
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;               // �t���X�N���[�����[�h�ɐ؂�ւ���ƁA�A�v���P�[�V���� �E�B���h�E�̃T�C�Y�ɍ��킹�ĉ𑜓x���ύX�����B

	// �t�@�N�g���[���쐬����
	// CreateDXGIFactory�ō쐬�����t�@�N�g���[���g�p����Ǝ��s�ł��邪���[�j���O�G���[�ɂȂ�̂� IDXGIAdapter ����쐬����B
	hr = pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::CreateSwapChain()�ŃG���[���������܂����B�t�@�N�g���[�̎擾�G���[�ł��B")));

	// �}���`�T���v�����O�̐ݒ�
	// �}���`�T���v�����g�p���Ȃ��ꍇ�̐ݒ�
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	int startMultisample;

	// �T���v�����O�����ő�̂��̂��̗p����
	if (m_UseMultiSample == TRUE)
		startMultisample = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
	// �}���`�T���v���𖳌��ɂ���B
	else
		startMultisample = 1;

	UINT Quality = 0;
	for (int i = startMultisample; i >= 0; i--)
	{
		// �T�|�[�g����N�H���e�B���x���̍ő�l���擾����
		hr = pD3DDevice->CheckMultisampleQualityLevels(pDisplayMode->Format, (UINT)i, &Quality);
		if (FAILED(hr))
			throw(UException(hr, _T("Direct3D11::CreateSwapChain()�ŃG���[���������܂����B�T�|�[�g����N�H���e�B���x���̍ő�l�̎擾�G���[�ł��B")));

		// 0 �ȊO�̂Ƃ��t�H�[�}�b�g�ƃT���v�����O���̑g�ݍ��킹���T�|�[�g����
		// �I�����C���w���v�ł�CheckMultisampleQualityLevels�̖߂�l�� 0 �̂Ƃ��T�|�[�g����Ȃ��Ƃ�����Ă��邪
		// pNumQualityLevels �̖߂�l�� 0 ���Ŕ��f����B
		// Direct3D 10 �� �̃I�����C���w���v�ɂ͂���������Ă���̂ŁA�I�����C���w���v�̊ԈႢ�B
		if (Quality != 0)
		{
			sd.SampleDesc.Count = i;
			sd.SampleDesc.Quality = Quality - 1;

			// �X���b�v�`�F�[�����쐬����B
			hr = pFactory->CreateSwapChain(pD3DDevice.Get(), &sd, &pSwapChain);
			_stprintf_s(s, _T("�������}���`�T���v���FCount(%d) Quality(%d)"), sd.SampleDesc.Count, sd.SampleDesc.Quality);
			if (SUCCEEDED(hr))
			{
				if (sd.SampleDesc.Count == 1)
					m_UseMultiSample = FALSE;

				_tcscat_s(s, _T("�@�ˁ@OK\n"));
				OutputDebugString(s);
				break;
			}
			else
			{
				_tcscat_s(s, _T("�@�ˁ@NG\n"));
				OutputDebugString(s);
			}
		}
	}
	if (pSwapChain == nullptr)
		throw(UException(-1, _T("Direct3D11::CreateSwapChain()�ŃG���[���������܂����B�X���b�v�`�F�[�����쐬�ł��܂���ł���")));

	_stprintf_s(s, _T("�������f�B�X�v���C���[�h�F�𑜓x( %d : %d )�@���t���b�V�����[�g( %d / %d )\n"),
		pDisplayMode->Width, pDisplayMode->Height,
		pDisplayMode->RefreshRate.Denominator, pDisplayMode->RefreshRate.Numerator);
	OutputDebugString(s);

}

// �����_�����O�^�[�Q�b�g�r���[�̍쐬
void Direct3D11::CreateRenderTargetView()
{
	HRESULT hr = E_FAIL;
	// �X���b�v�`�F�C���ɗp�ӂ��ꂽ�o�b�t�@�i2D�e�N�X�`���j���擾
	ComPtr<ID3D11Texture2D> back_buff = nullptr;

	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buff);
	if (FAILED(hr)){
		throw(UException(hr, _T("Direct3D11::CreateRenderTargetView()�ŃG���[���������܂����B�X���b�v�`�F�[������o�b�N�o�b�t�@�̎擾�G���[�ł��B")));
	}

	hr = pD3DDevice->CreateRenderTargetView(back_buff.Get(), nullptr, pRenderTargetView.GetAddressOf());
	if (FAILED(hr)){
		throw(UException(hr, _T("Direct3D11::CreateRenderTargetView()�ŃG���[���������܂����B�����_�����O�^�[�Q�b�g�r���[�̍쐬�G���[�ł��B")));
	}
}

// �[�x�X�e���V���r���[�̍쐬
void Direct3D11::CreateDepthStencilView()
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
	ComPtr<ID3D11Texture2D> pDepthBuffer = nullptr;
	D3D11_TEXTURE2D_DESC descDS;
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	DXGI_SWAP_CHAIN_DESC chainDesc;

	if (m_UseDepthBuffer == FALSE)
		return;


	// �X���b�v�`�F�[���̐ݒ���擾����
	hr = pSwapChain->GetDesc(&chainDesc);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::CreateDepthStencilView()�ŃG���[���������܂����B�X���b�v�`�F�[���̐ݒ�̎擾�G���[�ł��B")));

	ZeroMemory(&descDS, sizeof(D3D11_TEXTURE2D_DESC));
	descDS.Width = chainDesc.BufferDesc.Width;   // �o�b�N�o�b�t�@�Ɠ����ɂ���B
	descDS.Height = chainDesc.BufferDesc.Height;  // �o�b�N�o�b�t�@�Ɠ����ɂ���B
	descDS.MipLevels = 1;                            // �~�b�v�}�b�v���쐬���Ȃ�
	descDS.ArraySize = 1;                            // �e�N�X�`���[�̔z��
	descDS.Format = UD3D11_DEPTH_FORMAT;          // �t�H�[�}�b�g
	descDS.SampleDesc.Count = chainDesc.SampleDesc.Count;   // �o�b�N�o�b�t�@�Ɠ����ɂ���B
	descDS.SampleDesc.Quality = chainDesc.SampleDesc.Quality; // �o�b�N�o�b�t�@�Ɠ����ɂ���B
	descDS.Usage = D3D11_USAGE_DEFAULT;          // GPU �ɂ��ǂݎ�肨��я������݃A�N�Z�X��K�v�Ƃ��郊�\�[�X�B
	descDS.BindFlags = D3D11_BIND_DEPTH_STENCIL |    // �[�x�X�e���V���o�b�t�@�Ƃ��č쐬����
		D3D11_BIND_SHADER_RESOURCE;   // �V�F�[�_�[���\�[�X�r���[�Ƃ��č쐬����
	descDS.CPUAccessFlags = 0;                            // CPU �A�N�Z�X���s�v�B
	descDS.MiscFlags = 0;                            // ���̑��̃t���O���ݒ肵�Ȃ��B
	// �[�x�o�b�t�@�p�̃e�N�X�`���[�쐬
	hr = pD3DDevice->CreateTexture2D(&descDS, nullptr, &pDepthBuffer);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::CreateDepthStencilView()�ŃG���[���������܂����B�[�x�o�b�t�@�p�̃e�N�X�`���[�쐬�G���[�ł��B")));

	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	// �e�N�X�`���[�쐬���Ɏw�肵���t�H�[�}�b�g�ƌ݊���������A�[�x�X�e���V���r���[�Ƃ��Ďw��ł���t�H�[�}�b�g���w�肷��
	switch (descDS.Format)
	{
		// 8�r�b�g�t�H�[�}�b�g�͎g�p�ł��Ȃ��H
	case DXGI_FORMAT_R8_TYPELESS:
		descDSV.Format = DXGI_FORMAT_R8_UNORM;
		break;
	case DXGI_FORMAT_R16_TYPELESS:
		descDSV.Format = DXGI_FORMAT_D16_UNORM;
		break;
	case DXGI_FORMAT_R32_TYPELESS:
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
		descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	default:
		descDSV.Format = descDS.Format;
	}

	// �}���`�T���v�����g�p���Ă���ꍇ
	if (chainDesc.SampleDesc.Count > 1)
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	// �}���`�T���v�����g�p���Ă��Ȃ��ꍇ
	else
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	descDSV.Texture2D.MipSlice = 0;

	// �[�x�X�e���V���r���[���쐬
	hr = pD3DDevice->CreateDepthStencilView(pDepthBuffer.Get(), &descDSV, &pDepthStencilView);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::CreateDepthStencilView()�ŃG���[���������܂����B�[�x�X�e���V���r���[�̍쐬�G���[�ł��B")));
}

// �r���[�|�[�g�̍쐬
void Direct3D11::CreateViewport()
{
	HRESULT hr = E_FAIL;

	DXGI_SWAP_CHAIN_DESC chainDesc;
	// �X���b�v�`�F�[���̐ݒ���擾����
	hr = pSwapChain->GetDesc(&chainDesc);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::CreateViewport()�ŃG���[���������܂����B�X���b�v�`�F�[���̐ݒ�̎擾�G���[�ł��B")));

	D3D11_VIEWPORT Viewport[1];
	Viewport[0].TopLeftX = 0;
	Viewport[0].TopLeftY = 0;
	Viewport[0].Width = (FLOAT)chainDesc.BufferDesc.Width;
	Viewport[0].Height = (FLOAT)chainDesc.BufferDesc.Height;
	Viewport[0].MinDepth = 0.0f;
	Viewport[0].MaxDepth = 1.0f;
	pD3DDeviceContext->RSSetViewports(1, Viewport);
}

// �E�B���h�E�A�\�V�G�[�V����
void Direct3D11::SetWindowAssociation()
{
	HRESULT hr = E_FAIL;
	ComPtr<IDXGIFactory> pFactory = nullptr;


	// �t�@�N�g���[���쐬����
	// CreateDXGIFactory�ō쐬�����t�@�N�g���[���g�p����Ǝ��s�ł��邪���[�j���O�G���[�ɂȂ�̂� IDXGIAdapter ����쐬����B
	hr = pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::SetWindowAssociation()�ŃG���[���������܂����B�t�@�N�g���[�̎擾�G���[�ł��B")));

	// �\�����[�h�̎����؊����𖳌��ɂ���B
	hr = pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::SetWindowAssociation()�ŃG���[���������܂����B�E�B���h�E�A�\�V�G�[�V�����̍쐬�G���[�ł��B")));
}

// �f�t�H���g�̃��X�^���C�U��ݒ�
void Direct3D11::SetDefaultRasterize()
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11RasterizerState> g_pRasterState = nullptr;
	D3D11_RASTERIZER_DESC rsState;


	rsState.FillMode = D3D11_FILL_SOLID;    // �|���S���ʕ`��
	rsState.CullMode = D3D11_CULL_BACK;     // ���ʂ�`�悵�Ȃ�
	rsState.FrontCounterClockwise = TRUE;   // �����v����\��
	rsState.DepthBias = 0;
	rsState.DepthBiasClamp = 0;
	rsState.SlopeScaledDepthBias = 0;
	rsState.DepthClipEnable = TRUE;
	rsState.ScissorEnable = FALSE;          // �V�U�[��`����

	// �X���b�v�`�F�[���̃}���`�T���v�����O�̐ݒ�ɂ��킹��
	DXGI_SWAP_CHAIN_DESC swapDesc;
	pSwapChain->GetDesc(&swapDesc);
	if (swapDesc.SampleDesc.Count != 1)
		rsState.MultisampleEnable = TRUE;
	else
		rsState.MultisampleEnable = FALSE;

	rsState.AntialiasedLineEnable = FALSE;
	hr = pD3DDevice->CreateRasterizerState(&rsState, &g_pRasterState);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::SetDefaultRasterize()�ŃG���[���������܂����B���X�^���C�U�X�e�[�g�̍쐬�G���[�ł��B")));

	pD3DDeviceContext->RSSetState(g_pRasterState.Get());
}

// �f�t�H���g�̐[�x�X�e���V���X�e�[�g��ݒ�
void Direct3D11::SetDefaultDepthStencilState()
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11DepthStencilState> pDepthStencilState = nullptr;
	D3D11_DEPTH_STENCIL_DESC dsState;


	ZeroMemory(&dsState, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsState.DepthEnable = TRUE;
	dsState.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsState.DepthFunc = D3D11_COMPARISON_LESS;
	dsState.StencilEnable = FALSE;

	hr = pD3DDevice->CreateDepthStencilState(&dsState, &pDepthStencilState);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::SetDefaultDepthStencilState()�ŃG���[���������܂����B�[�x�X�e���V���X�e�[�g�̍쐬�G���[�ł��B")));

	pD3DDeviceContext->OMSetDepthStencilState(pDepthStencilState.Get(), 0);
}

HRESULT Direct3D11::Present(UINT SyncInterval, UINT Flags)
{
	HRESULT hr = E_FAIL;
	if (FAILED(hr = pSwapChain->Present(SyncInterval, Flags)))
		throw(UException(hr, _T("Direct3D11::Present()�ŃG���[���������܂����B")));

	return hr;
}
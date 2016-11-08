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


	// ファイルを開く
	hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		TCHAR errstr[1024];
		_stprintf_s(errstr, _T("LoadTextureDataFromFile()でエラーが発生しました。ファイルを開けません。 -> %s"), szFileName);
		throw(UException(HRESULT_FROM_WIN32(GetLastError()), errstr));
	}

	// ファイルのサイズを取得する
	LARGE_INTEGER FileSize = { 0 };
	GetFileSizeEx(hFile, &FileSize);

	// 32ビット符号なし整数を超えるサイズはエラーとして処理する
	if (FileSize.HighPart > 0)
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。32ビット符号なし整数を超えています。")));

	// ファイルサイズがヘッダー+マジックナンバーに満たない
	if (FileSize.LowPart < (sizeof(DDS_HEADER) + sizeof(DWORD)))
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。ファイルサイズがヘッダー+マジックナンバーに満たない。")));

	// ヘッダー領域確保
	pHeapData.reset(NEW BYTE[FileSize.LowPart]);

	DWORD BytesRead = 0;

	// ファイルを読む
	if (!ReadFile(hFile, pHeapData.get(), FileSize.LowPart, &BytesRead, nullptr))
		throw(UException(HRESULT_FROM_WIN32(GetLastError()), _T("LoadTextureDataFromFile()でエラーが発生しました。ファイルを読めません。")));

	// 読み込んだデータサイズが不正のためエラー
	if (BytesRead < FileSize.LowPart)
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。読み込んだデータサイズが不正。")));

	// ファイルの先頭にあるマジックナンバーが 'DDS' 以外のときはファイルフォーマットが異なるためエラー
	DWORD dwMagicNumber = *(DWORD*)(pHeapData.get());
	if (dwMagicNumber != DDS_MAGIC)
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。マジックナンバーが 'DDS' 以外。")));

	// ヘッダー領域のポインタを取得
	DDS_HEADER* pHeader = reinterpret_cast<DDS_HEADER*>(pHeapData.get() + sizeof(DWORD));

	// サイズチェック
	if (pHeader->dwSize != sizeof(DDS_HEADER) || pHeader->ddspf.dwSize != sizeof(DDS_PIXELFORMAT))
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。ヘッダ領域に設定されているサイズが不正。")));

	// RGBまたはRGBA の非圧縮フォーマット以外は未対応
	if (!(pHeader->ddspf.dwFlags & DDS_RGB))
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。RGBまたはRGBAフォーマット以外は未対応。")));

	if (pHeader->dwHeaderFlags & DDS_HEADER_FLAGS_VOLUME)
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。ボリュームテクスチャーは未対応。")));

	if (pHeader->dwCubemapFlags != 0)
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。キューブマップは未対応。")));

	// マジックナンバー + ヘッダーサイズ
	INT offset = sizeof(DWORD) + sizeof(DDS_HEADER);
	// データ領域の先頭ポインタを取得
	BYTE* pBitData = pHeapData.get() + offset;

	UINT iWidth = pHeader->dwWidth;
	UINT iHeight = pHeader->dwHeight;
	UINT iMipCount = pHeader->dwMipMapCount;
	if (iMipCount == 0)
		iMipCount = 1;
	else if (iMipCount > 1)
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。ミップマップは未対応。")));

	D3D11_TEXTURE2D_DESC desc = { 0 };

	desc.ArraySize = 1;

	if (pHeader->ddspf.dwRGBBitCount != 32)
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。32bitフォーマットのみ対応。")));

	// R8G8B8A8またはR8G8B8X8のみサポートする
	if (IsBitMask(pHeader, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	else if (IsBitMask(pHeader, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	else
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。不明なフォーマット。")));

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

	// 最寄りのバイトに切り上げ
	UINT RowBytes = (iWidth * 32 + 7) / 8;

	// データセット
	InitData.pSysMem = (void*)pBitData;
	// 1行当たりのバイト数
	InitData.SysMemPitch = RowBytes;

	if (FAILED(hr = pD3DDevice->CreateTexture2D(&desc, &InitData, &pTex2D)))
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。テクスチャーの作成に失敗した。")));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = desc.MipLevels;
	if (FAILED(hr = pD3DDevice->CreateShaderResourceView(pTex2D.Get(), &SRVDesc, ppSRV)))
		throw(UException(-1, _T("LoadTextureDataFromFile()でエラーが発生しました。シェーダーリソースビューの作成に失敗した。")));

	if (hFile)
		CloseHandle(hFile);
}

// レンダーターゲットビューを作成する
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
	Tex2DDesc.MipLevels = MipmapCount;    // 最大ミップマップ数。0のときは完全なミップマップチェーンを作成する
	Tex2DDesc.SampleDesc.Count = 1;
	// 1以外の場合はID3D11DeviceContext::GenerateMips()によりミップマップチェーンを作成するためのフラグを設定する
	if (MipmapCount != 1)
		Tex2DDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	if (FAILED(hr = pD3DDevice->CreateTexture2D(&Tex2DDesc, nullptr, &pTex2D)))
		throw(UException(hr, _T("CreateRenderTargetView()でエラーが発生しました。テクスチャーが作成できません。")));

	if (FAILED(hr = pD3DDevice->CreateRenderTargetView(pTex2D.Get(), nullptr, ppRTView)))
		throw(UException(hr, _T("CreateRenderTargetView()でエラーが発生しました。レンダーターゲットビューが作成できません。")));

	if (ppSRView != nullptr)
		Maps::CreateSRViewFromRTView(pD3DDevice, *ppRTView, ppSRView);
}

// ボリュームテクスチャー用のレンダーターゲットビューを作成する
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
		throw(UException(hr, _T("CreateRenderTargetViewOfVolumeTexture()でエラーが発生しました。テクスチャーが作成できません。")));

	if (FAILED(hr = pD3DDevice->CreateRenderTargetView(pTex3D.Get(), nullptr, ppRTView)))
		throw(UException(hr, _T("CreateRenderTargetViewOfVolumeTexture()でエラーが発生しました。レンダーターゲットビューが作成できません。")));

	if (ppSRView != nullptr)
		Maps::CreateSRViewFromRTView(pD3DDevice, *ppRTView, ppSRView);
}

// レンダーターゲットビューからシェーダーリソースビューを作成する
void Maps::CreateSRViewFromRTView(ID3D11Device* pD3DDevice, ID3D11RenderTargetView* pRTView, ID3D11ShaderResourceView** ppSRView)
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11Resource> pResource = nullptr;


	// レンダーターゲットビューからテクスチャーを取得する
	pRTView->GetResource(&pResource);

	// シェーダーリソースビューを作成する
	if (FAILED(hr = pD3DDevice->CreateShaderResourceView(pResource.Get(), nullptr, ppSRView)))
		throw(UException(hr, _T("CreateSRViewFromRTView()でエラーが発生しました。シェーダーリソースビューが作成できません。")));
}

// アンオーダードアクセスビューを作成する
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
		throw(UException(hr, _T("CreateRenderTargetViewOfRWTexture()でエラーが発生しました。テクスチャーが作成できません。")));

	if (FAILED(hr = pD3DDevice->CreateUnorderedAccessView(pTex2D.Get(), nullptr, ppUAView)))
		throw(UException(hr, _T("CreateRenderTargetViewOfRWTexture()でエラーが発生しました。アンオーダードアクセスビューが作成できません。")));

	if (ppSRView != nullptr)
		Maps::CreateSRViewFromUAView(pD3DDevice, *ppUAView, ppSRView);
}

// アンオーダードアクセスビューからシェーダーリソースビューを作成する
void Maps::CreateSRViewFromUAView(ID3D11Device* pD3DDevice, ID3D11UnorderedAccessView* pUAView, ID3D11ShaderResourceView** ppSRView)
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11Resource> pResource = nullptr;


	// アンオーダードアクセスビューからテクスチャーを取得する
	pUAView->GetResource(&pResource);

	// シェーダーリソースビューを作成する
	if (FAILED(hr = pD3DDevice->CreateShaderResourceView(pResource.Get(), nullptr, ppSRView)))
		throw(UException(hr, _T("CreateSRViewFromUAView()でエラーが発生しました。シェーダーリソースビューが作成できません。")));
}

// 深度ステンシルビューを作成する
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

	// 深度バッファ用のテクスチャー作成
	hr = pD3DDevice->CreateTexture2D(&Tex2DDesc, nullptr, &pTex2D);
	if (FAILED(hr))
		throw(UException(hr, _T("CreateDepthStencilView()でエラーが発生しました。深度バッファ用のテクスチャー作成エラーです。")));

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;

	ZeroMemory(&DSVDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	// テクスチャー作成時に指定したフォーマットと互換性があり、深度ステンシルビューとして指定できるフォーマットを指定する
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
		// 配列なし
	case 1:
		DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		DSVDesc.Texture2D.MipSlice = 0;
		break;
		// 配列あり
	default:
		DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		DSVDesc.Texture2DArray.ArraySize = ArraySize;
		DSVDesc.Texture2DArray.MipSlice = 0;
		DSVDesc.Texture2DArray.FirstArraySlice = 0;
		break;
	}
	// 深度ステンシルビューを作成
	hr = pD3DDevice->CreateDepthStencilView(pTex2D.Get(), &DSVDesc, ppDSView);
	if (FAILED(hr))
		throw(UException(hr, _T("CreateDepthStencilView()でエラーが発生しました。深度ステンシルビューが作成できません。")));

	if (ppSRView != nullptr)
		Maps::CreateSRViewFromDSView(pD3DDevice, *ppDSView, ppSRView);
}

// 深度ステンシルビューからシェーダーリソースビューを作成する
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

	// 深度ステンシルビューからテクスチャーを取得する
	pDSView->GetResource(&pResource);

	// シェーダーリソースビューを作成する
	if (FAILED(hr = pD3DDevice->CreateShaderResourceView(pResource.Get(), &SRVDesc, ppSRView)))
		throw(UException(hr, _T("CreateSRViewFromDSView()でエラーが発生しました。シェーダーリソースビューが作成できません。")));
}

// ノイズマップを作成する
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
		throw(UException(hr, _T("CreateSRViewOfNoiseMap()でエラーが発生しました。未対応のフォーマットが指定されました。")));
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

	// テクスチャー作成
	if (FAILED(hr = pD3DDevice->CreateTexture2D(&Tex2DDesc, &SubResourceData, &pTexture2D)))
		throw(UException(hr, _T("CreateSRViewOfNoiseMap()でエラーが発生しました。テクスチャーの作成に失敗しました")));

	// シェーダーリソースビューを作成
	if (FAILED(hr = pD3DDevice->CreateShaderResourceView(pTexture2D.Get(), nullptr, ppSRView)))
		throw(UException(hr, _T("CreateSRViewOfNoiseMap()でエラーが発生しました。シェーダーリソースビューの作成に失敗しました")));
}

// 頂点、インデックスバッファを作成する
ComPtr<ID3D11Buffer> Buffers::CreateBuffer(ID3D11Device* pD3DDevice, void* pData, size_t size, UINT CPUAccessFlag, D3D11_BIND_FLAG BindFlag, UINT miscFlag)
{
	HRESULT hr = E_FAIL;

	ComPtr<ID3D11Buffer> pBuffer = nullptr;

	// バッファー リソース。
	D3D11_BUFFER_DESC BufferDesc = { 0 };

	// サブリソース
	unique_ptr<D3D11_SUBRESOURCE_DATA> resource = nullptr;

	D3D11_USAGE Usage = D3D11_USAGE_DEFAULT;
	UINT CPUAccessFlags = 0;


	switch (CPUAccessFlag)
	{
		// CPUアクセスを許可しない
	case 0:
		Usage = D3D11_USAGE_DEFAULT;
		CPUAccessFlags = CPUAccessFlag;
		break;
		// CPUアクセスを許可する
	default:
		Usage = D3D11_USAGE_DYNAMIC;
		CPUAccessFlags = CPUAccessFlag;
		break;
	}

	// 初期値を設定する
	if (pData)
	{
		resource = make_unique<D3D11_SUBRESOURCE_DATA>();
		resource->pSysMem = pData;
		resource->SysMemPitch = 0;
		resource->SysMemSlicePitch = 0;
	}

	// バッファの設定
	BufferDesc.ByteWidth = size;                      // バッファサイズ
	BufferDesc.Usage = Usage;                     // リソース使用法を特定する
	switch (BindFlag)
	{
	case D3D11_BIND_STREAM_OUTPUT:
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
		break;
	default:
		BufferDesc.BindFlags = BindFlag;
		break;
	}
	BufferDesc.CPUAccessFlags = CPUAccessFlags;            // CPU アクセス
	BufferDesc.MiscFlags = miscFlag;                         // その他のフラグも設定しない

	if (miscFlag == D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		BufferDesc.StructureByteStride = size;

	// バッファを作成する
	hr = pD3DDevice->CreateBuffer(&BufferDesc, resource.get(), &pBuffer);
	if (FAILED(hr))
		throw(UException(hr, _T("CreateBuffer()でエラーが発生しました。バッファ作成エラーです。")));


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

// サンプラーステートを作成する
ComPtr<ID3D11SamplerState> Samplers::CreateSamplerState(ID3D11Device* pD3DDevice, D3D11_TEXTURE_ADDRESS_MODE TextureAddressMode, D3D11_FILTER Filter, D3D11_COMPARISON_FUNC ComparisonFunc)
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11SamplerState> pSamplerState = nullptr;

	// サンプラーステートを作成する
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = Filter;                           // サンプリング時に使用するフィルタ。
	samplerDesc.AddressU = TextureAddressMode;             // 0 ～ 1 の範囲外にある u テクスチャー座標の描画方法
	samplerDesc.AddressV = TextureAddressMode;             // 0 ～ 1 の範囲外にある v テクスチャー座標
	samplerDesc.AddressW = TextureAddressMode;             // 0 ～ 1 の範囲外にある w テクスチャー座標
	samplerDesc.MipLODBias = 0;                            // 計算されたミップマップ レベルからのバイアス
	samplerDesc.MaxAnisotropy = 1;                         // サンプリングに異方性補間を使用している場合の限界値。有効な値は 1 ～ 16 。
	samplerDesc.ComparisonFunc = ComparisonFunc;           // 比較オプション。
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;                                // アクセス可能なミップマップの下限値
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;                // アクセス可能なミップマップの上限値
	hr = pD3DDevice->CreateSamplerState(&samplerDesc, &pSamplerState);
	if (FAILED(hr))
		throw(UException(-1, _T("UIMesh;;CreateSamplerState()でエラーが発生しました。サンプラーステートを作成できません。")));

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

// デフォルトディスプレイのモードを列挙する
void Direct3D11::GetDisplayMode(DXGI_MODE_DESC* pModeDesc, UINT* pNum)
{
	HRESULT hr = E_FAIL;
	ComPtr<IDXGIOutput> pOutput = nullptr;


	// アダプターを取得。
	CreateAdapter();

	// アダプターに接続されているプライマリディスプレイを取得。
	hr = pAdapter->EnumOutputs(0, &pOutput);
	if (FAILED(hr))
		throw(UException(hr, _T("UDirect3D11::GetDisplayMode()でエラーが発生しました。アダプターに接続されているプライマリディスプレイの取得に失敗しました。")));

	// ディスプレイモードの一覧を取得
	hr = pOutput->GetDisplayModeList(UD3D11_FORMAT, 0, pNum, pModeDesc);
	if (FAILED(hr))
		throw(UException(hr, _T("UDirect3D11::GetDisplayMode()でエラーが発生しました。ディスプレイモードの一覧の取得に失敗しました。")));

	if (pModeDesc == nullptr)
	{
		// 0 が返ることがあるか不明だが念のため
		if (*pNum == 0)
			throw(UException(hr, _T("UDirect3D11::GetDisplayMode()でエラーが発生しました。ディスプレイモードが１つも取得できません。")));
	}
}

// Direct3Dの初期化
void Direct3D11::CreateDirect3D11(TCHAR* AppName, HINSTANCE hInstance, WNDPROC WndProc, DXGI_MODE_DESC* pDisplayMode,
	BOOL UseMultisample, BOOL UseDepthBuffer)
{
	m_UseMultiSample = UseMultisample;
	m_UseDepthBuffer = UseDepthBuffer;

	// ウィンドウを作成
	InitWindow(AppName, hInstance, WndProc, pDisplayMode);

	// アダプターを取得
	CreateAdapter();

	// デバイスの作成
	CreateDevice();

	// スワップチェーンの作成
	CreateSwapChain(pDisplayMode);

	// レンダーターゲットビューの作成
	CreateRenderTargetView();

	// 深度ステンシルビューの作成
	CreateDepthStencilView();

	// レンダーターゲットビューの設定
	pD3DDeviceContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), pDepthStencilView.Get());

	// ビューポート作成
	CreateViewport();

	// 表示モードの自動切換えを無効にする。
	// 自動切換えを有効にする場合はこの関数をコメントアウトする。
	// しかしコメントアウトした場合、初期起動をフルスクリーンモードにしたとき正しく動作してくれない。
	SetWindowAssociation();

	// デフォルトのラスタライザを設定する
	SetDefaultRasterize();

	// デフォルトの深度ステンシルステートを設定する
	SetDefaultDepthStencilState();

	//CreateSoundDevice();

	//// 入力関連の初期化
	//DInput.Init((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE));// DirectInput8の初期化
	///* Keyboard Object の初期化 */
	//if (!DInput.InitKeyboard(hWnd)) MSGBOX(_T("not found keyboard"), _T("DirectInput InitKeyboard"));
}

// ウィンドウ作成
void Direct3D11::InitWindow(TCHAR* AppName, HINSTANCE hInstance, WNDPROC WndProc, DXGI_MODE_DESC* pDisplayMode)
{
	// ウィンドウモード
	DWORD WindowStyle = WS_BORDER | WS_CAPTION | WS_SYSMENU;

	// ウィンドウクラス登録
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);					// WNDCLASSEX構造体のサイズ
	wcex.style = CS_HREDRAW | CS_VREDRAW;				// ウインドウスタイル	CS_HREDRAW（水平方向のサイズ変更があったら全体を描き直す）と、 CS_VREDRAW（垂直方向のサイズ変更で全体を描き直す）を指定
	wcex.lpfnWndProc = WndProc;							// ウィンドウプロシージャのアドレス
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;							// WinMain()のインスタンスハンドル
	wcex.hIcon = nullptr;								// アイコン設定		LoadIcon()でアイコンをロード
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);		// マウスカーソル
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);	// ウインドウのクライアント領域の背景色		HBRUSH型は、ブラシのハンドルデフォルトの色である、(HBRUSH)(COLOR_WINDOW + 1)とする
	wcex.lpszMenuName = nullptr;						// メニュー
	wcex.lpszClassName = AppName;						// Windowクラスの名前
	wcex.hIconSm = nullptr;								// 使用する小さいアイコン
	RegisterClassEx(&wcex);					// ウィンドウクラスを登録		登録したクラスは、CreateWindow 関数または CreateWindowEx 関数で使用

	RECT rc;
	::SetRect(&rc, 0, 0, pDisplayMode->Width, pDisplayMode->Height);
	// ウィンドウモードのときタイトルバーを考慮して表示領域を設定するために必要なサイズを取得する
	::AdjustWindowRect(&rc, WindowStyle, FALSE);

	// ウィンドウを作成
	hWnd = CreateWindow(
		wcex.lpszClassName,		// 登録されているクラス名
		wcex.lpszClassName,		// ウィンドウ名
		WindowStyle,			// ウィンドウスタイル
		0,						// ウィンドウの横方向の位置
		0,						// ウィンドウの縦方向の位置
		rc.right - rc.left,		// ウィンドウの幅
		rc.bottom - rc.top,		// ウィンドウの高さ
		nullptr,				// 親ウィンドウまたはオーナーウィンドウのハンドル
		nullptr,				// メニューハンドルまたは子ウィンドウ ID
		hInstance,				// アプリケーションインスタンスのハンドル
		nullptr					// ウィンドウ作成データ
		);

	if (hWnd == nullptr)
		throw(UException(-1, _T("UDirect3D11::InitWindow()でエラーが発生しました。ウィンドウが作成できません。")));
}

// アダプターを取得
void Direct3D11::CreateAdapter()
{
	HRESULT hr = E_FAIL;
	ComPtr<IDXGIFactory> pFactory = nullptr;

	if (pAdapter == nullptr)
	{
		// ファクトリを作成する
		hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
		if (FAILED(hr))
			throw(UException(hr, _T("Direct3D11::CreateAdapter()でエラーが発生しました。ファクトリの作成エラーです。")));

		// デフォルトアダプターを取得
		hr = pFactory->EnumAdapters(0, &pAdapter);
		if (FAILED(hr))
			throw(UException(hr, _T("Direct3D11::CreateAdapter()でエラーが発生しました。デフォルトアダプターの作成エラーです。")));
	}
}

// デバイスの作成
void Direct3D11::CreateDevice()
{
	HRESULT hr;

	// デバイス作成時に使用するビデオカードの性能一覧
	// Direct3D11.0のSM　5.0のみサポートする
	D3D_FEATURE_LEVEL featureLevel[] = {
		D3D_FEATURE_LEVEL_11_0,  // Direct3D11.0 SM 5.0
	};

	UINT createDeviceFlag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	// デバッグコンパイルの場合、デバッグレイヤーを有効にする
	// ソフトウェアレイヤー
	createDeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL level;

	// デバイスの作成
	hr = D3D11CreateDevice(
		pAdapter.Get(),				// 使用するアダプターを設定。nullptrの場合はデフォルトのアダプター。
		D3D_DRIVER_TYPE_UNKNOWN,	// D3D_DRIVER_TYPEのいずれか。ドライバーの種類。pAdapterが nullptr 以外の場合は、D3D_DRIVER_TYPE_UNKNOWNを指定する。
		nullptr,					// ソフトウェアラスタライザを実装するDLLへのハンドル。D3D_DRIVER_TYPE を D3D_DRIVER_TYPE_SOFTWARE に設定している場合は nullptr にできない。
		createDeviceFlag,			// D3D11_CREATE_DEVICE_FLAGの組み合わせ。デバイスを作成時に使用されるパラメータ。
		featureLevel,				// D3D_FEATURE_LEVELのポインタ
		1,							// D3D_FEATURE_LEVEL配列の要素数
		D3D11_SDK_VERSION,			// DirectX SDKのバージョン。この値は固定。
		&pD3DDevice,				// 初期化されたデバイス
		&level,						// 採用されたフィーチャーレベル
		&pD3DDeviceContext			// 初期化されたデバイスコンテキスト
		);

	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::CreateDevice()でエラーが発生しました。デバイスの作成エラーです。")));

	// アダプターーの情報を取得
	DXGI_ADAPTER_DESC desc;
	pAdapter->GetDesc(&desc);
}

// スワップチェーンの作成
void Direct3D11::CreateSwapChain(DXGI_MODE_DESC* pDisplayMode)
{
	HRESULT hr = E_FAIL;
	TCHAR s[256];
	ComPtr<IDXGIFactory> pFactory = nullptr;
	DXGI_SWAP_CHAIN_DESC sd;


	ZeroMemory(&sd, sizeof(sd));
	CopyMemory(&(sd.BufferDesc), pDisplayMode, sizeof(DXGI_MODE_DESC));

	sd.BufferDesc.Format = UD3D11_FORMAT;                            // バックバッファフォーマット
	// バックバッファのサーフェス使用法および CPU アクセス オプションを表す DXGI_USAGE 列挙型のメンバー
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                // バックバッファにレンダリング可能にする。
	// スワップ チェーンのバッファー数を表す値。
	sd.BufferCount = 1;
	// 出力ウィンドウへの HWND ハンドル。このメンバーを nullptr にはできない。
	sd.OutputWindow = hWnd;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;	//1/60 = 60fps
	// 出力がウィンドウ モードの場合は TRUE。それ以外の場合は FALSE。
	// 初期起動時の表示モードはウィンドウモードに固定とする。
	sd.Windowed = TRUE;
	// サーフェスのスワップ処理後にバックバッファの内容を保持するか。
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                        // バックバッファの内容を保持しない。
	// スワップ チェーンの動作のオプション。
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;               // フルスクリーンモードに切り替えると、アプリケーション ウィンドウのサイズに合わせて解像度が変更される。

	// ファクトリーを作成する
	// CreateDXGIFactoryで作成したファクトリーを使用すると実行できるがワーニングエラーになるので IDXGIAdapter から作成する。
	hr = pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::CreateSwapChain()でエラーが発生しました。ファクトリーの取得エラーです。")));

	// マルチサンプリングの設定
	// マルチサンプルを使用しない場合の設定
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	int startMultisample;

	// サンプリング数が最大のものを採用する
	if (m_UseMultiSample == TRUE)
		startMultisample = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
	// マルチサンプルを無効にする。
	else
		startMultisample = 1;

	UINT Quality = 0;
	for (int i = startMultisample; i >= 0; i--)
	{
		// サポートするクォリティレベルの最大値を取得する
		hr = pD3DDevice->CheckMultisampleQualityLevels(pDisplayMode->Format, (UINT)i, &Quality);
		if (FAILED(hr))
			throw(UException(hr, _T("Direct3D11::CreateSwapChain()でエラーが発生しました。サポートするクォリティレベルの最大値の取得エラーです。")));

		// 0 以外のときフォーマットとサンプリング数の組み合わせをサポートする
		// オンラインヘルプではCheckMultisampleQualityLevelsの戻り値が 0 のときサポートされないとかかれているが
		// pNumQualityLevels の戻り値が 0 かで判断する。
		// Direct3D 10 版 のオンラインヘルプにはそうかかれているので、オンラインヘルプの間違い。
		if (Quality != 0)
		{
			sd.SampleDesc.Count = i;
			sd.SampleDesc.Quality = Quality - 1;

			// スワップチェーンを作成する。
			hr = pFactory->CreateSwapChain(pD3DDevice.Get(), &sd, &pSwapChain);
			_stprintf_s(s, _T("■□■マルチサンプル：Count(%d) Quality(%d)"), sd.SampleDesc.Count, sd.SampleDesc.Quality);
			if (SUCCEEDED(hr))
			{
				if (sd.SampleDesc.Count == 1)
					m_UseMultiSample = FALSE;

				_tcscat_s(s, _T("　⇒　OK\n"));
				OutputDebugString(s);
				break;
			}
			else
			{
				_tcscat_s(s, _T("　⇒　NG\n"));
				OutputDebugString(s);
			}
		}
	}
	if (pSwapChain == nullptr)
		throw(UException(-1, _T("Direct3D11::CreateSwapChain()でエラーが発生しました。スワップチェーンが作成できませんでした")));

	_stprintf_s(s, _T("■□■ディスプレイモード：解像度( %d : %d )　リフレッシュレート( %d / %d )\n"),
		pDisplayMode->Width, pDisplayMode->Height,
		pDisplayMode->RefreshRate.Denominator, pDisplayMode->RefreshRate.Numerator);
	OutputDebugString(s);

}

// レンダリングターゲットビューの作成
void Direct3D11::CreateRenderTargetView()
{
	HRESULT hr = E_FAIL;
	// スワップチェインに用意されたバッファ（2Dテクスチャ）を取得
	ComPtr<ID3D11Texture2D> back_buff = nullptr;

	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buff);
	if (FAILED(hr)){
		throw(UException(hr, _T("Direct3D11::CreateRenderTargetView()でエラーが発生しました。スワップチェーンからバックバッファの取得エラーです。")));
	}

	hr = pD3DDevice->CreateRenderTargetView(back_buff.Get(), nullptr, pRenderTargetView.GetAddressOf());
	if (FAILED(hr)){
		throw(UException(hr, _T("Direct3D11::CreateRenderTargetView()でエラーが発生しました。レンダリングターゲットビューの作成エラーです。")));
	}
}

// 深度ステンシルビューの作成
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


	// スワップチェーンの設定を取得する
	hr = pSwapChain->GetDesc(&chainDesc);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::CreateDepthStencilView()でエラーが発生しました。スワップチェーンの設定の取得エラーです。")));

	ZeroMemory(&descDS, sizeof(D3D11_TEXTURE2D_DESC));
	descDS.Width = chainDesc.BufferDesc.Width;   // バックバッファと同じにする。
	descDS.Height = chainDesc.BufferDesc.Height;  // バックバッファと同じにする。
	descDS.MipLevels = 1;                            // ミップマップを作成しない
	descDS.ArraySize = 1;                            // テクスチャーの配列数
	descDS.Format = UD3D11_DEPTH_FORMAT;          // フォーマット
	descDS.SampleDesc.Count = chainDesc.SampleDesc.Count;   // バックバッファと同じにする。
	descDS.SampleDesc.Quality = chainDesc.SampleDesc.Quality; // バックバッファと同じにする。
	descDS.Usage = D3D11_USAGE_DEFAULT;          // GPU による読み取りおよび書き込みアクセスを必要とするリソース。
	descDS.BindFlags = D3D11_BIND_DEPTH_STENCIL |    // 深度ステンシルバッファとして作成する
		D3D11_BIND_SHADER_RESOURCE;   // シェーダーリソースビューとして作成する
	descDS.CPUAccessFlags = 0;                            // CPU アクセスが不要。
	descDS.MiscFlags = 0;                            // その他のフラグも設定しない。
	// 深度バッファ用のテクスチャー作成
	hr = pD3DDevice->CreateTexture2D(&descDS, nullptr, &pDepthBuffer);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::CreateDepthStencilView()でエラーが発生しました。深度バッファ用のテクスチャー作成エラーです。")));

	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	// テクスチャー作成時に指定したフォーマットと互換性があり、深度ステンシルビューとして指定できるフォーマットを指定する
	switch (descDS.Format)
	{
		// 8ビットフォーマットは使用できない？
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

	// マルチサンプルを使用している場合
	if (chainDesc.SampleDesc.Count > 1)
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	// マルチサンプルを使用していない場合
	else
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	descDSV.Texture2D.MipSlice = 0;

	// 深度ステンシルビューを作成
	hr = pD3DDevice->CreateDepthStencilView(pDepthBuffer.Get(), &descDSV, &pDepthStencilView);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::CreateDepthStencilView()でエラーが発生しました。深度ステンシルビューの作成エラーです。")));
}

// ビューポートの作成
void Direct3D11::CreateViewport()
{
	HRESULT hr = E_FAIL;

	DXGI_SWAP_CHAIN_DESC chainDesc;
	// スワップチェーンの設定を取得する
	hr = pSwapChain->GetDesc(&chainDesc);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::CreateViewport()でエラーが発生しました。スワップチェーンの設定の取得エラーです。")));

	D3D11_VIEWPORT Viewport[1];
	Viewport[0].TopLeftX = 0;
	Viewport[0].TopLeftY = 0;
	Viewport[0].Width = (FLOAT)chainDesc.BufferDesc.Width;
	Viewport[0].Height = (FLOAT)chainDesc.BufferDesc.Height;
	Viewport[0].MinDepth = 0.0f;
	Viewport[0].MaxDepth = 1.0f;
	pD3DDeviceContext->RSSetViewports(1, Viewport);
}

// ウィンドウアソシエーション
void Direct3D11::SetWindowAssociation()
{
	HRESULT hr = E_FAIL;
	ComPtr<IDXGIFactory> pFactory = nullptr;


	// ファクトリーを作成する
	// CreateDXGIFactoryで作成したファクトリーを使用すると実行できるがワーニングエラーになるので IDXGIAdapter から作成する。
	hr = pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::SetWindowAssociation()でエラーが発生しました。ファクトリーの取得エラーです。")));

	// 表示モードの自動切換えを無効にする。
	hr = pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::SetWindowAssociation()でエラーが発生しました。ウィンドウアソシエーションの作成エラーです。")));
}

// デフォルトのラスタライザを設定
void Direct3D11::SetDefaultRasterize()
{
	HRESULT hr = E_FAIL;
	ComPtr<ID3D11RasterizerState> g_pRasterState = nullptr;
	D3D11_RASTERIZER_DESC rsState;


	rsState.FillMode = D3D11_FILL_SOLID;    // ポリゴン面描画
	rsState.CullMode = D3D11_CULL_BACK;     // 裏面を描画しない
	rsState.FrontCounterClockwise = TRUE;   // 反時計回りを表面
	rsState.DepthBias = 0;
	rsState.DepthBiasClamp = 0;
	rsState.SlopeScaledDepthBias = 0;
	rsState.DepthClipEnable = TRUE;
	rsState.ScissorEnable = FALSE;          // シザー矩形無効

	// スワップチェーンのマルチサンプリングの設定にあわせる
	DXGI_SWAP_CHAIN_DESC swapDesc;
	pSwapChain->GetDesc(&swapDesc);
	if (swapDesc.SampleDesc.Count != 1)
		rsState.MultisampleEnable = TRUE;
	else
		rsState.MultisampleEnable = FALSE;

	rsState.AntialiasedLineEnable = FALSE;
	hr = pD3DDevice->CreateRasterizerState(&rsState, &g_pRasterState);
	if (FAILED(hr))
		throw(UException(hr, _T("Direct3D11::SetDefaultRasterize()でエラーが発生しました。ラスタライザステートの作成エラーです。")));

	pD3DDeviceContext->RSSetState(g_pRasterState.Get());
}

// デフォルトの深度ステンシルステートを設定
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
		throw(UException(hr, _T("Direct3D11::SetDefaultDepthStencilState()でエラーが発生しました。深度ステンシルステートの作成エラーです。")));

	pD3DDeviceContext->OMSetDepthStencilState(pDepthStencilState.Get(), 0);
}

HRESULT Direct3D11::Present(UINT SyncInterval, UINT Flags)
{
	HRESULT hr = E_FAIL;
	if (FAILED(hr = pSwapChain->Present(SyncInterval, Flags)))
		throw(UException(hr, _T("Direct3D11::Present()でエラーが発生しました。")));

	return hr;
}
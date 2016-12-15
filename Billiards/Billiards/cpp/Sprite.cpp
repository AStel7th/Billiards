#include "../Header/Sprite.h"
//#include "../Header/GraphicsPipeline.h"
//====================================================================
//  Sprite
//====================================================================
list<Sprite*> Sprite::drawObjectList;
//unique_ptr<GraphicsPipeline> Sprite::m_pGraphicsPipeline;
unique_ptr<VertexShader> Sprite::pVertexShader = nullptr;
unique_ptr<PixelShader> Sprite::pPixelShader = nullptr;
ComPtr<ID3D11Buffer> Sprite::m_pVertexBuffer = nullptr;
ComPtr<ID3D11Buffer> Sprite::m_pObjBuffers = nullptr;
ComPtr<ID3D11InputLayout> Sprite::pInputLayout = nullptr;
ComPtr<ID3D11SamplerState> Sprite::m_pSamplerState = nullptr;

// コピー
void Sprite::copy(const Sprite &r) {
	m_pSRView = r.m_pSRView;
	tex = r.tex;
	subtex = r.subtex;
	polyW = r.polyW;
	polyH = r.polyH;
	pivotX = r.pivotX;
	pivotY = r.pivotY;
	posX = r.posX;
	posY = r.posY;
	posZ = r.posZ;
	rad = r.rad;
	scaleX = r.scaleX;
	scaleY = r.scaleY;
	uvLeft = r.uvLeft;
	uvTop = r.uvTop;
	uvW = r.uvW;
	uvH = r.uvH;
	color = r.color;
	scW = r.scW;
	scH = r.scH;
}

// スクリーンサイズを指定します。
Sprite::Sprite(int screenWidth, int screenHeight) :
	polyW(0),
	polyH(0),
	pivotX(0),
	pivotY(0),
	posX(0),
	posY(0),
	posZ(0),
	rad(RADIAN(0.0f)),
	scaleX(1.0f),
	scaleY(1.0f),
	uvLeft(0.0f),
	uvTop(0.0f),
	uvW(1.0f),
	uvH(1.0f),
	color(1.0f, 1.0f, 1.0f, 1.0f)
{
	m_pSRView = nullptr;
	tex = nullptr;
	subtex = nullptr;
	SetScreenSize(screenWidth, screenHeight);
}

Sprite::Sprite() :
	polyW(0),
	polyH(0),
	pivotX(0),
	pivotY(0),
	posX(0),
	posY(0),
	posZ(0),
	rad(RADIAN(0.0f)),
	scaleX(1.0f),
	scaleY(1.0f),
	uvLeft(0.0f),
	uvTop(0.0f),
	uvW(1.0f),
	uvH(1.0f),
	color(1.0f, 1.0f, 1.0f, 1.0f)
{
	m_pSRView = nullptr;
	tex = nullptr;
	subtex = nullptr;
	SetScreenSize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

Sprite::Sprite(const Sprite &r)
{
	copy(r);
}

Sprite::~Sprite()
{
}

// 静的領域生成
void Sprite::Create()
{
	Direct3D11 &d3d11 = Direct3D11::Instance();

	// シェーダーを作成する
	//#if defined(DEBUG) || defined(_DEBUG)
	pVertexShader = make_unique<VertexShader>(d3d11.pD3DDevice.Get(), L"HLSL/Sprite_Pass0.hlsl", "Sprite_Pass0_VS_Main");
	pPixelShader = make_unique<PixelShader>(d3d11.pD3DDevice.Get(), L"HLSL/Sprite_Pass0.hlsl", "Sprite_Pass0_PS_Main");

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(d3d11.pD3DDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		pVertexShader->GetByteCode()->GetBufferPointer(), pVertexShader->GetByteCode()->GetBufferSize(), &pInputLayout)))
	{
		return;
	}

	Sprite::VERTEX vtx[] = {
		XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f),
		XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)

	};

	// 頂点バッファを作成する
	m_pVertexBuffer = Buffers::CreateVertexBuffer(d3d11.pD3DDevice.Get(), vtx, sizeof(vtx), 0);

	// サンプラーステートを作成する
	m_pSamplerState = Samplers::CreateSamplerState(d3d11.pD3DDevice.Get(), D3D11_TEXTURE_ADDRESS_CLAMP);

	// 定数バッファ
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Sprite::OBJ_BUFFER);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(d3d11.pD3DDevice->CreateBuffer(&bd, nullptr, &m_pObjBuffers)))
	{
		return;
	}
}

// 解放処理
void Sprite::end_last()
{
}


// テクスチャ設定
void Sprite::SetTexture(TextureInfo* tex)
{
	_ASSERT(tex);
	this->tex = tex;

	CreateShaderResourceView(d3d11.pD3DDevice.Get(), tex->image.GetImages(), tex->image.GetImageCount(), tex->metadata, &m_pSRView);
	SetSize(this->tex->metadata.width, this->tex->metadata.height);

}


#pragma region げったーせったー
// 板ポリサイズ指定
void Sprite::SetSize(int w, int h) {
	polyW = w;
	polyH = h;
}

//サイズゲッちゅ
void Sprite::GetSize(int* w, int* h)
{
	*w = polyW;
	*h = polyH;
}

XMINT2 Sprite::GetSize()
{
	XMINT2 size;
	size.x = polyW;
	size.y = polyH;

	return size;
}

// スクリーンサイズ指定
void Sprite::SetScreenSize(int w, int h) {
	scW = w;
	scH = h;
}

// 中心指定
void Sprite::SetPivot(float x, float y) {
	pivotX = x;
	pivotY = y;
}

// 座標
void Sprite::SetPos(float x, float y) {
	posX = x;
	posY = y;
}

// 回転
void Sprite::SetRotate(float deg) {
	rad = RADIAN(deg);
}

void Sprite::GetPos(float* x, float* y)
{
	if (x) *x = posX;
	if (y) *y = posY;
}

float Sprite::GetRotate() {
	return DEGREE(rad);
}

void Sprite::SetScale(float sx, float sy) {
	scaleX = sx;
	scaleY = sy;
}

// UV切り取り指定
void Sprite::SetUV(float l, float t, float w, float h) {
	uvLeft = l;
	uvTop = t;
	uvW = w;
	uvH = h;
}

// α設定
void Sprite::SetColor(XMFLOAT4& col) {
	color = col;
}

XMFLOAT4& Sprite::GetColor() {
	return color;
}

// プライオリティ設定
void Sprite::SetPriority(float z) {
	posZ = z;
}

float Sprite::GetPriority() {
	return posZ;
}

#pragma endregion 

// 描画リストに追加
void Sprite::Draw() {
	drawObjectList.push_back(this);
}

// 描画リストを描画
void Sprite::DrawAll()
{
	HRESULT hr;

	Direct3D11 &d3d11 = Direct3D11::Instance();

	d3d11.pD3DDeviceContext->VSSetShader(pVertexShader->GetShader(), nullptr, 0);
	d3d11.pD3DDeviceContext->PSSetShader(pPixelShader->GetShader(), nullptr, 0);
	d3d11.pD3DDeviceContext->IASetInputLayout(pInputLayout.Get());

	// 2D描画用射影変換行列
	XMMATRIX proj;
	proj = XMMatrixIdentity();

	list<Sprite*>::iterator it;
	for (it = drawObjectList.begin(); it != drawObjectList.end(); it++)
	{
		Sprite* sp = (*it);

		// 頂点バッファ設定
		UINT stride = sizeof(Sprite::VERTEX);
		UINT offset = 0;
		d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 1, sp->m_pVertexBuffer.GetAddressOf(), &stride, &offset);

		// プリミティブ タイプおよびデータの順序に関する情報を設定
		d3d11.pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// ピクセルシェーダーのサンプラーステートを設定する
		d3d11.pD3DDeviceContext->PSSetSamplers(0, 1, sp->m_pSamplerState.GetAddressOf());

		// デカールマップを設定する
		d3d11.pD3DDeviceContext->PSSetShaderResources(0, 1, sp->m_pSRView.GetAddressOf());



		proj = XMMatrixOrthographicLH((float)sp->scW, (float)-sp->scH, 0, 1);
		proj.r[3].m128_f32[0] = -1.0f;
		proj.r[3].m128_f32[1] = 1.0f;

		XMMATRIX world, scale, rot;
		world = XMMatrixIdentity();
		world = XMMatrixScaling((float)sp->polyW, (float)sp->polyH, 1.0f);
		scale = XMMatrixScaling(sp->scaleX, sp->scaleY, 1.0f);
		rot = XMMatrixRotationZ(sp->rad);
		world.r[3].m128_f32[0] = -sp->pivotX;		// ピボット分オフセット
		world.r[3].m128_f32[1] = -sp->pivotY;
		world = world * rot * scale;

		world.r[3].m128_f32[0] += sp->posX;
		world.r[3].m128_f32[1] += sp->posY;


		D3D11_MAPPED_SUBRESOURCE mappedResourceObj;
		Sprite::OBJ_BUFFER* cBufferObj;

		if (FAILED(hr = d3d11.pD3DDeviceContext->Map(sp->m_pObjBuffers.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceObj)))
			throw(UException(hr, _T("Sprite::DrawAll()でエラーが発生しました。ロックできません")));

		cBufferObj = (Sprite::OBJ_BUFFER*)mappedResourceObj.pData;

		XMStoreFloat4x4(&cBufferObj->World,XMMatrixTranspose(world));
		XMStoreFloat4x4(&cBufferObj->Proj, XMMatrixTranspose(proj));

		cBufferObj->Uv_left = sp->uvLeft;
		cBufferObj->Uv_top = sp->uvTop;
		cBufferObj->Uv_width = sp->uvW;
		cBufferObj->Uv_height = sp->uvH;
		cBufferObj->Color = sp->color;


		d3d11.pD3DDeviceContext->Unmap(sp->m_pObjBuffers.Get(), 0);

		// 頂点シェーダーに定数バッファを設定する。;
		d3d11.pD3DDeviceContext->VSSetConstantBuffers(0, 1, sp->m_pObjBuffers.GetAddressOf());

		// ピクセルシェーダーに定数バッファを設定する。
		d3d11.pD3DDeviceContext->PSSetConstantBuffers(0, 1, sp->m_pObjBuffers.GetAddressOf());

		// 描画
		d3d11.pD3DDeviceContext->Draw(4, 0);
	}

	ClearDrawList();
}

// 描画リストクリア
void Sprite::ClearDrawList()
{
	drawObjectList.clear();
}

// 簡単初期化 tex:コンテナから取得したTextureInfoのアドレス x:描画x座標 y:描画y座標
void Sprite::Init(TextureInfo* tex, float x, float y)
{
	SetTexture(tex);
	SetPos(x, y);
}

// 簡単初期化（座標を中心に表示します） tex:コンテナから取得したTextureInfoのアドレス x:描画x座標 y:描画y座標
void Sprite::InitCenter(TextureInfo *tex, float x, float y)
{
	SetTexture(tex);
	SetPos(x, y);
	SetPivot(tex->metadata.width / 2.0f, tex->metadata.height / 2.0f);
}

// AddUV()で設定したuv座標セットからスプライトにuv座標をセットします。中心点も設定してね　uvid:uv番号 x,y:中心点 初期値0
void Sprite::SetUVFromTex(int uvid, float x, float y)
{
	SetUV((float)tex->uvrect.x, (float)tex->uvrect.y, (float)tex->uvrect.z, (float)tex->uvrect.w);
	SetSize((int)(tex->metadata.width * tex->uvrect.z), (int)(tex->metadata.height * tex->uvrect.w));
	SetPivot(x, y);
	//setScale(1.0f * tex->uvrect[uvid].width, 1.0f * tex->uvrect[uvid].height);
}

// AddUV()で設定したuv座標セットからスプライトにuv座標をセットします。　中心点を中心にするお　uvid:uv番号
void Sprite::SetUVFromTexCenter(int uvid)
{
	SetUV((float)tex->uvrect.x, (float)tex->uvrect.y, (float)tex->uvrect.z, (float)tex->uvrect.w);
	SetSize(int(tex->metadata.width * tex->uvrect.z), (int)(tex->metadata.height * tex->uvrect.w));
	SetPivot((float)polyW / 2, (float)polyH / 2);
	//setScale(1.0f * tex->uvrect[uvid].width, 1.0f * tex->uvrect[uvid].height);
}

//====================================================================
// TextureInfo
//====================================================================

TextureInfo::TextureInfo() : uvrect(0.0f,0.0f,1.0f,1.0f)
{
}

TextureInfo::TextureInfo(const TextureInfo & r)
{
	metadata = r.metadata;
	image.InitializeFromImage(*r.image.GetImages());
	uvrect = r.uvrect;
}

TextureInfo::~TextureInfo()
{
}

TextureInfo & TextureInfo::operator=(const TextureInfo & r)
{
	metadata = r.metadata;
	image.InitializeFromImage(*r.image.GetImages());
	uvrect = r.uvrect; 
	
	return *this;
}

void TextureInfo::AddUV(int texNum, int left, int top, int width, int height, int uvid)
{
	uvrect.z = (float)width / metadata.width;
	uvrect.x = (float)left / metadata.width;
	uvrect.y = (float)top / metadata.height;
	uvrect.w = (float)height / metadata.height;
}

//====================================================================
//  TextureContainer
//====================================================================
TextureContainer::TextureContainer()
{

}

TextureContainer::~TextureContainer()
{
}

TextureInfo TextureContainer::LoadTexture(const string& t_pTextureName)
{
	auto it = data.find(t_pTextureName);

	if (it == data.end())
	{
		WCHAR	wstr[512];
		size_t wLen = 0;
		mbstowcs_s(&wLen, wstr, t_pTextureName.size() + 1, t_pTextureName.c_str(), _TRUNCATE);

		LoadFromWICFile(wstr, 0, &data[t_pTextureName].metadata, data[t_pTextureName].image);

		return data[t_pTextureName];
	}
	return (*it).second;
}
#include "../Header/Sprite.h"
//====================================================================
//  Sprite
//====================================================================
list<Sprite*> Sprite::drawObjectList;
unique_ptr<GraphicsPipeline> Sprite::m_pGraphicsPipeline;
ComPtr<ID3D11Buffer> Sprite::m_pVertexBuffer;
ComPtr<ID3D11Buffer> Sprite::m_pObjBuffers;
ComPtr<ID3D11SamplerState> Sprite::m_pSamplerState;

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
color(1.0f,1.0f,1.0f,1.0f)
{
	m_pSRView = nullptr;
	tex = nullptr;
	subtex = nullptr;
	setScreenSize(screenWidth, screenHeight);
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
	setScreenSize(WINDOW_WIDTH, WINDOW_HEIGHT);
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

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	m_pGraphicsPipeline = make_unique<GraphicsPipeline>();

	// ラスタライザーステートを作成する
	m_pGraphicsPipeline->CreateRasterizerState(D3D11_CULL_MODE::D3D11_CULL_BACK);

	// 深度ステンシルステートを作成する
	m_pGraphicsPipeline->CreateDepthStencilState(FALSE, D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL);

	// ブレンドステートを線形合成で作成する
	GraphicsPipeline::UEBLEND_STATE BlendStateType[1] = { GraphicsPipeline::UEBLEND_STATE::ALIGNMENT };
	m_pGraphicsPipeline->CreateBlendState(BlendStateType, 1);

	// シェーダーを作成する
	//#if defined(DEBUG) || defined(_DEBUG)
	m_pGraphicsPipeline->CreateVertexShaderFromFile(_T("HLSL/Sprite_Pass0.hlsl"), "Sprite_Pass0_VS_Main", layout, _countof(layout));
	m_pGraphicsPipeline->CreatePixelShaderFromFile(_T("HLSL/Sprite_Pass0.hlsl"), "Sprite_Pass0_PS_Main");

	Sprite::VERTEX vtx[] = {
		XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f),
		XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)

	};

	// 頂点バッファを作成する
	m_pVertexBuffer = Buffers::CreateVertexBuffer(d3d11.pD3DDevice.Get(),vtx, sizeof(vtx), 0);

	// サンプラーステートを作成する
	m_pSamplerState = Samplers::CreateSamplerState(d3d11.pD3DDevice.Get(),D3D11_TEXTURE_ADDRESS_CLAMP);

	// 定数バッファ
	m_pObjBuffers = m_pGraphicsPipeline->CreateConstantBuffer(nullptr, sizeof(Sprite::OBJ_BUFFER), D3D11_CPU_ACCESS_WRITE);
}

// 解放処理
void Sprite::end_last()
{

}


// テクスチャ設定
void Sprite::setTexture(TextureInfo* tex)
{
	_ASSERT(tex);
	this->tex = tex;

	CreateShaderResourceView(d3d11.pD3DDevice.Get(), tex->image.GetImages(), tex->image.GetImageCount(), tex->metadata, &m_pSRView);
	setSize(this->tex->metadata.width, this->tex->metadata.height);

}


#pragma region げったーせったー
// 板ポリサイズ指定
void Sprite::setSize(int w, int h) {
	polyW = w;
	polyH = h;
}

//サイズゲッちゅ
void Sprite::getSize(int* w, int* h)
{
	*w = polyW;
	*h = polyH;
}

// スクリーンサイズ指定
void Sprite::setScreenSize(int w, int h) {
	scW = w;
	scH = h;
}

// 中心指定
void Sprite::setPivot(float x, float y) {
	pivotX = x;
	pivotY = y;
}

// 座標
void Sprite::setPos(float x, float y) {
	posX = x;
	posY = y;
}

// 回転
void Sprite::setRotate(float deg) {
	rad = RADIAN(deg);
}

void Sprite::getPos(float* x, float* y)
{
	if (x) *x = posX;
	if (y) *y = posY;
}

float Sprite::getRotate() {
	return DEGREE(rad);
}

void Sprite::setScale(float sx, float sy) {
	scaleX = sx;
	scaleY = sy;
}

// UV切り取り指定
void Sprite::setUV(float l, float t, float w, float h) {
	uvLeft = l;
	uvTop = t;
	uvW = w;
	uvH = h;
}

// α設定
void Sprite::setColor(XMFLOAT4& col) {
	color = col;
}

XMFLOAT4& Sprite::getColor() {
	return color;
}

// プライオリティ設定
void Sprite::setPriority(float z) {
	posZ = z;
}

float Sprite::getPriority() {
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

	// 2D描画用射影変換行列
	XMMATRIX proj;
	proj = XMMatrixIdentity();

	list<Sprite*>::iterator it;
	for (it = drawObjectList.begin(); it != drawObjectList.end(); it++)
	{
		Sprite* sp = (*it);

		// 各種グラフィックパイプラインを設定
		sp->m_pGraphicsPipeline->SetVertexShader();
		sp->m_pGraphicsPipeline->SetHullShader();
		sp->m_pGraphicsPipeline->SetDomainShader();
		sp->m_pGraphicsPipeline->SetGeometryShader();
		sp->m_pGraphicsPipeline->SetPixelShader();
		sp->m_pGraphicsPipeline->SetRasterizerState();
		sp->m_pGraphicsPipeline->SetDepthStencilState();
		sp->m_pGraphicsPipeline->SetBlendState();

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

		cBufferObj->World = XMMatrixTranspose(world);
		cBufferObj->Proj = XMMatrixTranspose(proj);

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
	setTexture(tex);
	setPos(x, y);
}

// 簡単初期化（座標を中心に表示します） tex:コンテナから取得したTextureInfoのアドレス x:描画x座標 y:描画y座標
void Sprite::InitCenter(TextureInfo *tex, float x, float y)
{
	setTexture(tex);
	setPos(x, y);
	setPivot(tex->metadata.width / 2.0f, tex->metadata.height / 2.0f);
}

// AddUV()で設定したuv座標セットからスプライトにuv座標をセットします。中心点も設定してね　uvid:uv番号 x,y:中心点 初期値0
void Sprite::SetUVFromTex(int uvid, float x, float y)
{
	if (tex->uvrect.find(uvid) != tex->uvrect.end())
	{
		setUV((float)tex->uvrect[uvid].x, (float)tex->uvrect[uvid].y, (float)tex->uvrect[uvid].z, (float)tex->uvrect[uvid].w);
		setSize((int)(tex->metadata.width * tex->uvrect[uvid].z), (int)(tex->metadata.height * tex->uvrect[uvid].w));
		setPivot(x, y);
		//setScale(1.0f * tex->uvrect[uvid].width, 1.0f * tex->uvrect[uvid].height);
	}

}

// AddUV()で設定したuv座標セットからスプライトにuv座標をセットします。　中心点を中心にするお　uvid:uv番号
void Sprite::SetUVFromTexCenter(int uvid)
{
	if (tex->uvrect.find(uvid) != tex->uvrect.end())
	{
		setUV((float)tex->uvrect[uvid].x, (float)tex->uvrect[uvid].y, (float)tex->uvrect[uvid].z, (float)tex->uvrect[uvid].w);
		setSize(int(tex->metadata.width * tex->uvrect[uvid].z), (int)(tex->metadata.height * tex->uvrect[uvid].w));
		setPivot((float)polyW / 2, (float)polyH / 2);
		//setScale(1.0f * tex->uvrect[uvid].width, 1.0f * tex->uvrect[uvid].height);
	}

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

// テクスチャを読み込みます。 t_pTextureName：ファイル名　texNum:識別ID
int TextureContainer::LoadTexture(const WCHAR* t_pTextureName, int texNum)
{
	int result = -1;

	if (data.find(texNum) == data.end())
	{
		//// テクスチャの作成
		HRESULT hr;

		setlocale(LC_ALL, "japanese");

		hr = LoadFromWICFile(t_pTextureName, 0, &data[texNum].metadata, data[texNum].image);
		if (SUCCEEDED(hr)){
			result = 0;
		}
	}
	return(result);
}

// UV設定するお texNum:設定するテクスチャID left:基点のx座標 top:基点のy座標 width:幅 height:高さ uvid:登録するuv番号
bool TextureContainer::AddUV(int texNum, int left, int top, int width, int height, int uvid)
{
	if (data.find(texNum) != data.end())
	{

		XMFLOAT4 rect;
		rect.z = (float)width / data[texNum].metadata.width;
		rect.x = (float)left / data[texNum].metadata.width;
		rect.y = (float)top / data[texNum].metadata.height;
		rect.w = (float)height / data[texNum].metadata.height;

		data[texNum].uvrect[uvid] = rect;
		return true;
	}
	return false;
}

// 指定したTextureInfoのアドレスを取得します。texNum:テクスチャ番号
TextureInfo* TextureContainer::GetTexture(int texNum)
{
	TextureInfo* result = NULL;

	if (data.find(texNum) != data.end())
	{
		//すでに何らかのテクスチャが読み込まれていたら
		if (&data[texNum] != NULL){
			// そのTextureInfoのアドレスを返す
			//result.reset(&data[texNum]);
			result = &data[texNum];
		}
	}
	return(result);
}
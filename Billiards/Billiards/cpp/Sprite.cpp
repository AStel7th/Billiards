#include "../Header/Sprite.h"
//====================================================================
//  Sprite
//====================================================================
list<Sprite*> Sprite::drawObjectList;
unique_ptr<GraphicsPipeline> Sprite::m_pGraphicsPipeline;
ComPtr<ID3D11Buffer> Sprite::m_pVertexBuffer;
ComPtr<ID3D11Buffer> Sprite::m_pObjBuffers;
ComPtr<ID3D11SamplerState> Sprite::m_pSamplerState;

// �R�s�[
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

// �X�N���[���T�C�Y���w�肵�܂��B
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

// �ÓI�̈搶��
void Sprite::Create() 
{
	Direct3D11 &d3d11 = Direct3D11::Instance();

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	m_pGraphicsPipeline = make_unique<GraphicsPipeline>();

	// ���X�^���C�U�[�X�e�[�g���쐬����
	m_pGraphicsPipeline->CreateRasterizerState(D3D11_CULL_MODE::D3D11_CULL_BACK);

	// �[�x�X�e���V���X�e�[�g���쐬����
	m_pGraphicsPipeline->CreateDepthStencilState(FALSE, D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL);

	// �u�����h�X�e�[�g����`�����ō쐬����
	GraphicsPipeline::UEBLEND_STATE BlendStateType[1] = { GraphicsPipeline::UEBLEND_STATE::ALIGNMENT };
	m_pGraphicsPipeline->CreateBlendState(BlendStateType, 1);

	// �V�F�[�_�[���쐬����
	//#if defined(DEBUG) || defined(_DEBUG)
	m_pGraphicsPipeline->CreateVertexShaderFromFile(_T("HLSL/Sprite_Pass0.hlsl"), "Sprite_Pass0_VS_Main", layout, _countof(layout));
	m_pGraphicsPipeline->CreatePixelShaderFromFile(_T("HLSL/Sprite_Pass0.hlsl"), "Sprite_Pass0_PS_Main");

	Sprite::VERTEX vtx[] = {
		XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f),
		XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)

	};

	// ���_�o�b�t�@���쐬����
	m_pVertexBuffer = Buffers::CreateVertexBuffer(d3d11.pD3DDevice.Get(),vtx, sizeof(vtx), 0);

	// �T���v���[�X�e�[�g���쐬����
	m_pSamplerState = Samplers::CreateSamplerState(d3d11.pD3DDevice.Get(),D3D11_TEXTURE_ADDRESS_CLAMP);

	// �萔�o�b�t�@
	m_pObjBuffers = m_pGraphicsPipeline->CreateConstantBuffer(nullptr, sizeof(Sprite::OBJ_BUFFER), D3D11_CPU_ACCESS_WRITE);
}

// �������
void Sprite::end_last()
{

}


// �e�N�X�`���ݒ�
void Sprite::setTexture(TextureInfo* tex)
{
	_ASSERT(tex);
	this->tex = tex;

	CreateShaderResourceView(d3d11.pD3DDevice.Get(), tex->image.GetImages(), tex->image.GetImageCount(), tex->metadata, &m_pSRView);
	setSize(this->tex->metadata.width, this->tex->metadata.height);

}


#pragma region �������[�������[
// �|���T�C�Y�w��
void Sprite::setSize(int w, int h) {
	polyW = w;
	polyH = h;
}

//�T�C�Y�Q�b����
void Sprite::getSize(int* w, int* h)
{
	*w = polyW;
	*h = polyH;
}

// �X�N���[���T�C�Y�w��
void Sprite::setScreenSize(int w, int h) {
	scW = w;
	scH = h;
}

// ���S�w��
void Sprite::setPivot(float x, float y) {
	pivotX = x;
	pivotY = y;
}

// ���W
void Sprite::setPos(float x, float y) {
	posX = x;
	posY = y;
}

// ��]
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

// UV�؂���w��
void Sprite::setUV(float l, float t, float w, float h) {
	uvLeft = l;
	uvTop = t;
	uvW = w;
	uvH = h;
}

// ���ݒ�
void Sprite::setColor(XMFLOAT4& col) {
	color = col;
}

XMFLOAT4& Sprite::getColor() {
	return color;
}

// �v���C�I���e�B�ݒ�
void Sprite::setPriority(float z) {
	posZ = z;
}

float Sprite::getPriority() {
	return posZ;
}

#pragma endregion 

// �`�惊�X�g�ɒǉ�
void Sprite::Draw() {
	drawObjectList.push_back(this);
}

// �`�惊�X�g��`��
void Sprite::DrawAll()
{
	HRESULT hr;

	Direct3D11 &d3d11 = Direct3D11::Instance();

	// 2D�`��p�ˉe�ϊ��s��
	XMMATRIX proj;
	proj = XMMatrixIdentity();

	list<Sprite*>::iterator it;
	for (it = drawObjectList.begin(); it != drawObjectList.end(); it++)
	{
		Sprite* sp = (*it);

		// �e��O���t�B�b�N�p�C�v���C����ݒ�
		sp->m_pGraphicsPipeline->SetVertexShader();
		sp->m_pGraphicsPipeline->SetHullShader();
		sp->m_pGraphicsPipeline->SetDomainShader();
		sp->m_pGraphicsPipeline->SetGeometryShader();
		sp->m_pGraphicsPipeline->SetPixelShader();
		sp->m_pGraphicsPipeline->SetRasterizerState();
		sp->m_pGraphicsPipeline->SetDepthStencilState();
		sp->m_pGraphicsPipeline->SetBlendState();

		// ���_�o�b�t�@�ݒ�
		UINT stride = sizeof(Sprite::VERTEX);
		UINT offset = 0;
		d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 1, sp->m_pVertexBuffer.GetAddressOf(), &stride, &offset);

		// �v���~�e�B�u �^�C�v����уf�[�^�̏����Ɋւ������ݒ�
		d3d11.pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// �s�N�Z���V�F�[�_�[�̃T���v���[�X�e�[�g��ݒ肷��
		d3d11.pD3DDeviceContext->PSSetSamplers(0, 1, sp->m_pSamplerState.GetAddressOf());

		// �f�J�[���}�b�v��ݒ肷��
		d3d11.pD3DDeviceContext->PSSetShaderResources(0, 1, sp->m_pSRView.GetAddressOf());



		proj = XMMatrixOrthographicLH((float)sp->scW, (float)-sp->scH, 0, 1);
		proj.r[3].m128_f32[0] = -1.0f;
		proj.r[3].m128_f32[1] = 1.0f;

		XMMATRIX world, scale, rot;
		world = XMMatrixIdentity();
		world = XMMatrixScaling((float)sp->polyW, (float)sp->polyH, 1.0f);
		scale = XMMatrixScaling(sp->scaleX, sp->scaleY, 1.0f);
		rot = XMMatrixRotationZ(sp->rad);
		world.r[3].m128_f32[0] = -sp->pivotX;		// �s�{�b�g���I�t�Z�b�g
		world.r[3].m128_f32[1] = -sp->pivotY;
		world = world * rot * scale;

		world.r[3].m128_f32[0] += sp->posX;
		world.r[3].m128_f32[1] += sp->posY;


		D3D11_MAPPED_SUBRESOURCE mappedResourceObj;
		Sprite::OBJ_BUFFER* cBufferObj;

		if (FAILED(hr = d3d11.pD3DDeviceContext->Map(sp->m_pObjBuffers.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceObj)))
			throw(UException(hr, _T("Sprite::DrawAll()�ŃG���[���������܂����B���b�N�ł��܂���")));

		cBufferObj = (Sprite::OBJ_BUFFER*)mappedResourceObj.pData;

		cBufferObj->World = XMMatrixTranspose(world);
		cBufferObj->Proj = XMMatrixTranspose(proj);

		cBufferObj->Uv_left = sp->uvLeft;
		cBufferObj->Uv_top = sp->uvTop;
		cBufferObj->Uv_width = sp->uvW;
		cBufferObj->Uv_height = sp->uvH;
		cBufferObj->Color = sp->color;


		d3d11.pD3DDeviceContext->Unmap(sp->m_pObjBuffers.Get(), 0);

		// ���_�V�F�[�_�[�ɒ萔�o�b�t�@��ݒ肷��B;
		d3d11.pD3DDeviceContext->VSSetConstantBuffers(0, 1, sp->m_pObjBuffers.GetAddressOf());

		// �s�N�Z���V�F�[�_�[�ɒ萔�o�b�t�@��ݒ肷��B
		d3d11.pD3DDeviceContext->PSSetConstantBuffers(0, 1, sp->m_pObjBuffers.GetAddressOf());

		// �`��
		d3d11.pD3DDeviceContext->Draw(4, 0);
	}

	ClearDrawList();
}

// �`�惊�X�g�N���A
void Sprite::ClearDrawList()
{
	drawObjectList.clear();
}

// �ȒP������ tex:�R���e�i����擾����TextureInfo�̃A�h���X x:�`��x���W y:�`��y���W
void Sprite::Init(TextureInfo* tex, float x, float y)
{
	setTexture(tex);
	setPos(x, y);
}

// �ȒP�������i���W�𒆐S�ɕ\�����܂��j tex:�R���e�i����擾����TextureInfo�̃A�h���X x:�`��x���W y:�`��y���W
void Sprite::InitCenter(TextureInfo *tex, float x, float y)
{
	setTexture(tex);
	setPos(x, y);
	setPivot(tex->metadata.width / 2.0f, tex->metadata.height / 2.0f);
}

// AddUV()�Őݒ肵��uv���W�Z�b�g����X�v���C�g��uv���W���Z�b�g���܂��B���S�_���ݒ肵�Ăˁ@uvid:uv�ԍ� x,y:���S�_ �����l0
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

// AddUV()�Őݒ肵��uv���W�Z�b�g����X�v���C�g��uv���W���Z�b�g���܂��B�@���S�_�𒆐S�ɂ��邨�@uvid:uv�ԍ�
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

// �e�N�X�`����ǂݍ��݂܂��B t_pTextureName�F�t�@�C�����@texNum:����ID
int TextureContainer::LoadTexture(const WCHAR* t_pTextureName, int texNum)
{
	int result = -1;

	if (data.find(texNum) == data.end())
	{
		//// �e�N�X�`���̍쐬
		HRESULT hr;

		setlocale(LC_ALL, "japanese");

		hr = LoadFromWICFile(t_pTextureName, 0, &data[texNum].metadata, data[texNum].image);
		if (SUCCEEDED(hr)){
			result = 0;
		}
	}
	return(result);
}

// UV�ݒ肷�邨 texNum:�ݒ肷��e�N�X�`��ID left:��_��x���W top:��_��y���W width:�� height:���� uvid:�o�^����uv�ԍ�
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

// �w�肵��TextureInfo�̃A�h���X���擾���܂��BtexNum:�e�N�X�`���ԍ�
TextureInfo* TextureContainer::GetTexture(int texNum)
{
	TextureInfo* result = NULL;

	if (data.find(texNum) != data.end())
	{
		//���łɉ��炩�̃e�N�X�`�����ǂݍ��܂�Ă�����
		if (&data[texNum] != NULL){
			// ����TextureInfo�̃A�h���X��Ԃ�
			//result.reset(&data[texNum]);
			result = &data[texNum];
		}
	}
	return(result);
}
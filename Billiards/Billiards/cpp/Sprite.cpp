#include "../Header/Sprite.h"
//#include "../Header/GraphicsPipeline.h"
//====================================================================
//  Sprite
//====================================================================
list<Sprite*> Sprite::drawObjectList;
//unique_ptr<GraphicsPipeline> Sprite::m_pGraphicsPipeline;
VertexShader* Sprite::pVertexShader = nullptr;
PixelShader* Sprite::pPixelShader = nullptr;
ComPtr<ID3D11Buffer> Sprite::m_pVertexBuffer;
ComPtr<ID3D11Buffer> Sprite::m_pObjBuffers;
ComPtr<ID3D11InputLayout> Sprite::pInputLayout;
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

// �ÓI�̈搶��
void Sprite::Create() 
{
	Direct3D11 &d3d11 = Direct3D11::Instance();

	// �V�F�[�_�[���쐬����
	//#if defined(DEBUG) || defined(_DEBUG)
	pVertexShader = NEW VertexShader(d3d11.pD3DDevice.Get(), L"HLSL/Sprite_Pass0.hlsl", "Sprite_Pass0_VS_Main");
	pPixelShader = NEW PixelShader(d3d11.pD3DDevice.Get(), L"HLSL/Sprite_Pass0.hlsl", "Sprite_Pass0_PS_Main");

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

// �������
void Sprite::end_last()
{

}


// �e�N�X�`���ݒ�
void Sprite::SetTexture(TextureInfo* tex)
{
	_ASSERT(tex);
	this->tex = tex;

	CreateShaderResourceView(d3d11.pD3DDevice.Get(), tex->image.GetImages(), tex->image.GetImageCount(), tex->metadata, &m_pSRView);
	SetSize(this->tex->metadata.width, this->tex->metadata.height);

}


#pragma region �������[�������[
// �|���T�C�Y�w��
void Sprite::SetSize(int w, int h) {
	polyW = w;
	polyH = h;
}

//�T�C�Y�Q�b����
void Sprite::GetSize(int* w, int* h)
{
	*w = polyW;
	*h = polyH;
}

// �X�N���[���T�C�Y�w��
void Sprite::SetScreenSize(int w, int h) {
	scW = w;
	scH = h;
}

// ���S�w��
void Sprite::SetPivot(float x, float y) {
	pivotX = x;
	pivotY = y;
}

// ���W
void Sprite::SetPos(float x, float y) {
	posX = x;
	posY = y;
}

// ��]
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

// UV�؂���w��
void Sprite::SetUV(float l, float t, float w, float h) {
	uvLeft = l;
	uvTop = t;
	uvW = w;
	uvH = h;
}

// ���ݒ�
void Sprite::SetColor(XMFLOAT4& col) {
	color = col;
}

XMFLOAT4& Sprite::GetColor() {
	return color;
}

// �v���C�I���e�B�ݒ�
void Sprite::SetPriority(float z) {
	posZ = z;
}

float Sprite::GetPriority() {
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

	d3d11.pD3DDeviceContext->VSSetShader(pVertexShader->GetShader(), nullptr, 0);
	d3d11.pD3DDeviceContext->PSSetShader(pPixelShader->GetShader(), nullptr, 0);
	d3d11.pD3DDeviceContext->IASetInputLayout(pInputLayout.Get());

	// 2D�`��p�ˉe�ϊ��s��
	XMMATRIX proj;
	proj = XMMatrixIdentity();

	list<Sprite*>::iterator it;
	for (it = drawObjectList.begin(); it != drawObjectList.end(); it++)
	{
		Sprite* sp = (*it);

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
	SetTexture(tex);
	SetPos(x, y);
}

// �ȒP�������i���W�𒆐S�ɕ\�����܂��j tex:�R���e�i����擾����TextureInfo�̃A�h���X x:�`��x���W y:�`��y���W
void Sprite::InitCenter(TextureInfo *tex, float x, float y)
{
	SetTexture(tex);
	SetPos(x, y);
	SetPivot(tex->metadata.width / 2.0f, tex->metadata.height / 2.0f);
}

// AddUV()�Őݒ肵��uv���W�Z�b�g����X�v���C�g��uv���W���Z�b�g���܂��B���S�_���ݒ肵�Ăˁ@uvid:uv�ԍ� x,y:���S�_ �����l0
void Sprite::SetUVFromTex(int uvid, float x, float y)
{
	if (tex->uvrect.find(uvid) != tex->uvrect.end())
	{
		SetUV((float)tex->uvrect[uvid].x, (float)tex->uvrect[uvid].y, (float)tex->uvrect[uvid].z, (float)tex->uvrect[uvid].w);
		SetSize((int)(tex->metadata.width * tex->uvrect[uvid].z), (int)(tex->metadata.height * tex->uvrect[uvid].w));
		SetPivot(x, y);
		//setScale(1.0f * tex->uvrect[uvid].width, 1.0f * tex->uvrect[uvid].height);
	}

}

// AddUV()�Őݒ肵��uv���W�Z�b�g����X�v���C�g��uv���W���Z�b�g���܂��B�@���S�_�𒆐S�ɂ��邨�@uvid:uv�ԍ�
void Sprite::SetUVFromTexCenter(int uvid)
{
	if (tex->uvrect.find(uvid) != tex->uvrect.end())
	{
		SetUV((float)tex->uvrect[uvid].x, (float)tex->uvrect[uvid].y, (float)tex->uvrect[uvid].z, (float)tex->uvrect[uvid].w);
		SetSize(int(tex->metadata.width * tex->uvrect[uvid].z), (int)(tex->metadata.height * tex->uvrect[uvid].w));
		SetPivot((float)polyW / 2, (float)polyH / 2);
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

// UV�ݒ肷�� texNum:�ݒ肷��e�N�X�`��ID left:��_��x���W top:��_��y���W width:�� height:���� uvid:�o�^����uv�ԍ�
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
#pragma once
#include "Direct3D11.h"
#include <map>
#include <list>
#include <locale.h>
#include "Shader.h"

//====================================================================
//  �X�v���C�g�֘A
//====================================================================

// �e�N�X�`���̏��
class TextureInfo 
{
public:
	TexMetadata metadata;
	ScratchImage image;    // �e�N�X�`���ւ̃|�C���^
	XMFLOAT4 uvrect;            // uv�L��

	TextureInfo();
	TextureInfo(const TextureInfo& r);
	virtual ~TextureInfo();

	TextureInfo &operator =(const TextureInfo &r);

	void AddUV(int texNum, int left, int top, int width, int height, int uvid);// UV�ݒ肷�邨 texNum:�ݒ肷��e�N�X�`��ID left:��_��x���W top:��_��y���W width:�� height:���� uvid:�o�^����uv�ԍ�
};

// �e�N�X�`���܂Ƃ߂��N���X
class TextureContainer {
	map<string, TextureInfo> data; // �ǂݍ��񂾃f�[�^
protected:
	TextureContainer();
public:

	// �V���O���g���I�u�W�F�N�g���擾
	static TextureContainer &Instance()
	{
		static TextureContainer inst;
		return inst;
	}

	virtual ~TextureContainer();
	TextureInfo LoadTexture(const string& t_pTextureName);   // �e�N�X�`����ǂݍ��݂܂��B t_pTextureName�F�t�@�C�����@texNum:����ID

private:

};

class Sprite
{
	// ���_��`
	typedef struct _VERTEX
	{
		XMFLOAT3 pos;     // ���_�̍��W
		XMFLOAT2 texel;   // �e�N�Z�����W
	}VERTEX;

	// �萔�o�b�t�@�̒�`
	typedef struct _CONSTANT_BUFFER
	{
		XMFLOAT2 Position;    // �X�N���[�����W�n��ł̕\���ʒu
		XMFLOAT2 TexelOffset; // �e�N�X�`���[��̃e�N�Z���ʒu���w�肷��I�t�Z�b�g�l

		_CONSTANT_BUFFER() {};
		_CONSTANT_BUFFER(XMFLOAT2 position, XMFLOAT2 texelOffset)
		{
			::CopyMemory(&Position, &position, sizeof(XMFLOAT2));
			::CopyMemory(&TexelOffset, &texelOffset, sizeof(XMFLOAT2));
		};
	}CONSTANT_BUFFER;

	// �萔�o�b�t�@�̒�`
	typedef struct _OBJ_BUFFER
	{
		XMFLOAT4X4 World;
		XMFLOAT4X4 Proj;
		FLOAT Uv_left;
		FLOAT Uv_top;
		FLOAT Uv_width;
		FLOAT Uv_height;
		XMFLOAT4 Color;

	}OBJ_BUFFER;

	Direct3D11 &d3d11 = Direct3D11::Instance();

	// ���_�o�b�t�@
	static ComPtr<ID3D11Buffer> m_pVertexBuffer;

	static list<Sprite*> drawObjectList;		// �`��Ώۃ��X�g

												// �V�F�[�_�[�p�萔�o�b�t�@
	static ComPtr<ID3D11Buffer> m_pObjBuffers;

	//�C���v�b�g���C�A�E�g
	static ComPtr<ID3D11InputLayout> pInputLayout;

	// �V�F�[�_�[���\�[�X�r���[
	ComPtr<ID3D11ShaderResourceView> m_pSRView;

	// �T���v���[�X�e�[�g
	static ComPtr<ID3D11SamplerState> m_pSamplerState;

	static unique_ptr<VertexShader> pVertexShader;
	static unique_ptr<PixelShader> pPixelShader;

	TextureInfo* tex;		// �e�N�X�`��
	shared_ptr<TextureInfo> subtex;		// �e�N�X�`��

	int scW, scH;			// �X�N���[���T�C�Y
	int polyW, polyH;		// �|���S���T�C�Y
	float pivotX, pivotY;	// �s�{�b�g���W
	float posX, posY, posZ;	// �ʒu
	float rad;				// ��]�p�x�i���W�A���j
	float scaleX, scaleY;	// �X�P�[��
	float uvLeft, uvTop;	// UV������W
	float uvW, uvH;			// UV����
	XMFLOAT4 color;
	float SubuvLeft, SubuvTop;	// subUV������W
	float SubuvW, SubuvH;			// subUV����

									// �R�s�[
	void copy(const Sprite &r);

public:
	Sprite();
	Sprite(int screenWidth, int screenHeight); // �X�N���[���T�C�Y���w�肵�܂�
	Sprite(const Sprite &r);
	~Sprite();



	Sprite &operator =(const Sprite &r) { copy(r); return *this; }

	// �ÓI�̈搶��
	static void Create();

	// �I��
	static void end_last();

	// �e�N�X�`���ݒ�
	void SetTexture(TextureInfo* tex);

	void SetSubTexture(TextureInfo* tex);

	// �|���T�C�Y�w��
	void SetSize(int w, int h);
	void GetSize(int* w, int* h);
	XMINT2 GetSize();

	// �X�N���[���T�C�Y�w��
	void SetScreenSize(int w, int h);

	// �s�{�b�g�w��
	void SetPivot(float x, float y);

	// �p���w��
	void SetPos(float x, float y);
	void GetPos(float* x, float* y);
	XMFLOAT2* GetPos() { return &XMFLOAT2(posX, posY); };
	void SetRotate(float deg);
	float GetRotate();
	void SetScale(float sx, float sy);

	// UV�؂���w��
	void SetUV(float l, float t, float w, float h);
	// UV�؂���w��
	void SetSubUV(float l, float t, float w, float h);

	// ���ݒ�
	void SetColor(XMFLOAT4& a);
	XMFLOAT4& GetColor();

	// �v���C�I���e�B�ݒ�
	void SetPriority(float z);
	float GetPriority();

	// �`�惊�X�g�ɒǉ�
	void Draw();

	// �`�惊�X�g��`��
	static void DrawAll();

	// �`�惊�X�g�N���A
	static void Sprite::ClearDrawList();

	// �ȒP������
	void Init(TextureInfo* tex, float x = 0.0f, float y = 0.0f);
	// �ȒP������ ���S�_���摜�̒����Ɏw��
	void InitCenter(TextureInfo* tex, float x = 0.0f, float y = 0.0f);
	// �e�N�X�`���ɐݒ肵��uv���Z�b�g���܂�
	void SetUVFromTex(int uvid, float x = 0.0f, float y = 0.0f);
	void SetUVFromTexCenter(int uvid);
};
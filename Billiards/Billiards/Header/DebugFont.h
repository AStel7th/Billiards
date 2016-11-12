#ifndef UDRAWFONT_H
#define UDRAWFONT_H

#include "Direct3D11.h"
#include "../HLSL/DebugFont_VS_Main.h"
#include "../HLSL/DebugFont_PS_Main.h"

class GraphicsPipeline;

class DebugFont
{
private:
	Direct3D11 &d3d11 = Direct3D11::Instance();

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
		XMFLOAT4 Color;       // ���_�J���[

		_CONSTANT_BUFFER(){};
		_CONSTANT_BUFFER(XMFLOAT2 position, XMFLOAT2 texelOffset, XMFLOAT4 color)
		{
			::CopyMemory(&Position, &position, sizeof(XMFLOAT2));
			::CopyMemory(&TexelOffset, &texelOffset, sizeof(XMFLOAT2));
			::CopyMemory(&Color, &color, sizeof(XMFLOAT4));
		};
	}CONSTANT_BUFFER;

	// ���_�o�b�t�@
	ComPtr<ID3D11Buffer> m_pVertexBuffer;

	// �V�F�[�_�[���\�[�X�r���[
	ComPtr<ID3D11ShaderResourceView> m_pSRView;

	// �T���v���[�X�e�[�g
	ComPtr<ID3D11SamplerState> m_pSamplerState;

	// �V�F�[�_�[�p�萔�o�b�t�@
	ComPtr<ID3D11Buffer> m_pConstantBuffers;

	GraphicsPipeline* m_pGraphicsPipeline;

	float m_FontWidth;   // �|���S���̉���
	float m_FontHeight;  // �|���S���̏c��
	static const int m_FontCnt = 95;     // �e�N�X�`���[��̃t�H���g��

	TCHAR m_pStr[512];
	XMFLOAT2 m_Position;
	XMFLOAT4 m_Color;

public:
	DebugFont();
	virtual ~DebugFont();
	void Invalidate();

	// �쐬����
	// �t�H���g�T�C�Y�́A�ˉe��ԏ�ł̕��Ŏw�肷��B1���w�肷��ƃX�N���[�������ς����̑傫���ƂȂ�
	// �t�H���g�T�C�Y�͍쐬���ɐݒ肵���l����ŕύX�ł��Ȃ�.
	void CreateMesh(float FontWidth, float FontHeight);

	// �����͔��p�p�����Ɣ��p�L���̂݃T�|�[�g����.����ȊO�̕������p�����[�^�ɓn�����ꍇ�̓���͕ۏ؂��Ȃ�.
	// �\���ʒu�͎ˉe��ԏ�ł̍��W�Őݒ肷��
	void Update(TCHAR* pStr, XMFLOAT2* pPosition, XMFLOAT4* pColor);

	// �f�o�b�N�p�̃e�L�X�g��`�悷��
	void Draw();
};


// **********************************************************************************
// FPS�`��N���X
// **********************************************************************************

class UFPS : private DebugFont
{
private:
	DWORD m_PrevTime;      // �O�񎞊�
	WORD  m_FrameCounter;  // FPS���v��
	DWORD m_Frame;         // FPS�v���l

public:
	UFPS();
	virtual ~UFPS();
	void Invalidate();
	void CreateMesh();
	void Update();
	void Draw();
};


//==================================================================================
//�f�o�b�O�t�H���g�`��N���X
//==================================================================================

class DebugText : private DebugFont
{
private:

public:
	DebugText();
	virtual ~DebugText();
	void Invalidate();
	void CreateMesh();
	void SetText(TCHAR* tchar, XMFLOAT2 *pos);
	void Draw();
};
#endif
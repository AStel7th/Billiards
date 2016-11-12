#ifndef DRAWSYSTEM_H
#define DRAWSYSTEM_H

#include <map>
#include <memory>
#include "Sprite.h"
#include "Shader.h"
#include "GameObject.h"

const UINT MAX_BONES = 255;

enum struct SHADER_ID
{
	VERTEX,
	PIXEL,
	GEOMETORY,
	COMPUTE,
	HULL,
	DOMEIN,
	ALL
};

struct SHADER_GLOBAL
{
	XMFLOAT4 lightDir;
	XMFLOAT4 eye;
};

//�V�F�[�_�[�ɓn���{�[���s��z��
struct SHADER_GLOBAL_BONES
{
	XMFLOAT4X4 mBone[MAX_BONES];
	SHADER_GLOBAL_BONES()
	{
		for (int i = 0; i<MAX_BONES; i++)
		{
			XMStoreFloat4x4(&mBone[i], XMMatrixIdentity());
		}
	}
};

struct CBMATRIX
{
	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 mWVP;
};

/////////////////////////////////////////////////
// DrawSystem
//
//    �V���O���g���@�`��Ǘ�
////////////////////////////////////////
class DrawSystem
{

private:
	Direct3D11 &d3d11 = Direct3D11::Instance();

	ComPtr<ID3D11RasterizerState> pRS;
	ComPtr<ID3D11RasterizerState> pRSnoCull;
	ComPtr<ID3D11BlendState> pBlendState;
	ComPtr<ID3D11DepthStencilState>	pDepthStencilState;

	unique_ptr<VertexShader> pvsFBX = nullptr;
	unique_ptr<VertexShader> pvsFBXInstancing = nullptr;
	unique_ptr<VertexShader> pvsVertexColor = nullptr;
	unique_ptr<VertexShader> pvsFBXAnimation = nullptr;
	unique_ptr<VertexShader> pPlaneVS_UV = nullptr;
	unique_ptr<PixelShader> pPlanePS_UV = nullptr;
	unique_ptr<PixelShader> ppsFBX = nullptr;
	unique_ptr<PixelShader> ppsVertexColor = nullptr;
	unique_ptr<PixelShader> ppsFBXAnimation = nullptr;

	ComPtr<ID3D11InputLayout> pInputLayoutUV;
	ComPtr<ID3D11InputLayout> pInputLayoutUV1;
	ComPtr<ID3D11InputLayout> pInputLayoutUV2;
	ComPtr<ID3D11InputLayout> pInputLayoutColor;

	ComPtr<ID3D11Buffer> pcBuffer0 = nullptr;
	ComPtr<ID3D11Buffer> pcBuffer1 = nullptr;
	ComPtr<ID3D11Buffer> pcBoneBuffer = nullptr;


protected:
	// �V���O���g���Ȃ̂ŃR���X�g���N�^���v���g�^�C�v�錾���܂�
	DrawSystem();
public:
	virtual ~DrawSystem();

	// �V���O���g���I�u�W�F�N�g���擾
	static DrawSystem &Instance()
	{
		static DrawSystem inst;
		return inst;
	}

	// �V�X�e��������������
	virtual HRESULT Init(unsigned int msaaSamples);

	// ���X�g�ɓo�^���ꂽ�^�X�N��`�悷��
	virtual bool Draw();

	ID3D11Buffer* GetCBuffer(int i);
	void SetCBufferVS(int i);
};

#endif //DRAWSYSTEM_H
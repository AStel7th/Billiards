#pragma once
#include "Direct3D11.h"

class GraphicsPipeline
{
private:
	Direct3D11 &d3d11 = Direct3D11::Instance();

	// ���̓��C�A�E�g
	ComPtr<ID3D11InputLayout> m_pLayout;

	// ���_�V�F�[�_�[
	// �Ƃ肠�����X�g���[���A�E�g�v�b�g�͍l�����Ȃ�
	ComPtr<ID3D11VertexShader> m_pVertexShader;

	// �n���V�F�[�_�[
	ComPtr<ID3D11HullShader> m_pHullShader;

	// �h���C���V�F�[�_�[
	ComPtr<ID3D11DomainShader> m_pDomainShader;

	// �W�I���g���V�F�[�_�[
	ComPtr<ID3D11GeometryShader> m_pGeometryShader;

	// �s�N�Z���V�F�[�_�[
	ComPtr<ID3D11PixelShader> m_pPixelShader;

	// �R���s���[�g�V�F�[�_�[
	ComPtr<ID3D11ComputeShader> m_pComputeShader;

	// ���X�^���C�U�X�e�[�g
	ComPtr<ID3D11RasterizerState> m_pRasterizerState;

	// �[�x�X�e���V���X�e�[�g
	ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;

	// �u�����h�X�e�[�g
	ComPtr<ID3D11BlendState> m_pBlendState;

public:
	enum class UEBLEND_STATE
	{
		NONE = 0,
		ADD = 1,
		ALIGNMENT = 2,
		DEPTH_TEST = 3,
		TEST = 4,
		DEFAULT = 5
	};

	enum class UEACCESS_FLAG
	{
		CPU_NONE_GPU_READWRITE = 0,   // CPU�ɂ��A�N�Z�X�Ȃ��AGPU�ɂ��Ǎ��݂Ə�����.
		UPDATE_SUBRESOURCE = 1,       // ID3D11DeviceContext::UpdateSubresource()���g�p���ăR�s�[���s���ꍇ
		CPU_WRITE_GPU_READ = 2,       // CPU�ɂ�鏑���݁AGPU�ɂ��Ǎ���
	};

	GraphicsPipeline();
	virtual ~GraphicsPipeline();

	// ���_�V�F�[�_�[���t�@�C������쐬����
	void CreateVertexShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[],
		D3D11_INPUT_ELEMENT_DESC pInputElementDescs[], UINT NumElements);

	// ���_�V�F�[�_�[������������쐬����
	void CreateVertexShaderFromMemory(BYTE* pShader, size_t size,
		D3D11_INPUT_ELEMENT_DESC pInputElementDescs[], UINT NumElements);

	// �n���V�F�[�_�[���t�@�C������쐬����
	void CreateHullShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[]);

	// �n���V�F�[�_�[������������쐬����
	void CreateHullShaderFromMemory(BYTE* pShader, size_t size);

	// �h���C���V�F�[�_�[���t�@�C������쐬����
	void CreateDomainShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[]);

	// �h���C���V�F�[�_�[������������쐬����
	void CreateDomainShaderFromMemory(BYTE* pShader, size_t size);

	// �W�I���g���V�F�[�_�[���t�@�C������쐬����
	void CreateGeometryShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[]);

	// �W�I���g���V�F�[�_�[������������쐬����
	void CreateGeometryShaderFromMemory(BYTE* pShader, size_t size);

	// �s�N�Z���V�F�[�_�[���t�@�C������쐬����
	void CreatePixelShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[]);

	// �s�N�Z���V�F�[�_�[������������쐬����
	void CreatePixelShaderFromMemory(BYTE* pShader, size_t size);

	// �R���s���[�g�V�F�[�_�[���t�@�C������쐬����
	void CreateComputeShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[]);

	// �R���s���[�g�V�F�[�_�[������������쐬����
	void CreateComputeShaderFromMemory(BYTE* pShader, size_t size);

	// ���X�^���C�U�X�e�[�g���쐬
	void CreateRasterizerState(D3D11_CULL_MODE CullMode, D3D11_FILL_MODE FillMode = D3D11_FILL_SOLID, BOOL fcc = TRUE);

	// �[�x�X�e���V���X�e�[�g���쐬
	void CreateDepthStencilState(BOOL DepthEnable, D3D11_DEPTH_WRITE_MASK DepthWriteEnabled, D3D11_COMPARISON_FUNC DepthFunc = D3D11_COMPARISON_LESS);

	// �u�����h�X�e�[�g���쐬
	void CreateBlendState(UEBLEND_STATE* BlendStateType, UINT BlendStateTypeLength, BOOL AlphaToCoverageEnable = FALSE);

	// �萔�o�b�t�@���쐬����
	ComPtr<ID3D11Buffer> CreateConstantBuffer(void* pData, size_t size, UINT CPUAccessFlag);

	// ���_�V�F�[�_�[���f�o�C�X�ɐݒ肷��
	inline void SetVertexShader()
	{
		// ���̓A�Z���u���[ �X�e�[�W��ݒ�
		d3d11.pD3DDeviceContext->IASetInputLayout(m_pLayout.Get());
		// ���_�V�F�[�_�[���f�o�C�X�ɐݒ肷��B
		d3d11.pD3DDeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	}

	// �n���V�F�[�_�[���f�o�C�X�ɐݒ肷��
	inline void SetHullShader()
	{
		d3d11.pD3DDeviceContext->HSSetShader(m_pHullShader.Get(), nullptr, 0);
	}

	// �h���C���V�F�[�_�[���f�o�C�X�ɐݒ肷��
	inline void SetDomainShader()
	{
		d3d11.pD3DDeviceContext->DSSetShader(m_pDomainShader.Get(), nullptr, 0);
	}

	// �W�I���g���V�F�[�_�[���f�o�C�X�ɐݒ肷��
	inline void SetGeometryShader()
	{
		d3d11.pD3DDeviceContext->GSSetShader(m_pGeometryShader.Get(), nullptr, 0);
	}

	// �s�N�Z���V�F�[�_�[���f�o�C�X�ɐݒ肷��
	inline void SetPixelShader()
	{
		d3d11.pD3DDeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	}

	// �R���s���[�g�V�F�[�_�[���f�o�C�X�ɐݒ肷��
	inline void SetComputeShader()
	{
		d3d11.pD3DDeviceContext->CSSetShader(m_pComputeShader.Get(), nullptr, 0);
	}

	// ���X�^���C�U�X�e�[�g���f�o�C�X�ɐݒ肷��
	inline void SetRasterizerState()
	{
		d3d11.pD3DDeviceContext->RSSetState(m_pRasterizerState.Get());
	}

	// �[�x�X�e���V���X�e�[�g���f�o�C�X�ɐݒ肷��
	inline void SetDepthStencilState()
	{
		d3d11.pD3DDeviceContext->OMSetDepthStencilState(m_pDepthStencilState.Get(), 0);
	}

	// �u�����h�X�e�[�g���f�o�C�X�ɐݒ肷��
	inline void SetBlendState()
	{
		d3d11.pD3DDeviceContext->OMSetBlendState(m_pBlendState.Get(), nullptr, 0xffffffff);
	}
};
#include "../Header/GraphicsPipeline.h"

GraphicsPipeline::GraphicsPipeline()
{
	m_pLayout = nullptr;
	m_pVertexShader = nullptr;
	m_pHullShader = nullptr;
	m_pDomainShader = nullptr;
	m_pGeometryShader = nullptr;
	m_pPixelShader = nullptr;
	m_pRasterizerState = nullptr;
	m_pDepthStencilState = nullptr;
	m_pBlendState = nullptr;
}

GraphicsPipeline::~GraphicsPipeline()
{

}

// ���_�V�F�[�_�[���t�@�C������쐬����
void GraphicsPipeline::CreateVertexShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[],
	D3D11_INPUT_ELEMENT_DESC pLayoutDesc[], UINT NumElements)
{
	HRESULT hr = E_FAIL;
	CHAR* Profile = "vs_5_0";

	ComPtr<ID3DBlob> pBlob = nullptr;

	// �s����D��Őݒ肵�A�Â��`���̋L�q�������Ȃ��悤�ɂ���
	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
	// �œK�����x����ݒ肷��
#if defined(DEBUG) || defined(_DEBUG)
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	
	// �t�@�C�������ɃG�t�F�N�g���R���p�C������
	if (FAILED(hr = D3DCompileFromFile(pSrcFile, nullptr, nullptr, pFunctionName, Profile, Flag1, 0, &pBlob, nullptr)))
	{
		DWORD ss = GetLastError();
		TCHAR errstr[1024];
		_stprintf_s(errstr, _T("GraphicsPipeline::CreateVertexShaderFromFile()�ŃG���[���������܂����B�G�t�F�N�g�t�@�C���̃R���p�C���G���[�ł��B -> %s"), pSrcFile);
		throw(UException(hr, errstr));
	}
		
	// ���_�V�F�[�_�[������������쐬����
	CreateVertexShaderFromMemory((LPBYTE)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), pLayoutDesc, NumElements);
}

// ���_�V�F�[�_�[������������쐬����
void GraphicsPipeline::CreateVertexShaderFromMemory(BYTE* pShader, size_t size,
	D3D11_INPUT_ELEMENT_DESC m_pLayoutDesc[], UINT NumElements)
{
	HRESULT hr = E_FAIL;

	// �R���p�C���ς݃V�F�[�_�[����A���_�V�F�[�_�[ �I�u�W�F�N�g���쐬����
	if (FAILED(hr = d3d11.pD3DDevice->CreateVertexShader(pShader, size, nullptr, &m_pVertexShader)))
		throw(UException(hr, _T("GraphicsPipeline::CreateVertexShaderFromMemory()�ŃG���[���������܂����B���_�V�F�[�_�[�̍쐬�G���[�ł��B")));

	// �V�F�[�_�[�Ŏg�p�������̓o�b�t�@�[���L�q���邽�߂̓��̓��C�A�E�g�I�u�W�F�N�g���쐬����B
	if (FAILED(hr = d3d11.pD3DDevice->CreateInputLayout(m_pLayoutDesc, NumElements, pShader, size, &m_pLayout)))
		throw(UException(hr, _T("GraphicsPipeline::CreateVertexShaderFromMemory()�ŃG���[���������܂����B���̓��C�A�E�g�̍쐬�G���[�ł��B")));
}

// �n���V�F�[�_�[���t�@�C������쐬����
void GraphicsPipeline::CreateHullShaderFromFile(TCHAR pSrcFile[], CHAR  pFunctionName[])
{
	HRESULT hr = E_FAIL;
	CHAR* Profile = "hs_5_0";

	ComPtr<ID3DBlob> pBlob = nullptr;

	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	
	// �t�@�C�������ɃG�t�F�N�g���R���p�C������
	if (FAILED(hr = D3DCompileFromFile(pSrcFile, nullptr, nullptr, pFunctionName, Profile, Flag1, 0, &pBlob, nullptr)))
	{
		TCHAR errstr[1024];
		_stprintf_s(errstr, _T("GraphicsPipeline::CreateHullShaderFromFile()�ŃG���[���������܂����B�G�t�F�N�g�t�@�C���̃R���p�C���G���[�ł��B -> %s"), pSrcFile);
		throw(UException(hr, errstr));
	}

	// �n���V�F�[�_�[������������쐬����
	CreateHullShaderFromMemory((LPBYTE)pBlob->GetBufferPointer(), pBlob->GetBufferSize());
}

// �n���V�F�[�_�[������������쐬����
void GraphicsPipeline::CreateHullShaderFromMemory(BYTE* pShader, size_t size)
{
	HRESULT hr = E_FAIL;

	// �R���p�C���ς݃V�F�[�_�[����A�n���V�F�[�_�[ �I�u�W�F�N�g���쐬����
	if (FAILED(hr = d3d11.pD3DDevice->CreateHullShader(pShader, size, nullptr, &m_pHullShader)))
		throw(UException(hr, _T("GraphicsPipeline::CreateHullShaderFromMemory()�ŃG���[���������܂����B�n���V�F�[�_�[�̍쐬�G���[�ł��B")));
}
// �h���C���V�F�[�_�[���t�@�C������쐬����
void GraphicsPipeline::CreateDomainShaderFromFile(TCHAR pSrcFile[], CHAR  pFunctionName[])
{
	HRESULT hr = E_FAIL;
	CHAR* Profile = "ds_5_0";

	ComPtr<ID3DBlob> pBlob = nullptr;

	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	
	// �t�@�C�������ɃG�t�F�N�g���R���p�C������
	if (FAILED(hr = D3DCompileFromFile(pSrcFile, nullptr, nullptr, pFunctionName, Profile, Flag1, 0, &pBlob, nullptr)))
	{
		TCHAR errstr[1024];
		_stprintf_s(errstr, _T("GraphicsPipeline::CreateDomainShaderFromFile()�ŃG���[���������܂����B�G�t�F�N�g�t�@�C���̃R���p�C���G���[�ł��B -> %s"), pSrcFile);
		throw(UException(hr, errstr));
	}

	// �h���C���V�F�[�_�[������������쐬����
	CreateDomainShaderFromMemory((LPBYTE)pBlob->GetBufferPointer(), pBlob->GetBufferSize());
}

// �h���C���V�F�[�_�[������������쐬����
void GraphicsPipeline::CreateDomainShaderFromMemory(BYTE* pShader, size_t size)
{
	HRESULT hr = E_FAIL;

	// �R���p�C���ς݃V�F�[�_�[����A�h���C���V�F�[�_�[ �I�u�W�F�N�g���쐬����
	if (FAILED(hr = d3d11.pD3DDevice->CreateDomainShader(pShader, size, nullptr, &m_pDomainShader)))
		throw(UException(hr, _T("GraphicsPipeline::CreateDomainShaderFromMemory()�ŃG���[���������܂����B�h���C���V�F�[�_�[�̍쐬�G���[�ł��B")));
}

// �W�I���g���V�F�[�_�[���t�@�C������쐬����
void GraphicsPipeline::CreateGeometryShaderFromFile(TCHAR pSrcFile[], CHAR  pFunctionName[])
{
	HRESULT hr = E_FAIL;
	CHAR* Profile = "gs_5_0";

	ComPtr<ID3DBlob> pBlob = nullptr;

	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif


	// �t�@�C�������ɃG�t�F�N�g���R���p�C������
	if (FAILED(hr = D3DCompileFromFile(pSrcFile, nullptr, nullptr, pFunctionName, Profile, Flag1, 0, &pBlob, nullptr)))
	{
		TCHAR errstr[1024];
		_stprintf_s(errstr, _T("GraphicsPipeline::CreateGeometryShaderFromFile()�ŃG���[���������܂����B�G�t�F�N�g�t�@�C���̃R���p�C���G���[�ł��B -> %s"), pSrcFile);
		throw(UException(hr, errstr));
	}

	// �s�N�Z���V�F�[�_�[������������쐬����
	CreateGeometryShaderFromMemory((LPBYTE)pBlob->GetBufferPointer(), pBlob->GetBufferSize());
}

// �W�I���g���V�F�[�_�[������������쐬����
void GraphicsPipeline::CreateGeometryShaderFromMemory(BYTE* pShader, size_t size)
{
	HRESULT hr = E_FAIL;

	// �R���p�C���ς݃V�F�[�_�[����A�W�I���g���V�F�[�_�[ �I�u�W�F�N�g���쐬����
	if (FAILED(hr = d3d11.pD3DDevice->CreateGeometryShader(pShader, size, nullptr, &m_pGeometryShader)))
		throw(UException(hr, _T("GraphicsPipeline::CreateGeometryShaderFromMemory()�ŃG���[���������܂����B�W�I���g���V�F�[�_�[�̍쐬�G���[�ł��B")));
}

// �s�N�Z���V�F�[�_�[���t�@�C������쐬����
void GraphicsPipeline::CreatePixelShaderFromFile(TCHAR pSrcFile[],
	CHAR  pFunctionName[]
	)
{
	HRESULT hr = E_FAIL;
	CHAR* Profile = "ps_5_0";

	ComPtr<ID3DBlob> pBlob = nullptr;

	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	
	// �t�@�C�������ɃG�t�F�N�g���R���p�C������
	if (FAILED(hr = D3DCompileFromFile(pSrcFile, nullptr, nullptr, pFunctionName, Profile, Flag1, 0, &pBlob, nullptr)))
	{
		TCHAR errstr[1024];
		_stprintf_s(errstr, _T("GraphicsPipeline::CreatePixelShaderFromFile()�ŃG���[���������܂����B�G�t�F�N�g�t�@�C���̃R���p�C���G���[�ł��B -> %s"), pSrcFile);
		throw(UException(hr, errstr));
	}

	// �s�N�Z���V�F�[�_�[������������쐬����
	CreatePixelShaderFromMemory((LPBYTE)pBlob->GetBufferPointer(), pBlob->GetBufferSize());
}

// �s�N�Z���V�F�[�_�[������������쐬����
void GraphicsPipeline::CreatePixelShaderFromMemory(BYTE* pShader, size_t size)
{
	HRESULT hr = E_FAIL;

	// �R���p�C���ς݃V�F�[�_�[����A�s�N�Z���V�F�[�_�[ �I�u�W�F�N�g���쐬����
	if (FAILED(hr = d3d11.pD3DDevice->CreatePixelShader(pShader, size, nullptr, &m_pPixelShader)))
		throw(UException(hr, _T("GraphicsPipeline::CreatePixelShaderFromMemory()�ŃG���[���������܂����B�s�N�Z���V�F�[�_�[�̍쐬�G���[�ł��B")));
}

// �R���s���[�g�V�F�[�_�[������������쐬����
void GraphicsPipeline::CreateComputeShaderFromMemory(BYTE* pShader, size_t size)
{
	HRESULT hr = E_FAIL;

	// �R���p�C���ς݃V�F�[�_�[����A�R���s���[�g�V�F�[�_�[ �I�u�W�F�N�g���쐬����
	if (FAILED(hr = d3d11.pD3DDevice->CreateComputeShader(pShader, size, nullptr, &m_pComputeShader)))
		throw(UException(hr, _T("GraphicsPipeline::CreatePixelShaderFromMemory()�ŃG���[���������܂����B�s�N�Z���V�F�[�_�[�̍쐬�G���[�ł��B")));
}

// �R���s���[�g�V�F�[�_�[���t�@�C������쐬����
void GraphicsPipeline::CreateComputeShaderFromFile(TCHAR pSrcFile[],CHAR  pFunctionName[])
{
	HRESULT hr = E_FAIL;
	CHAR* Profile = "cs_5_0";

	ComPtr<ID3DBlob> pBlob = nullptr;

	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif


	// �t�@�C�������ɃG�t�F�N�g���R���p�C������
	if (FAILED(hr = D3DCompileFromFile(pSrcFile, nullptr, nullptr, pFunctionName, Profile, Flag1, 0, &pBlob, nullptr)))
	{
		TCHAR errstr[1024];
		_stprintf_s(errstr, _T("GraphicsPipeline::CreateComputeShaderFromFile()�ŃG���[���������܂����B�G�t�F�N�g�t�@�C���̃R���p�C���G���[�ł��B -> %s"), pSrcFile);
		throw(UException(hr, errstr));
	}

	// �R���s���[�g�V�F�[�_�[������������쐬����
	CreateComputeShaderFromMemory((LPBYTE)pBlob->GetBufferPointer(), pBlob->GetBufferSize());
}

// ���X�^���C�U�X�e�[�g���쐬
void GraphicsPipeline::CreateRasterizerState(D3D11_CULL_MODE CullMode, D3D11_FILL_MODE FillMode,BOOL fcc)
{
	HRESULT hr;

	D3D11_RASTERIZER_DESC RasterizerDesc;

	::ZeroMemory(&RasterizerDesc, sizeof(RasterizerDesc));
	RasterizerDesc.FillMode = FillMode;            // �|���S���ʕ`��
	RasterizerDesc.CullMode = CullMode;            // �w��̕����������Ă���O�p�`���J�����O����
	RasterizerDesc.FrontCounterClockwise = fcc;   // �����v����\��
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0;
	RasterizerDesc.SlopeScaledDepthBias = 0;
	RasterizerDesc.DepthClipEnable = TRUE;
	RasterizerDesc.ScissorEnable = FALSE;          // �V�U�[��`����
	RasterizerDesc.MultisampleEnable = FALSE;
	RasterizerDesc.AntialiasedLineEnable = FALSE;


	if (FAILED(hr = d3d11.pD3DDevice->CreateRasterizerState(&RasterizerDesc, &m_pRasterizerState)))
		throw(UException(hr, _T("GraphicsPipeline::CreateRasterizerState()�ŃG���[���������܂����BID3D11RasterizerState�C���^�[�t�F�[�X�̍쐬�G���[�ł��B")));
}

// �[�x�X�e���V���X�e�[�g���쐬
void GraphicsPipeline::CreateDepthStencilState(BOOL DepthEnable, D3D11_DEPTH_WRITE_MASK DepthWriteEnabled, D3D11_COMPARISON_FUNC DepthFunc)
{
	HRESULT hr;

	D3D11_DEPTH_STENCIL_DESC DSDesc;

	DSDesc.DepthEnable = DepthEnable;           // �[�x�e�X�g���s����
	DSDesc.DepthWriteMask = DepthWriteEnabled;  // �[�x�o�b�t�@�ւ̏������݂��s����
	DSDesc.DepthFunc = DepthFunc;
	DSDesc.StencilEnable = FALSE;


	if (FAILED(hr = d3d11.pD3DDevice->CreateDepthStencilState(&DSDesc, &m_pDepthStencilState)))
		throw(UException(hr, _T("GraphicsPipeline::CreateDepthStencilState()�ŃG���[���������܂����BID3D11DepthStencilState�C���^�[�t�F�[�X�̍쐬�G���[�ł��B")));
}

// �u�����h�X�e�[�g���쐬
void GraphicsPipeline::CreateBlendState(UEBLEND_STATE* BlendStateType, UINT BlendStateTypeLength, BOOL AlphaToCoverageEnable)
{
	HRESULT hr;
	D3D11_BLEND_DESC BlendDesc;

	if (BlendStateTypeLength == 0 || BlendStateTypeLength >= 8)
		throw(UException(-1, _T("GraphicsPipeline::CreateBlendState()�ŃG���[���������܂����B���������s���ł��B")));

	::ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = AlphaToCoverageEnable;

	if (BlendStateTypeLength == 1)
		BlendDesc.IndependentBlendEnable = FALSE;
	else
		BlendDesc.IndependentBlendEnable = TRUE;

	for (UINT i = 0; i<BlendStateTypeLength; i++)
	{
		switch (BlendStateType[i])
		{
		case UEBLEND_STATE::NONE:
			BlendDesc.RenderTarget[i].BlendEnable = FALSE;
			BlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
			BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;

			// ���Z����
		case UEBLEND_STATE::ADD:
			BlendDesc.RenderTarget[i].BlendEnable = TRUE;
			BlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
			BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;

			// ���`����
		case UEBLEND_STATE::ALIGNMENT:
			BlendDesc.RenderTarget[i].BlendEnable = TRUE;
			BlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			BlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			BlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
			BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;

			// �[�x�e�X�g
		case UEBLEND_STATE::DEPTH_TEST:
			BlendDesc.RenderTarget[i].BlendEnable = TRUE;
			BlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_MIN;      // �őO�ʂ̐[�x�l�ŏ㏑�����邽�߂ɍŏ��l��I��
			BlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_MIN;
			BlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;

		case UEBLEND_STATE::TEST:
			BlendDesc.RenderTarget[i].BlendEnable = TRUE;
			BlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			BlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ZERO;
			BlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
			BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[i].RenderTargetWriteMask = 0x0F;
			break;

		case UEBLEND_STATE::DEFAULT:
			BlendDesc.RenderTarget[i].BlendEnable = FALSE;
			BlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
			BlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
			BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
	}


	if (FAILED(hr = d3d11.pD3DDevice->CreateBlendState(&BlendDesc, &m_pBlendState)))
		throw(UException(hr, _T("GraphicsPipeline::CreateBlendState()�ŃG���[���������܂����BID3D11BlendState�C���^�[�t�F�[�X�̍쐬�G���[�ł��B")));
}

// �萔�o�b�t�@���쐬����
ComPtr<ID3D11Buffer> GraphicsPipeline::CreateConstantBuffer(void* pData, size_t size, UINT CPUAccessFlag)
{
	HRESULT hr = E_FAIL;

	ComPtr<ID3D11Buffer> pBuffer = nullptr;

	// �o�b�t�@�[ ���\�[�X�B
	D3D11_BUFFER_DESC BufferDesc = { 0 };

	// �T�u���\�[�X
	D3D11_SUBRESOURCE_DATA* resource = nullptr;

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
		resource = NEW D3D11_SUBRESOURCE_DATA();
		resource->pSysMem = pData;
		resource->SysMemPitch = 0;
		resource->SysMemSlicePitch = 0;
	}

	// �o�b�t�@�̐ݒ�
	BufferDesc.ByteWidth = size;                       // �o�b�t�@�T�C�Y
	BufferDesc.Usage = Usage;                      // ���\�[�X�g�p�@����肷��
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // �o�b�t�@�̎��
	BufferDesc.CPUAccessFlags = CPUAccessFlags;             // CPU �A�N�Z�X
	BufferDesc.MiscFlags = 0;                          // ���̑��̃t���O���ݒ肵�Ȃ�

	// �o�b�t�@���쐬����
	hr = d3d11.pD3DDevice->CreateBuffer(&BufferDesc, resource, &pBuffer);
	if (FAILED(hr))
		throw(UException(hr, _T("GraphicsPipeline::CreateConstantBuffer()�ŃG���[���������܂����B�o�b�t�@�쐬�G���[�ł��B")));

	return pBuffer;
}
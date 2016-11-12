#pragma once
#include "Direct3D11.h"

class GraphicsPipeline
{
private:
	Direct3D11 &d3d11 = Direct3D11::Instance();

	// 入力レイアウト
	ComPtr<ID3D11InputLayout> m_pLayout;

	// 頂点シェーダー
	// とりあえずストリームアウトプットは考慮しない
	ComPtr<ID3D11VertexShader> m_pVertexShader;

	// ハルシェーダー
	ComPtr<ID3D11HullShader> m_pHullShader;

	// ドメインシェーダー
	ComPtr<ID3D11DomainShader> m_pDomainShader;

	// ジオメトリシェーダー
	ComPtr<ID3D11GeometryShader> m_pGeometryShader;

	// ピクセルシェーダー
	ComPtr<ID3D11PixelShader> m_pPixelShader;

	// コンピュートシェーダー
	ComPtr<ID3D11ComputeShader> m_pComputeShader;

	// ラスタライザステート
	ComPtr<ID3D11RasterizerState> m_pRasterizerState;

	// 深度ステンシルステート
	ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;

	// ブレンドステート
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
		CPU_NONE_GPU_READWRITE = 0,   // CPUによるアクセスなし、GPUによる読込みと書込み.
		UPDATE_SUBRESOURCE = 1,       // ID3D11DeviceContext::UpdateSubresource()を使用してコピーを行う場合
		CPU_WRITE_GPU_READ = 2,       // CPUによる書込み、GPUによる読込み
	};

	GraphicsPipeline();
	virtual ~GraphicsPipeline();

	// 頂点シェーダーをファイルから作成する
	void CreateVertexShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[],
		D3D11_INPUT_ELEMENT_DESC pInputElementDescs[], UINT NumElements);

	// 頂点シェーダーをメモリから作成する
	void CreateVertexShaderFromMemory(BYTE* pShader, size_t size,
		D3D11_INPUT_ELEMENT_DESC pInputElementDescs[], UINT NumElements);

	// ハルシェーダーをファイルから作成する
	void CreateHullShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[]);

	// ハルシェーダーをメモリから作成する
	void CreateHullShaderFromMemory(BYTE* pShader, size_t size);

	// ドメインシェーダーをファイルから作成する
	void CreateDomainShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[]);

	// ドメインシェーダーをメモリから作成する
	void CreateDomainShaderFromMemory(BYTE* pShader, size_t size);

	// ジオメトリシェーダーをファイルから作成する
	void CreateGeometryShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[]);

	// ジオメトリシェーダーをメモリから作成する
	void CreateGeometryShaderFromMemory(BYTE* pShader, size_t size);

	// ピクセルシェーダーをファイルから作成する
	void CreatePixelShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[]);

	// ピクセルシェーダーをメモリから作成する
	void CreatePixelShaderFromMemory(BYTE* pShader, size_t size);

	// コンピュートシェーダーをファイルから作成する
	void CreateComputeShaderFromFile(TCHAR pSrcFile[], CHAR pFunctionName[]);

	// コンピュートシェーダーをメモリから作成する
	void CreateComputeShaderFromMemory(BYTE* pShader, size_t size);

	// ラスタライザステートを作成
	void CreateRasterizerState(D3D11_CULL_MODE CullMode, D3D11_FILL_MODE FillMode = D3D11_FILL_SOLID, BOOL fcc = TRUE);

	// 深度ステンシルステートを作成
	void CreateDepthStencilState(BOOL DepthEnable, D3D11_DEPTH_WRITE_MASK DepthWriteEnabled, D3D11_COMPARISON_FUNC DepthFunc = D3D11_COMPARISON_LESS);

	// ブレンドステートを作成
	void CreateBlendState(UEBLEND_STATE* BlendStateType, UINT BlendStateTypeLength, BOOL AlphaToCoverageEnable = FALSE);

	// 定数バッファを作成する
	ComPtr<ID3D11Buffer> CreateConstantBuffer(void* pData, size_t size, UINT CPUAccessFlag);

	// 頂点シェーダーをデバイスに設定する
	inline void SetVertexShader()
	{
		// 入力アセンブラー ステージを設定
		d3d11.pD3DDeviceContext->IASetInputLayout(m_pLayout.Get());
		// 頂点シェーダーをデバイスに設定する。
		d3d11.pD3DDeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	}

	// ハルシェーダーをデバイスに設定する
	inline void SetHullShader()
	{
		d3d11.pD3DDeviceContext->HSSetShader(m_pHullShader.Get(), nullptr, 0);
	}

	// ドメインシェーダーをデバイスに設定する
	inline void SetDomainShader()
	{
		d3d11.pD3DDeviceContext->DSSetShader(m_pDomainShader.Get(), nullptr, 0);
	}

	// ジオメトリシェーダーをデバイスに設定する
	inline void SetGeometryShader()
	{
		d3d11.pD3DDeviceContext->GSSetShader(m_pGeometryShader.Get(), nullptr, 0);
	}

	// ピクセルシェーダーをデバイスに設定する
	inline void SetPixelShader()
	{
		d3d11.pD3DDeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	}

	// コンピュートシェーダーをデバイスに設定する
	inline void SetComputeShader()
	{
		d3d11.pD3DDeviceContext->CSSetShader(m_pComputeShader.Get(), nullptr, 0);
	}

	// ラスタライザステートをデバイスに設定する
	inline void SetRasterizerState()
	{
		d3d11.pD3DDeviceContext->RSSetState(m_pRasterizerState.Get());
	}

	// 深度ステンシルステートをデバイスに設定する
	inline void SetDepthStencilState()
	{
		d3d11.pD3DDeviceContext->OMSetDepthStencilState(m_pDepthStencilState.Get(), 0);
	}

	// ブレンドステートをデバイスに設定する
	inline void SetBlendState()
	{
		d3d11.pD3DDeviceContext->OMSetBlendState(m_pBlendState.Get(), nullptr, 0xffffffff);
	}
};
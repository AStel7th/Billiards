#include "../Header/DebugFont.h"

DebugFont::DebugFont()
{
	m_pVertexBuffer = nullptr;
	m_pSRView = nullptr;
	m_pSamplerState = nullptr;
	m_pConstantBuffers = nullptr;
	m_pGraphicsPipeline = nullptr;
	m_pStr[0] = '\0';
}

DebugFont::~DebugFont()
{
	Invalidate();
}

void DebugFont::Invalidate()
{
	m_pStr[0] = '\0';
}

void DebugFont::CreateMesh(float FontWidth, float FontHeight)
{
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_pGraphicsPipeline = make_unique<GraphicsPipeline>();

	// シェーダーを作成する
	m_pGraphicsPipeline->CreateVertexShaderFromMemory((LPBYTE)g_DebugFont_VS_Main, sizeof(g_DebugFont_VS_Main), layout, _countof(layout));
	m_pGraphicsPipeline->CreatePixelShaderFromMemory((LPBYTE)g_DebugFont_PS_Main, sizeof(g_DebugFont_PS_Main));

	// ラスタライザーステートを作成する
	m_pGraphicsPipeline->CreateRasterizerState(D3D11_CULL_MODE::D3D11_CULL_BACK);

	// 深度ステンシルステートを作成する
	m_pGraphicsPipeline->CreateDepthStencilState(FALSE, D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL);

	// ブレンドステートを線形合成で作成する
	GraphicsPipeline::UEBLEND_STATE BlendStateType[1] = { GraphicsPipeline::UEBLEND_STATE::ALIGNMENT };
	m_pGraphicsPipeline->CreateBlendState(BlendStateType, 1);

	m_FontWidth = FontWidth;             // ポリゴンの横幅
	m_FontHeight = FontHeight;            // ポリゴンの縦幅
	float TU = 1.0f / (float)m_FontCnt;   // 1フォント当たりのテクセルの横幅

	// 頂点のデータ
	DebugFont::VERTEX v[] = {
		XMFLOAT3(m_FontWidth * 0.5f, m_FontHeight * 0.5f, 0), XMFLOAT2(TU, 0),
		XMFLOAT3(-m_FontWidth * 0.5f, m_FontHeight * 0.5f, 0), XMFLOAT2(0, 0),
		XMFLOAT3(m_FontWidth * 0.5f, -m_FontHeight * 0.5f, 0), XMFLOAT2(TU, 1),
		XMFLOAT3(-m_FontWidth * 0.5f, -m_FontHeight * 0.5f, 0), XMFLOAT2(0, 1)
	};
	// 頂点バッファを作成する
	m_pVertexBuffer = Buffers::CreateVertexBuffer(d3d11.pD3DDevice.Get(), v, sizeof(v), 0);

	// デカールマップをロード
	Maps::CreateSRViewFromDDSFile(d3d11.pD3DDevice.Get(), (WCHAR*)(_T("Resource/font.dds")), &m_pSRView);

	// サンプラーステートを作成する
	m_pSamplerState = Samplers::CreateSamplerState(d3d11.pD3DDevice.Get(), D3D11_TEXTURE_ADDRESS_CLAMP);

	// 定数バッファを作成する
	m_pConstantBuffers = m_pGraphicsPipeline->CreateConstantBuffer(nullptr, sizeof(DebugFont::CONSTANT_BUFFER), D3D11_CPU_ACCESS_WRITE);
}

void DebugFont::Update(TCHAR* pStr, XMFLOAT2* pPosition, XMFLOAT4* pColor)
{
	_tcscpy_s(m_pStr, pStr);
	::CopyMemory(&m_Position, pPosition, sizeof(XMFLOAT2));
	::CopyMemory(&m_Color, pColor, sizeof(XMFLOAT4));
}

void DebugFont::Draw()
{
	HRESULT hr = E_FAIL;

	if (_tcsclen(m_pStr) == 0)
		return;

	XMFLOAT2 pos = m_Position;

	// 各種グラフィックパイプラインを設定
	m_pGraphicsPipeline->SetVertexShader();
	m_pGraphicsPipeline->SetHullShader();
	m_pGraphicsPipeline->SetDomainShader();
	m_pGraphicsPipeline->SetGeometryShader();
	m_pGraphicsPipeline->SetPixelShader();
	m_pGraphicsPipeline->SetRasterizerState();
	m_pGraphicsPipeline->SetDepthStencilState();
	m_pGraphicsPipeline->SetBlendState();

	// 頂点バッファ設定
	UINT stride = sizeof(DebugFont::VERTEX);
	UINT offset = 0;
	d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);

	// プリミティブ タイプおよびデータの順序に関する情報を設定
	d3d11.pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ピクセルシェーダーのサンプラーステートを設定する
	d3d11.pD3DDeviceContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());

	// デカールマップを設定する
	d3d11.pD3DDeviceContext->PSSetShaderResources(0, 1, m_pSRView.GetAddressOf());

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DebugFont::CONSTANT_BUFFER* cBuffer;

	// 文字数分ループ
	for (int i = 0; i<(int)_tcsclen(m_pStr); i++)
	{
		if (m_pStr[i] == '\n')
		{
			pos.x = m_Position.x;
			pos.y -= m_FontHeight;
			continue;
		}

		if (FAILED(hr = d3d11.pD3DDeviceContext->Map(m_pConstantBuffers.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
			throw(UException(hr, _T("DebugFont::RenderDebugText()でエラーが発生しました。ロックできません")));

		cBuffer = (DebugFont::CONSTANT_BUFFER*)(mappedResource.pData);

		// 表示位置
		::CopyMemory(&cBuffer->Position, &pos, sizeof(XMFLOAT2));

		//// テクセル
		cBuffer->TexelOffset.x = (float)(m_pStr[i] - 32) / m_FontCnt; // TU
		cBuffer->TexelOffset.y = 0.0f;                                  // TV

		// 頂点カラー
		::CopyMemory(&cBuffer->Color, &m_Color, sizeof(XMFLOAT4));

		d3d11.pD3DDeviceContext->Unmap(m_pConstantBuffers.Get(), 0);

		// 頂点シェーダーに定数バッファを設定する。
		d3d11.pD3DDeviceContext->VSSetConstantBuffers(0, 1, m_pConstantBuffers.GetAddressOf());

		// ピクセルシェーダーに定数バッファを設定する。
		d3d11.pD3DDeviceContext->PSSetConstantBuffers(0, 1, m_pConstantBuffers.GetAddressOf());

		// 描画
		d3d11.pD3DDeviceContext->Draw(4, 0);

		pos.x += m_FontWidth;
	}
}


// **********************************************************************************
// FPS描画クラス
// **********************************************************************************

UFPS::UFPS()
{
	m_PrevTime = ::timeGetTime();
	m_FrameCounter = 0;
	m_Frame = 0;
}

UFPS::~UFPS()
{
	Invalidate();
}

void UFPS::Invalidate()
{
	DebugFont::Invalidate();
}

void UFPS::CreateMesh()
{
	DebugFont::CreateMesh(0.03f, 0.08f);
}

void UFPS::Update()
{
	DWORD now = ::timeGetTime();
	TCHAR fps[20];

	if (now - m_PrevTime > 1000)
	{
		m_PrevTime = now;
		m_Frame = m_FrameCounter;
		m_FrameCounter = 0;
	}
	else
		m_FrameCounter++;

	_stprintf_s(fps, _T("FPS : %d"), m_Frame);

	DebugFont::Update(fps, &XMFLOAT2(-0.97f, 0.95f), &XMFLOAT4(1, 1, 1, 1));
}

void UFPS::Draw()
{
	DebugFont::Draw();
}


// **********************************************************************************
// デバッグテキスト描画クラス
// **********************************************************************************

DebugText::DebugText()
{
	
}

DebugText::~DebugText()
{
	Invalidate();
}

void DebugText::Invalidate()
{
	DebugFont::Invalidate();
}

void DebugText::CreateMesh()
{
	DebugFont::CreateMesh(0.03f, 0.08f);
}

void DebugText::SetText(TCHAR* tchar,XMFLOAT2 *pos)
{
	DebugFont::Update(tchar, pos, &XMFLOAT4(1, 1, 1, 1));
}

void DebugText::Draw()
{
	DebugFont::Draw();
}
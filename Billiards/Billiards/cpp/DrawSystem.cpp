#include "../Header/DrawSystem.h"
#include "../Header/Camera.h"
#include "../Header/Effect.h"


DrawSystem::DrawSystem()
{

}

DrawSystem::~DrawSystem()
{

}

// システムを初期化する
HRESULT DrawSystem::Init(unsigned int msaaSamples)
{
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = FALSE;

	if (FAILED(d3d11.pD3DDevice->CreateRasterizerState(&rsDesc, &pRS)))
	{
		return E_FAIL;
	}

	rsDesc.CullMode = D3D11_CULL_NONE;
	if (FAILED(d3d11.pD3DDevice->CreateRasterizerState(&rsDesc, &pRSnoCull)))
	{
		return E_FAIL;
	}

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;      ///tryed D3D11_BLEND_ONE ... (and others desperate combinations ... )
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;     ///tryed D3D11_BLEND_ONE ... (and others desperate combinations ... )
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	if (FAILED(d3d11.pD3DDevice->CreateBlendState(&blendDesc, &pBlendState)))
	{
		return E_FAIL;
	}

	D3D11_DEPTH_STENCIL_DESC descDSS;
	ZeroMemory(&descDSS, sizeof(descDSS));
	descDSS.DepthEnable = TRUE;
	descDSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDSS.DepthFunc = D3D11_COMPARISON_LESS;
	descDSS.StencilEnable = FALSE;
	if (FAILED(d3d11.pD3DDevice->CreateDepthStencilState(&descDSS, &pDepthStencilState)))
	{
		return E_FAIL;
	}

	pvsFBX = make_unique<VertexShader>(d3d11.pD3DDevice.Get(), L"HLSL/simpleRenderVS.hlsl", "vs_main");
	pvsFBXInstancing = make_unique<VertexShader>(d3d11.pD3DDevice.Get(), L"HLSL/simpleRenderInstancingVS.hlsl", "vs_main");
	pvsVertexColor = make_unique<VertexShader>(d3d11.pD3DDevice.Get(), L"HLSL/VertexColor.hlsl", "vs_main");
	pvsFBXAnimation = make_unique<VertexShader>(d3d11.pD3DDevice.Get(), L"HLSL/FbxAnimation.hlsl", "VSSkin");
	/*pPlaneVS_UV = make_unique<VertexShader>(d3d11.pD3DDevice.Get(), L"HLSL/Plane_UV.hlsl", "vs_main");
	pPlanePS_UV = make_unique<PixelShader>(d3d11.pD3DDevice.Get(), L"HLSL/Plane_UV.hlsl", "ps_main");*/
	ppsFBX = make_unique<PixelShader>(d3d11.pD3DDevice.Get(), L"HLSL/simpleRenderPS.hlsl", "PS");
	ppsVertexColor = make_unique<PixelShader>(d3d11.pD3DDevice.Get(), L"HLSL/VertexColor.hlsl", "ps_main");
	ppsFBXAnimation = make_unique<PixelShader>(d3d11.pD3DDevice.Get(), L"HLSL/FbxAnimation.hlsl", "PSSkin");

	D3D11_INPUT_ELEMENT_DESC layout_uv[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11_INPUT_ELEMENT_DESC layout_uv2[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC layout_color[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// InputLayoutの作成
	if (FAILED(d3d11.pD3DDevice->CreateInputLayout(layout_uv, ARRAYSIZE(layout_uv),
		pvsFBXAnimation->GetByteCode()->GetBufferPointer(), pvsFBXAnimation->GetByteCode()->GetBufferSize(), &pInputLayoutUV)))
	{
		return E_FAIL;
	}

	if (FAILED(d3d11.pD3DDevice->CreateInputLayout(layout_uv, ARRAYSIZE(layout_uv),
		pvsFBX->GetByteCode()->GetBufferPointer(), pvsFBX->GetByteCode()->GetBufferSize(), &pInputLayoutUV1)))
	{
		return E_FAIL;
	}

	if (FAILED(d3d11.pD3DDevice->CreateInputLayout(layout_color, ARRAYSIZE(layout_color),
		pvsVertexColor->GetByteCode()->GetBufferPointer(), pvsVertexColor->GetByteCode()->GetBufferSize(), &pInputLayoutColor)))
	{
		return E_FAIL;
	}

	/*if (FAILED(d3d11.pD3DDevice->CreateInputLayout(layout_uv2, ARRAYSIZE(layout_uv2),
		pPlaneVS_UV->GetByteCode()->GetBufferPointer(), pPlaneVS_UV->GetByteCode()->GetBufferSize(), &pInputLayoutUV2)))
	{
		return E_FAIL;
	}*/

	//コンスタントバッファー0作成  
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(d3d11.pD3DDevice->CreateBuffer(&bd, NULL, &pcBuffer0)))
	{
		return E_FAIL;
	}

	//コンスタントバッファー1作成  
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(CBMATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(d3d11.pD3DDevice->CreateBuffer(&bd, NULL, &pcBuffer1)))
	{
		return E_FAIL;
	}

	//コンスタントバッファーボーン用　作成  
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(SHADER_GLOBAL_BONES);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(d3d11.pD3DDevice->CreateBuffer(&bd, NULL, &pcBoneBuffer)))
	{
		return E_FAIL;
	}

	return true;
}

// リストに登録されたタスクを描画する
bool DrawSystem::Draw()
{
	XMMATRIX view = XMLoadFloat4x4(&Camera::Instance().view);

	float ClearColor[4] = { 0.0f, 0.0f, 0.7f, 1.0f };
	//float ClearColor[4] = { 0,0,1,1 };

	// バックバッファをクリアする。
	d3d11.ClearBackBuffer(ClearColor);

	// 深度バッファをクリアする。
	d3d11.ClearDepthStencilView(D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	float blendFactors[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	d3d11.pD3DDeviceContext->RSSetState(pRS.Get());
	d3d11.pD3DDeviceContext->OMSetBlendState(pBlendState.Get(), blendFactors, 0xffffffff);
	d3d11.pD3DDeviceContext->OMSetDepthStencilState(pDepthStencilState.Get(), 0);

	//{
	//	d3d11.pD3DDeviceContext->RSSetState(pRSnoCull.Get());			//頂点カラーは表裏なしのため両面描画に変更
	//	d3d11.pD3DDeviceContext->VSSetShader(pPlaneVS_UV->GetShader(), NULL, 0);
	//	d3d11.pD3DDeviceContext->PSSetShader(pPlanePS_UV->GetShader(), NULL, 0);
	//	d3d11.pD3DDeviceContext->IASetInputLayout(pInputLayoutUV2.Get());

	//	TaskDraw::All::Draw(GROUP_DRAW_PLANE_UV);
	//}

	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(d3d11.pD3DDeviceContext->Map(pcBuffer0.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		SHADER_GLOBAL sg;
		sg.lightDir = XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f);
		sg.eye = XMFLOAT4(view.r[3].m128_f32[0], view.r[3].m128_f32[1], view.r[3].m128_f32[2], 0);
		memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(SHADER_GLOBAL));
		d3d11.pD3DDeviceContext->Unmap(pcBuffer0.Get(), 0);
	}
	d3d11.pD3DDeviceContext->VSSetConstantBuffers(0, 1, pcBuffer0.GetAddressOf());
	d3d11.pD3DDeviceContext->PSSetConstantBuffers(0, 1, pcBuffer0.GetAddressOf());

	{
		d3d11.pD3DDeviceContext->VSSetShader(pvsFBX->GetShader(), NULL, 0);
		d3d11.pD3DDeviceContext->PSSetShader(ppsFBXAnimation->GetShader(), NULL, 0);
		d3d11.pD3DDeviceContext->IASetInputLayout(pInputLayoutUV1.Get());

		//TaskDraw::All::Draw(GROUP_DRAW_OPACITY);
		//GameObjectManager::Draw(CALL_TAG::TABLE);
	}

	{
		d3d11.pD3DDeviceContext->VSSetShader(pvsFBXAnimation->GetShader(), NULL, 0);
		d3d11.pD3DDeviceContext->PSSetShader(ppsFBXAnimation->GetShader(), NULL, 0);
		d3d11.pD3DDeviceContext->IASetInputLayout(pInputLayoutUV.Get());

		//TaskDraw::All::Draw(GROUP_DRAW_OPACITY_ANIMATION);
	}

	{
		d3d11.pD3DDeviceContext->RSSetState(pRSnoCull.Get());			//頂点カラーは表裏なしのため両面描画に変更
		d3d11.pD3DDeviceContext->VSSetShader(pvsVertexColor->GetShader(), NULL, 0);
		d3d11.pD3DDeviceContext->PSSetShader(ppsVertexColor->GetShader(), NULL, 0);
		d3d11.pD3DDeviceContext->IASetInputLayout(pInputLayoutColor.Get());

		//TaskDraw::All::Draw(GROUP_DRAW_VERTEXCOLOR);

		d3d11.pD3DDeviceContext->RSSetState(pRS.Get());
	}

	//TaskDraw::All::Draw(GROUP_DRAW_SPRITE);

	//EffectManager::Instance().Draw();

	Sprite::DrawAll();

	d3d11.pD3DDeviceContext->VSSetShader(NULL, NULL, 0);
	d3d11.pD3DDeviceContext->PSSetShader(NULL, NULL, 0);

	d3d11.Present(1, 0);

	return true;
}

ID3D11Buffer* DrawSystem::GetCBuffer(int i)
{
	switch (i)
	{
	case 0:
		return pcBuffer0.Get();
	case 1:
		return pcBuffer1.Get();
	case 2:
		return pcBoneBuffer.Get();
	}
}
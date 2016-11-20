#include "../Header/DrawSystem.h"
#include "../Header/Camera.h"
#include "../Header/Effect.h"
#include "../Header/Component.h"
#include "../Header/ResourceManager.h"
#include "../Header/MeshData.h"
#include "../Header/MaterialData.h"


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

	d3d11.pD3DDeviceContext->VSSetShader(pvsFBX->GetShader(), NULL, 0);
	d3d11.pD3DDeviceContext->PSSetShader(ppsFBXAnimation->GetShader(), NULL, 0);
	d3d11.pD3DDeviceContext->IASetInputLayout(pInputLayoutUV1.Get());

	for (auto it = opaqueDrawList[DRAW_PATTERN::STATIC_MESH].begin(); it != opaqueDrawList[DRAW_PATTERN::STATIC_MESH].end(); ++it)
	{
		if (ResourceManager::Instance().GetRefCount((*it)->pMeshData->GetName()) > 1)
			DrawStaticMeshInstance(*it);
		else
			DrawStaticMesh(*it);
	}

	d3d11.pD3DDeviceContext->VSSetShader(pvsFBXAnimation->GetShader(), NULL, 0);
	d3d11.pD3DDeviceContext->PSSetShader(ppsFBXAnimation->GetShader(), NULL, 0);
	d3d11.pD3DDeviceContext->IASetInputLayout(pInputLayoutUV.Get());

	for (auto it = opaqueDrawList[DRAW_PATTERN::ANIMATION].begin(); it != opaqueDrawList[DRAW_PATTERN::ANIMATION].end(); ++it)
	{
		if (ResourceManager::Instance().GetRefCount((*it)->pMeshData->GetName()) > 1)
			DrawAnimationInstance(*it);
		else
			DrawAnimation(*it);
	}

	//EffectManager::Instance().Draw();

	Sprite::DrawAll();

	d3d11.pD3DDeviceContext->VSSetShader(NULL, NULL, 0);
	d3d11.pD3DDeviceContext->PSSetShader(NULL, NULL, 0);

	d3d11.Present(1, 0);

	return true;
}

void DrawSystem::DrawAnimation(GraphicsComponent* pGC)
{
	// FBX Modelのnode数を取得
	size_t nodeCount = pGC->pMeshData->GetMeshCount();

	// 全ノードを描画
	for (int j = 0; j<nodeCount; j++)
	{
		MESH mesh = pGC->pMeshData->GetMeshList()[j];

		D3D11_MAPPED_SUBRESOURCE MappedResource;
		d3d11.pD3DDeviceContext->Map(pcBuffer1.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

		CBMATRIX*	cbFBX = (CBMATRIX*)MappedResource.pData;

		XMMATRIX _world = XMLoadFloat4x4(&pGC->world);
		XMMATRIX _view = XMLoadFloat4x4(&view);
		XMMATRIX _proj = XMLoadFloat4x4(&proj);
		XMMATRIX _local = XMLoadFloat4x4(&mesh.mLocal);

		// 左手系
		cbFBX->mWorld = pGC->world;
		cbFBX->mView = view;
		cbFBX->mProj = proj;

		XMStoreFloat4x4(&cbFBX->mWVP, XMMatrixTranspose(/*local**/_world*_view*_proj));

		d3d11.pD3DDeviceContext->Unmap(pcBuffer1.Get(), 0);

		d3d11.pD3DDeviceContext->VSSetConstantBuffers(1, 1, pcBuffer1.GetAddressOf());

		pGC->pMeshData->SetAnimationFrame(j, pGC->frame);

		//MATERIAL_DATA material = GetNodeMaterial(j);

		//if (material.pMaterialCb)
		//	d3d11.pD3DDeviceContext->UpdateSubresource(material.pMaterialCb.Get(), 0, NULL, &material.materialConstantData, 0, 0);

		////pDirect3D11->pD3DDeviceContext->VSSetShaderResources(0, 1, &g_pTransformSRV);
		//d3d11.pD3DDeviceContext->PSSetShaderResources(0, 1, material.pSRV.GetAddressOf());
		//d3d11.pD3DDeviceContext->PSSetConstantBuffers(0, 1, material.pMaterialCb.GetAddressOf());
		//d3d11.pD3DDeviceContext->PSSetSamplers(0, 1, material.pSampler.GetAddressOf());

		

		//フレームを進めたことにより変化したポーズ（ボーンの行列）をシェーダーに渡す
		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(d3d11.pD3DDeviceContext->Map(pcBoneBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			SHADER_GLOBAL_BONES sg;
			for (int i = 0; i < mesh.boneCount; i++)
			{
				XMMATRIX local = XMLoadFloat4x4(&mesh.mLocal);
				XMMATRIX mat = local * XMLoadFloat4x4(&/*node->mLocal**/pGC->pMeshData->GetCurrentPose(&mesh, i));
				mat = XMMatrixTranspose(mat);
				XMStoreFloat4x4(&sg.mBone[i], mat);
			}
			memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(SHADER_GLOBAL_BONES));
			d3d11.pD3DDeviceContext->Unmap(pcBoneBuffer.Get(), 0);
		}

		d3d11.pD3DDeviceContext->VSSetConstantBuffers(2, 1, pcBoneBuffer.GetAddressOf());
		d3d11.pD3DDeviceContext->PSSetConstantBuffers(2, 1, pcBoneBuffer.GetAddressOf());

		UINT stride = sizeof(VERTEX_DATA);
		UINT offset = 0;
		d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 1, mesh.pVB.GetAddressOf(), &stride, &offset);

		//マテリアルの数だけ、それぞれのマテリアルのインデックスバッファ－を描画
		for (int i = 0; i<mesh.materialCount; i++)
		{
			MaterialData material = *mesh.materialData[i];

			//使用されていないマテリアル対策
			if (material.faceCount == 0)
			{
				continue;
			}
			//インデックスバッファーをセット
			stride = sizeof(int);
			offset = 0;
			d3d11.pD3DDeviceContext->IASetIndexBuffer(mesh.pIB[i].Get(), DXGI_FORMAT_R32_UINT, 0);

			d3d11.pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			if (material.pMaterialCb)
				d3d11.pD3DDeviceContext->UpdateSubresource(material.pMaterialCb.Get(), 0, NULL, &material.materialConstantData, 0, 0);

			d3d11.pD3DDeviceContext->VSSetConstantBuffers(3, 1, material.pMaterialCb.GetAddressOf());
			d3d11.pD3DDeviceContext->PSSetConstantBuffers(3, 1, material.pMaterialCb.GetAddressOf());

			if (material.pSRV != nullptr)
			{
				d3d11.pD3DDeviceContext->PSSetShaderResources(0, 1, material.pSRV.GetAddressOf());
				d3d11.pD3DDeviceContext->PSSetSamplers(0, 1, material.pSampler.GetAddressOf());
			}

			//Draw
			d3d11.pD3DDeviceContext->DrawIndexed(material.faceCount * 3, 0, 0);
		}
		//}
	}

}

void DrawSystem::DrawStaticMesh(GraphicsComponent * pGC)
{
	// FBX Modelのnode数を取得
	size_t nodeCount = pGC->pMeshData->GetMeshCount();

	// 全ノードを描画
	for (int j = 0; j<nodeCount; j++)
	{
		MESH mesh = pGC->pMeshData->GetMeshList()[j];

		if (mesh.pVB == nullptr)
			continue;

		D3D11_MAPPED_SUBRESOURCE MappedResource;
		d3d11.pD3DDeviceContext->Map(pcBuffer1.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

		CBMATRIX*	cbFBX = (CBMATRIX*)MappedResource.pData;

		XMMATRIX _world = XMLoadFloat4x4(&pGC->world);
		XMMATRIX _view = XMLoadFloat4x4(&view);
		XMMATRIX _proj = XMLoadFloat4x4(&proj);
		XMMATRIX _local = XMLoadFloat4x4(&mesh.mLocal);

		// 左手系
		cbFBX->mWorld = pGC->world;
		cbFBX->mView = view;
		cbFBX->mProj = proj;

		XMStoreFloat4x4(&cbFBX->mWVP, XMMatrixTranspose(/*local**/_world*_view*_proj));

		d3d11.pD3DDeviceContext->Unmap(pcBuffer1.Get(), 0);

		d3d11.pD3DDeviceContext->VSSetConstantBuffers(1, 1, pcBuffer1.GetAddressOf());

		UINT stride = sizeof(VERTEX_DATA);
		UINT offset = 0;
		d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 1, mesh.pVB.GetAddressOf(), &stride, &offset);

		//マテリアルの数だけ、それぞれのマテリアルのインデックスバッファ－を描画
		for (int i = 0; i<mesh.materialCount; i++)
		{
			MaterialData material = *mesh.materialData[i];

			//使用されていないマテリアル対策
			if (material.faceCount == 0)
			{
				continue;
			}
			//インデックスバッファーをセット
			stride = sizeof(int);
			offset = 0;
			d3d11.pD3DDeviceContext->IASetIndexBuffer(mesh.pIB[i].Get(), DXGI_FORMAT_R32_UINT, 0);

			d3d11.pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			if (material.pMaterialCb)
				d3d11.pD3DDeviceContext->UpdateSubresource(material.pMaterialCb.Get(), 0, NULL, &material.materialConstantData, 0, 0);

			d3d11.pD3DDeviceContext->VSSetConstantBuffers(3, 1, material.pMaterialCb.GetAddressOf());
			d3d11.pD3DDeviceContext->PSSetConstantBuffers(3, 1, material.pMaterialCb.GetAddressOf());

			if (material.pSRV != nullptr)
			{
				d3d11.pD3DDeviceContext->PSSetShaderResources(0, 1, material.pSRV.GetAddressOf());
				d3d11.pD3DDeviceContext->PSSetSamplers(0, 1, material.pSampler.GetAddressOf());
			}

			//Draw
			d3d11.pD3DDeviceContext->DrawIndexed(material.faceCount * 3, 0, 0);
		}
		//}
	}
}

void DrawSystem::DrawAnimationInstance(GraphicsComponent * pGC)
{
}

void DrawSystem::DrawStaticMeshInstance(GraphicsComponent * pGC)
{
}

void DrawSystem::AddDrawList(DRAW_PRIOLITY priolity, DRAW_PATTERN pattern, GraphicsComponent* pGC)
{
	if(priolity == DRAW_PRIOLITY::Opaque)
		opaqueDrawList[pattern].push_back(pGC);
	else if(priolity == DRAW_PRIOLITY::Alpha)
		alphaDrawList[pattern].push_back(pGC);
}

void DrawSystem::SetView(XMFLOAT4X4 * v)
{
	view = *v;
}

void DrawSystem::SetProjection(XMFLOAT4X4 * p)
{
	proj = *p;
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
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
	SAFE_DELETE(pvsFBX);
	SAFE_DELETE(pvsFBXInstancing);
	SAFE_DELETE(pvsFBXAnimInstancing);
	SAFE_DELETE(pvsFBXAnimation);
	SAFE_DELETE(ppsVertexColor);
	SAFE_DELETE(ppsTex);
}

// �V�X�e��������������
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
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;      ///tryed D3D11_BLEND_ONE ... (and others desperate combinations ... )
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

	pvsFBX = NEW VertexShader(d3d11.pD3DDevice.Get(), L"HLSL/simpleRenderVS.hlsl", "vs_main");
	pvsFBXInstancing = NEW VertexShader(d3d11.pD3DDevice.Get(), L"HLSL/simpleRenderInstancingVS.hlsl", "vs_main");
	pvsFBXAnimInstancing = NEW VertexShader(d3d11.pD3DDevice.Get(), L"HLSL/AnimationInstance.hlsl", "VSSkin");
	pvsFBXAnimation = NEW VertexShader(d3d11.pD3DDevice.Get(), L"HLSL/FbxAnimation.hlsl", "VSSkin");
	ppsVertexColor = NEW PixelShader(d3d11.pD3DDevice.Get(), L"HLSL/simpleRenderPS.hlsl", "PS");
	ppsTex = NEW PixelShader(d3d11.pD3DDevice.Get(), L"HLSL/SimpleRenderTexPS.hlsl", "PS");

	D3D11_INPUT_ELEMENT_DESC layout_staticMesh[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	D3D11_INPUT_ELEMENT_DESC layout_Animation[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// InputLayout�̍쐬
	if (FAILED(d3d11.pD3DDevice->CreateInputLayout(layout_Animation, ARRAYSIZE(layout_Animation),
		pvsFBXAnimation->GetByteCode()->GetBufferPointer(), pvsFBXAnimation->GetByteCode()->GetBufferSize(), &pInputLayoutAnimation)))
	{
		return E_FAIL;
	}

	if (FAILED(d3d11.pD3DDevice->CreateInputLayout(layout_staticMesh, ARRAYSIZE(layout_staticMesh),
		pvsFBX->GetByteCode()->GetBufferPointer(), pvsFBX->GetByteCode()->GetBufferSize(), &pInputLayoutStaticMesh)))
	{
		return E_FAIL;
	}

	if (FAILED(d3d11.pD3DDevice->CreateInputLayout(layout_Animation, ARRAYSIZE(layout_Animation),
		pvsFBXAnimInstancing->GetByteCode()->GetBufferPointer(), pvsFBXAnimInstancing->GetByteCode()->GetBufferSize(), &pInputLayoutAnimationInstance)))
	{
		return E_FAIL;
	}

	if (FAILED(d3d11.pD3DDevice->CreateInputLayout(layout_staticMesh, ARRAYSIZE(layout_staticMesh),
		pvsFBXInstancing->GetByteCode()->GetBufferPointer(), pvsFBXInstancing->GetByteCode()->GetBufferSize(), &pInputLayoutStaticMeshInstance)))
	{
		return E_FAIL;
	}

	//�R���X�^���g�o�b�t�@�[0�쐬  
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(d3d11.pD3DDevice->CreateBuffer(&bd, nullptr, &pcBuffer0)))
	{
		return E_FAIL;
	}

	//�R���X�^���g�o�b�t�@�[1�쐬  
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(CBMATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(d3d11.pD3DDevice->CreateBuffer(&bd, nullptr, &pcBuffer1)))
	{
		return E_FAIL;
	}

	//�R���X�^���g�o�b�t�@�[�C���X�^���X�쐬  
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(CBMATRIX_INSTANCING);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(d3d11.pD3DDevice->CreateBuffer(&bd, nullptr, &pcBufferInstance)))
	{
		return E_FAIL;
	}

	//�R���X�^���g�o�b�t�@�[�{�[���p�@�쐬  
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(SHADER_GLOBAL_BONES);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(d3d11.pD3DDevice->CreateBuffer(&bd, nullptr, &pcBoneBuffer)))
	{
		return E_FAIL;
	}

	const uint32_t count = MAX_INSTANCE;
	const uint32_t stride = static_cast<uint32_t>(sizeof(SRVPerInstanceData));

	// Create StructuredBuffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = stride * count;
	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd.StructureByteStride = stride;
	if (FAILED(d3d11.pD3DDevice->CreateBuffer(&bd, nullptr, &pTransformStructuredBuffer)))
		return E_FAIL;

	// Create ShaderResourceView
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // �g�����ꂽ�o�b�t�@�[�ł��邱�Ƃ��w�肷��
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.BufferEx.NumElements = count;                  // ���\�[�X���̗v�f�̐�

														   // �\�����o�b�t�@�[�����ƂɃV�F�[�_�[���\�[�X�r���[���쐬����
	if (FAILED(d3d11.pD3DDevice->CreateShaderResourceView(pTransformStructuredBuffer.Get(), &srvDesc, &pTransformSRV)))
		return E_FAIL;

	lightDir = XMFLOAT4(-100.0f, -200.0f, 100.0f, 1.0f);

	return true;
}

void DrawSystem::SetMatrix(vector<GraphicsComponent*>& pGClist)
{
	HRESULT hr = S_OK;
	const uint32_t count = MAX_INSTANCE;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	d3d11.pD3DDeviceContext->Map(pTransformStructuredBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	SRVPerInstanceData*	pSrvInstanceData = (SRVPerInstanceData*)MappedResource.pData;

	for (uint32_t i = 0; i<pGClist.size(); i++)
	{
		XMMATRIX _world = XMLoadFloat4x4(&pGClist[i]->pGameObject->worldMat);
		XMStoreFloat4x4(&pSrvInstanceData[i].mWorld, _world);
	}

	d3d11.pD3DDeviceContext->Unmap(pTransformStructuredBuffer.Get(), 0);
}

// ���X�g�ɓo�^���ꂽ�^�X�N��`�悷��
bool DrawSystem::Draw()
{
	float ClearColor[4] = { 0.08f, 0.05f, 0.05f, 1.0f };
	//float ClearColor[4] = { 0,0,1,1 };

	// �o�b�N�o�b�t�@���N���A����B
	d3d11.ClearBackBuffer(ClearColor);

	// �[�x�o�b�t�@���N���A����B
	d3d11.ClearDepthStencilView(D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	float blendFactors[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	d3d11.pD3DDeviceContext->RSSetState(pRS.Get());
	d3d11.pD3DDeviceContext->OMSetBlendState(pBlendState.Get(), blendFactors, 0xffffffff);
	d3d11.pD3DDeviceContext->OMSetDepthStencilState(pDepthStencilState.Get(), 0);

	for (auto it = opaqueDrawList.begin(); it != opaqueDrawList.end(); ++it)
	{
		bool isAnim;
		int refCnt = (*it).second.size();

		if (refCnt == 0)
			continue;

		if (refCnt > 1)
		{
			MeshData* pMesh = (*it).second[0]->pMeshData;
			isAnim = pMesh->isAnimation;

			if (isAnim)
			{
				d3d11.pD3DDeviceContext->VSSetShader(pvsFBXAnimInstancing->GetShader(), nullptr, 0);
				d3d11.pD3DDeviceContext->VSSetConstantBuffers(1, 1, pcBufferInstance.GetAddressOf());
				d3d11.pD3DDeviceContext->IASetInputLayout(pInputLayoutAnimationInstance.Get());
			}
			else
			{
				d3d11.pD3DDeviceContext->VSSetShader(pvsFBXInstancing->GetShader(), nullptr, 0);
				d3d11.pD3DDeviceContext->VSSetConstantBuffers(1, 1, pcBufferInstance.GetAddressOf());
				d3d11.pD3DDeviceContext->IASetInputLayout(pInputLayoutStaticMeshInstance.Get());
			}

			RenderInstancing((*it).second, refCnt, isAnim);
		}
		else
		{
			MeshData* pMesh = (*it).second[0]->pMeshData;

			isAnim = pMesh->isAnimation;
			if (isAnim)
			{
				d3d11.pD3DDeviceContext->VSSetShader(pvsFBXAnimation->GetShader(), nullptr, 0);
				d3d11.pD3DDeviceContext->VSSetConstantBuffers(1, 1, pcBuffer1.GetAddressOf());
				d3d11.pD3DDeviceContext->IASetInputLayout(pInputLayoutAnimation.Get());
			}
			else
			{
				d3d11.pD3DDeviceContext->VSSetShader(pvsFBX->GetShader(), nullptr, 0);
				d3d11.pD3DDeviceContext->VSSetConstantBuffers(1, 1, pcBuffer1.GetAddressOf());
				d3d11.pD3DDeviceContext->IASetInputLayout(pInputLayoutStaticMesh.Get());
			}


			Render((*it).second[0], isAnim);
		}
	}

	//EffectManager::Instance().Draw();

	Sprite::DrawAll();

	d3d11.pD3DDeviceContext->VSSetShader(nullptr, nullptr, 0);
	d3d11.pD3DDeviceContext->PSSetShader(nullptr, nullptr, 0);

	d3d11.Present(1, 0);

	for (auto it = opaqueDrawList.begin(); it != opaqueDrawList.end(); ++it)
	{
		it->second.clear();
	}

	return true;
}

void DrawSystem::Render(GraphicsComponent* pGC, bool isAnim)
{
	// FBX Model��node�����擾
	size_t nodeCount = pGC->pMeshData->GetMeshCount();

	// �S�m�[�h��`��
	for (unsigned int j = 0; j<nodeCount; j++)
	{
		MESH mesh = pGC->pMeshData->GetMeshList()[j];

		XMMATRIX m, m1;
		XMVECTOR LightDir;
		XMVECTOR v;

		//�J�����ʒu
		m1 = XMLoadFloat4x4(&pGC->pGameObject->worldMat) * XMLoadFloat4x4(&Camera::Instance().view);
		m1 = XMMatrixInverse(nullptr, m1);
		v = XMVector4Transform(XMVectorSet(Camera::Instance().m_eye.x, Camera::Instance().m_eye.y, Camera::Instance().m_eye.z, 0), m1);
		XMFLOAT4 eyePos;
		XMStoreFloat4(&eyePos, v);

		// ���b�V����̃��[�J�����W�n��ł̕��s�����̕����x�N�g�����v�Z����
		m1 = XMMatrixInverse(nullptr, XMLoadFloat4x4(&pGC->pGameObject->worldMat));
		v = XMVector4Transform(XMLoadFloat4(&lightDir), m1);
		v = XMVector3Normalize(v);
		XMFLOAT4 localLightPos;
		XMStoreFloat4(&localLightPos, v);

		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(d3d11.pD3DDeviceContext->Map(pcBuffer0.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			SHADER_GLOBAL sg;
			sg.lightDir = localLightPos;
			sg.eye = eyePos;
			memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(SHADER_GLOBAL));
			d3d11.pD3DDeviceContext->Unmap(pcBuffer0.Get(), 0);
		}

		d3d11.pD3DDeviceContext->VSSetConstantBuffers(0, 1, pcBuffer0.GetAddressOf());
		d3d11.pD3DDeviceContext->PSSetConstantBuffers(0, 1, pcBuffer0.GetAddressOf());


		D3D11_MAPPED_SUBRESOURCE MappedResource;
		d3d11.pD3DDeviceContext->Map(pcBuffer1.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

		CBMATRIX*	cbFBX = (CBMATRIX*)MappedResource.pData;

		XMMATRIX _world = XMLoadFloat4x4(&pGC->pGameObject->worldMat);
		XMMATRIX _view = XMLoadFloat4x4(&Camera::Instance().view);
		XMMATRIX _proj = XMLoadFloat4x4(&Camera::Instance().proj);
		XMMATRIX _local = XMLoadFloat4x4(&mesh.mLocal);

		// ����n
		cbFBX->mWorld = pGC->pGameObject->worldMat;
		cbFBX->mView = Camera::Instance().view;
		cbFBX->mProj = Camera::Instance().proj;

		XMStoreFloat4x4(&cbFBX->mWVP, XMMatrixTranspose(_local*_world*_view*_proj));

		d3d11.pD3DDeviceContext->Unmap(pcBuffer1.Get(), 0);

		pGC->pMeshData->SetAnimationFrame(j, pGC->frame);

		//�t���[����i�߂����Ƃɂ��ω������|�[�Y�i�{�[���̍s��j���V�F�[�_�[�ɓn��
		if (SUCCEEDED(d3d11.pD3DDeviceContext->Map(pcBoneBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			SHADER_GLOBAL_BONES sg;
			for (unsigned int i = 0; i < mesh.boneArray.size(); i++)
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

		if (isAnim)
		{
			ID3D11Buffer* pBuf[2] = { mesh.pVB.Get(), mesh.pVB_Bone.Get() };
			UINT stride[2] = { sizeof(VERTEX_DATA), sizeof(BONE_DATA_PER_VERTEX) };
			UINT offset[2] = { 0, 0 };
			// 3D�A�Z�b�g�f�[�^�A����уW�I���g�������p�̍s�����̓A�Z���u���ɐݒ肷��
			d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 2, pBuf, stride, offset);
		}
		else
		{
			UINT stride = sizeof(VERTEX_DATA);
			UINT offset = 0;
			d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 1, mesh.pVB.GetAddressOf(), &stride, &offset);
		}

		//�}�e���A���̐������A���ꂼ��̃}�e���A���̃C���f�b�N�X�o�b�t�@�|��`��
		for (unsigned int i = 0; i<mesh.materialData.size(); i++)
		{
			MaterialData material = *mesh.materialData[i];

			//�g�p����Ă��Ȃ��}�e���A���΍�
			if (material.faceCount == 0)
			{
				continue;
			}
			//�C���f�b�N�X�o�b�t�@�[���Z�b�g
			d3d11.pD3DDeviceContext->IASetIndexBuffer(mesh.pIB[i].Get(), DXGI_FORMAT_R32_UINT, 0);

			d3d11.pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			if (material.pMaterialCb)
				d3d11.pD3DDeviceContext->UpdateSubresource(material.pMaterialCb.Get(), 0, nullptr, &material.materialConstantData, 0, 0);

			d3d11.pD3DDeviceContext->VSSetConstantBuffers(3, 1, material.pMaterialCb.GetAddressOf());
			d3d11.pD3DDeviceContext->PSSetConstantBuffers(3, 1, material.pMaterialCb.GetAddressOf());

			if (material.pSRV != nullptr)
			{
				d3d11.pD3DDeviceContext->PSSetShaderResources(0, 1, material.pSRV.GetAddressOf());
				d3d11.pD3DDeviceContext->PSSetSamplers(0, 1, material.pSampler.GetAddressOf());

				d3d11.pD3DDeviceContext->PSSetShader(ppsTex->GetShader(), nullptr, 0);
			}
			else
			{
				d3d11.pD3DDeviceContext->PSSetShader(ppsVertexColor->GetShader(), nullptr, 0);
			}

			//Draw
			d3d11.pD3DDeviceContext->DrawIndexed(material.faceCount * 3, 0, 0);
		}
		//}
	}

}

void DrawSystem::RenderInstancing(vector<GraphicsComponent*>& pGClist, int refCnt, bool isAnim)
{
	// FBX Model��node�����擾
	size_t nodeCount = pGClist[0]->pMeshData->GetMeshCount();

	// �S�m�[�h��`��
	for (unsigned int j = 0; j<nodeCount; j++)
	{
		MESH mesh = pGClist[0]->pMeshData->GetMeshList()[j];

		if (mesh.pVB == nullptr)
			continue;

		D3D11_MAPPED_SUBRESOURCE MappedResource;
		d3d11.pD3DDeviceContext->Map(pcBufferInstance.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

		CBMATRIX_INSTANCING*	cbFBX = (CBMATRIX_INSTANCING*)MappedResource.pData;

		XMMATRIX _view = XMLoadFloat4x4(&Camera::Instance().view);
		XMMATRIX _proj = XMLoadFloat4x4(&Camera::Instance().proj);
		XMMATRIX _local = XMLoadFloat4x4(&mesh.mLocal);

		// ����n
		cbFBX->mView = Camera::Instance().view;
		cbFBX->mProj = Camera::Instance().proj;
		cbFBX->mLocal = mesh.mLocal;

		d3d11.pD3DDeviceContext->Unmap(pcBufferInstance.Get(), 0);

		pGClist[0]->pMeshData->SetAnimationFrame(j, pGClist[0]->frame);

		SetMatrix(pGClist);

		//�t���[����i�߂����Ƃɂ��ω������|�[�Y�i�{�[���̍s��j���V�F�[�_�[�ɓn��
		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(d3d11.pD3DDeviceContext->Map(pcBoneBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			SHADER_GLOBAL_BONES sg;
			for (unsigned int i = 0; i < mesh.boneArray.size(); i++)
			{
				XMMATRIX local = XMLoadFloat4x4(&mesh.mLocal);
				XMMATRIX mat = local * XMLoadFloat4x4(&/*node->mLocal**/pGClist[0]->pMeshData->GetCurrentPose(&mesh, i));
				mat = XMMatrixTranspose(mat);
				XMStoreFloat4x4(&sg.mBone[i], mat);
			}
			memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(SHADER_GLOBAL_BONES));
			d3d11.pD3DDeviceContext->Unmap(pcBoneBuffer.Get(), 0);
		}

		d3d11.pD3DDeviceContext->VSSetConstantBuffers(2, 1, pcBoneBuffer.GetAddressOf());
		d3d11.pD3DDeviceContext->PSSetConstantBuffers(2, 1, pcBoneBuffer.GetAddressOf());

		if (isAnim)
		{
			ID3D11Buffer* pBuf[2] = { mesh.pVB.Get(), mesh.pVB_Bone.Get() };
			UINT stride[2] = { sizeof(VERTEX_DATA), sizeof(BONE_DATA_PER_VERTEX) };
			UINT offset[2] = { 0, 0 };
			// 3D�A�Z�b�g�f�[�^�A����уW�I���g�������p�̍s�����̓A�Z���u���ɐݒ肷��
			d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 2, pBuf, stride, offset);
		}
		else
		{
			UINT stride = sizeof(VERTEX_DATA);
			UINT offset = 0;
			d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 1, mesh.pVB.GetAddressOf(), &stride, &offset);
		}

		d3d11.pD3DDeviceContext->VSSetShaderResources(0, 1, pTransformSRV.GetAddressOf());

		//�}�e���A���̐������A���ꂼ��̃}�e���A���̃C���f�b�N�X�o�b�t�@�|��`��
		for (unsigned int i = 0; i<mesh.materialData.size(); i++)
		{
			MaterialData material = *mesh.materialData[i];

			//�g�p����Ă��Ȃ��}�e���A���΍�
			if (material.faceCount == 0)
			{
				continue;
			}
			//�C���f�b�N�X�o�b�t�@�[���Z�b�g
			UINT stride = sizeof(int);
			UINT offset = 0;
			d3d11.pD3DDeviceContext->IASetIndexBuffer(mesh.pIB[i].Get(), DXGI_FORMAT_R32_UINT, 0);

			d3d11.pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			if (material.pMaterialCb)
				d3d11.pD3DDeviceContext->UpdateSubresource(material.pMaterialCb.Get(), 0, nullptr, &material.materialConstantData, 0, 0);

			d3d11.pD3DDeviceContext->VSSetConstantBuffers(3, 1, material.pMaterialCb.GetAddressOf());
			d3d11.pD3DDeviceContext->PSSetConstantBuffers(3, 1, material.pMaterialCb.GetAddressOf());

			if (material.pSRV != nullptr)
			{
				d3d11.pD3DDeviceContext->PSSetShaderResources(0, 1, material.pSRV.GetAddressOf());
				d3d11.pD3DDeviceContext->PSSetSamplers(0, 1, material.pSampler.GetAddressOf());

				d3d11.pD3DDeviceContext->PSSetShader(ppsTex->GetShader(), nullptr, 0);
			}
			else
			{
				d3d11.pD3DDeviceContext->PSSetShader(ppsVertexColor->GetShader(), nullptr, 0);
			}

			//Draw
			d3d11.pD3DDeviceContext->DrawIndexedInstanced(material.faceCount * 3, refCnt, 0, 0, 0);
		}
		//}
	}
}

void DrawSystem::AddDrawList(DRAW_PRIOLITY priolity, const string& tag, GraphicsComponent* pGC)
{
	if (priolity == DRAW_PRIOLITY::Opaque)
	{
		unordered_map<string, vector<GraphicsComponent*>>::iterator modelName = opaqueDrawList.find(tag);

		if (modelName == opaqueDrawList.end())
		{
			opaqueDrawList[tag].push_back(pGC);
		}
		else
		{
			vector<GraphicsComponent*>::iterator it = find(modelName->second.begin(), modelName->second.end(), pGC);

			if (it == modelName->second.end())
			{
				modelName->second.push_back(pGC);
			}
		}
	}
	else if (priolity == DRAW_PRIOLITY::Alpha)
	{
		unordered_map<string, vector<GraphicsComponent*>>::iterator modelName = alphaDrawList.find(tag);

		if (modelName == alphaDrawList.end())
		{
			alphaDrawList[tag].push_back(pGC);
		}
		else
		{
			vector<GraphicsComponent*>::iterator it = find(modelName->second.begin(), modelName->second.end(), pGC);

			if (it == modelName->second.end())
			{
				modelName->second.push_back(pGC);
			}
		}
	}
}
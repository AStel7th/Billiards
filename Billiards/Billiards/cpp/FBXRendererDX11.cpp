//#include "../Header/FBXRendererDX11.h"
//#include "../Header/FBXLoader.h"
//#include "../Header/DrawSystem.h"
//#include < locale.h >
//
//FBXRenderDX11::FBXRenderDX11()
//{
//	m_pFBX = nullptr;
//	m_meshNodeArray.clear();
//}
//
//FBXRenderDX11::~FBXRenderDX11()
//{
//	Release();
//}
//
//void FBXRenderDX11::Release()
//{
//	m_meshNodeArray.clear();
//
//	/*if (m_pFBX)
//	{
//	delete m_pFBX;
//	m_pFBX = nullptr;
//	}*/
//}
//
//HRESULT FBXRenderDX11::LoadFBX(const char* filename)
//{
//	if (!filename)
//		return E_FAIL;
//
//	HRESULT hr = S_OK;
//
//	//m_pFBX = make_unique<FBXLoader>();
//	m_pFBX = NEW FBXLoader();
//	hr = m_pFBX->LoadFBX(filename);
//	if (FAILED(hr))
//		return hr;
//
//	hr = CreateNodes();
//	if (FAILED(hr))
//		return hr;
//
//	return hr;
//}
//
//HRESULT FBXRenderDX11::LoadFBX(FBXLoader* loader)
//{
//	if (!loader)
//		return E_FAIL;
//
//	HRESULT hr = S_OK;
//
//	//m_pFBX = make_unique<FBXLoader>();
//	m_pFBX = loader;
//
//	hr = CreateNodes();
//	if (FAILED(hr))
//		return hr;
//
//	return hr;
//}
//
////
//HRESULT FBXRenderDX11::CreateNodes()
//{
//	HRESULT hr = S_OK;
//
//	size_t nodeCoount = m_pFBX->GetNodesCount();
//	if (nodeCoount == 0)
//		return E_FAIL;
//
//	m_meshNodeArray.resize(nodeCoount);
//
//	for (size_t i = 0; i<nodeCoount; i++)
//	{
//		MESH_NODE meshNode;
//		FBX_MESH_NODE fbxNode = m_pFBX->GetNode(static_cast<unsigned int>(i));
//
//		vector<VERTEX_DATA>	pV;
//		if (fbxNode.vertexCount != 0)
//			fbxNode.vertexCount < fbxNode.UVCount ? pV.resize(fbxNode.UVCount) : pV.resize(fbxNode.vertexCount);
//
//		//pV.resize(fbxNode.m_vertexDataArray.size());
//
//		VertexConstruction(fbxNode, meshNode, pV);
//
//		// index buffer
//		/*meshNode.indexCount = static_cast<DWORD>(fbxNode.m_materialArray[i].indexCount);
//		meshNode.SetIndexBit(meshNode.indexCount);*/
//
//		memcpy(meshNode.mat4x4, fbxNode.mat4x4, sizeof(float) * 16);
//
//		// �{�[��
//		BoneConstruction(fbxNode, meshNode, pV);
//
//		// �}�e���A��
//		MaterialConstruction(fbxNode, meshNode);
//
//		if (fbxNode.vertexCount != 0)
//			CreateVertexBuffer(&meshNode.m_pVB, pV.data(), sizeof(VERTEX_DATA), pV.size());
//
//		pV.clear();
//
//		m_meshNodeArray[i] = meshNode;
//	}
//
//
//	return hr;
//}
//
//HRESULT FBXRenderDX11::CreateVertexBuffer(ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount)
//{
//	if (stride == 0 || vertexCount == 0)
//		return E_FAIL;
//
//	HRESULT hr = S_OK;
//
//	D3D11_BUFFER_DESC bd;
//	ZeroMemory(&bd, sizeof(bd));
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = stride * vertexCount;
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//	D3D11_SUBRESOURCE_DATA InitData;
//	ZeroMemory(&InitData, sizeof(InitData));
//
//	InitData.pSysMem = pVertices;
//
//	hr = d3d11.pD3DDevice->CreateBuffer(&bd, &InitData, pBuffer);
//	if (FAILED(hr))
//		return hr;
//
//	return hr;
//}
//
//HRESULT FBXRenderDX11::CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer)
//{
//	D3D11_BUFFER_DESC bd;
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = dwSize;
//	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//	bd.MiscFlags = 0;
//	D3D11_SUBRESOURCE_DATA InitData;
//	InitData.pSysMem = pIndex;
//	InitData.SysMemPitch = 0;
//	InitData.SysMemSlicePitch = 0;
//	if (FAILED(d3d11.pD3DDevice->CreateBuffer(&bd, &InitData, ppIndexBuffer)))
//	{
//		return FALSE;
//	}
//
//	return S_OK;
//}
//
//HRESULT FBXRenderDX11::VertexConstruction(FBX_MESH_NODE &fbxNode, MESH_NODE& meshNode, vector<VERTEX_DATA>& vData)
//{
//	meshNode.vertexCount = static_cast<DWORD>(fbxNode.vertexCount);
//	if (meshNode.vertexCount == 0)
//		return E_FAIL;
//
//	meshNode.uvCount = static_cast<DWORD>(fbxNode.UVCount);
//
//	HRESULT hr = S_OK;
//
//	//	const size_t numUVSet = fbxNode.m_texcoordArray.size();
//
//	for (int i = 0; i < fbxNode.m_vertexDataArray.size(); i++)
//	{
//		FBX_VERTEX_DATA data = fbxNode.m_vertexDataArray[i];
//		vData[i].vPos = DirectX::XMFLOAT3((float)data.pos.mData[0],
//			(float)data.pos.mData[1],
//			(float)data.pos.mData[2]);
//
//		vData[i].vNor = DirectX::XMFLOAT3((float)data.nor.mData[0],
//			(float)data.nor.mData[1],
//			(float)data.nor.mData[2]);
//
//		if ((float)fbxNode.UVCount > 0)
//		{
//			// �����UV1�������Ȃ�
//			// UV��V�l���]
//			vData[i].vTexcoord = DirectX::XMFLOAT2((float)/*abs(1.0f - */data.uv.mData[0]/*)*/,
//				(float)/*abs(*/1.0f - data.uv.mData[1])/*)*/;
//		}
//		else
//			vData[i].vTexcoord = DirectX::XMFLOAT2(0, 0);
//	}
//
//	return hr;
//}
//
////
//HRESULT FBXRenderDX11::MaterialConstruction(FBX_MESH_NODE &fbxNode, MESH_NODE& meshNode)
//{
//	if (fbxNode.m_materialArray.size() == 0)
//		return E_FAIL;
//
//	HRESULT hr = S_OK;
//
//	int mCount = fbxNode.m_materialArray.size();
//	meshNode.materialCount = mCount;
//	meshNode.materialData.resize(mCount);
//
//	for (int i = 0; i < mCount; i++)
//	{
//		FBX_MATERIAL_NODE fbxMaterial = fbxNode.m_materialArray[i];
//		meshNode.materialData[i].specularPower = fbxMaterial.shininess;
//		meshNode.materialData[i].TransparencyFactor = fbxMaterial.TransparencyFactor;
//
//		meshNode.materialData[i].ambient
//			= DirectX::XMFLOAT4(fbxMaterial.ambient.r, fbxMaterial.ambient.g, fbxMaterial.ambient.b, fbxMaterial.ambient.a);
//		meshNode.materialData[i].diffuse
//			= DirectX::XMFLOAT4(fbxMaterial.diffuse.r, fbxMaterial.diffuse.g, fbxMaterial.diffuse.b, fbxMaterial.diffuse.a);
//		meshNode.materialData[i].specular
//			= DirectX::XMFLOAT4(fbxMaterial.specular.r, fbxMaterial.specular.g, fbxMaterial.specular.b, fbxMaterial.specular.a);
//		meshNode.materialData[i].emmisive
//			= DirectX::XMFLOAT4(fbxMaterial.emmisive.r, fbxMaterial.emmisive.g, fbxMaterial.emmisive.b, fbxMaterial.emmisive.a);
//
//
//		// Diffuse��������e�N�X�`����ǂݍ���
//		if (fbxMaterial.diffuse.textureSetArray.size()>0)
//		{
//			TextureSet::const_iterator it = fbxMaterial.diffuse.textureSetArray.begin();
//			if (it->second.size())
//			{
//				std::string path = it->second[0];
//				std::string ext = GetExtension(path);
//
//				WCHAR	wstr[512];
//				size_t wLen = 0;
//				mbstowcs_s(&wLen, wstr, path.size() + 1, path.c_str(), _TRUNCATE);
//
//				TexMetadata metadata;
//				ScratchImage image;
//
//				//Resource����̃p�X��ǉ�
//				TCHAR res[512] = L"Resource/";
//				wcscat_s(res, wstr);
//
//				if (ext == "tga")
//				{
//					hr = LoadFromTGAFile(res, &metadata, image);
//					if (FAILED(hr)) {
//						return hr;
//					}
//				}
//				else
//				{
//					hr = LoadFromWICFile(res, 0, &metadata, image);
//					if (FAILED(hr)) {
//						return hr;
//					}
//				}
//
//
//				// �V�F�[�_�[���\�[�X�r���[�̍쐬
//				hr = CreateShaderResourceView(d3d11.pD3DDevice.Get(), image.GetImages(), image.GetImageCount(), metadata, &meshNode.materialData[i].pSRV);
//			}
//		}
//
//		ComPtr<ID3D11Buffer> indexBuffer;
//		if (fbxMaterial.indexCount>0)
//			CreateIndexBuffer(fbxMaterial.indexCount * sizeof(int), fbxMaterial.pIndex.data(), &indexBuffer);
//
//		meshNode.m_pIB.push_back(indexBuffer);
//
//		meshNode.materialData[i].faceCount = fbxMaterial.indexCount / 3;//���̃}�e���A�����̃|���S����			
//
//																		// samplerstate
//		D3D11_SAMPLER_DESC sampDesc;
//		ZeroMemory(&sampDesc, sizeof(sampDesc));
//		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//		sampDesc.MinLOD = 0;
//		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
//		hr = d3d11.pD3DDevice->CreateSamplerState(&sampDesc, &meshNode.materialData[i].pSampler);
//
//		// material Constant Buffer
//		D3D11_BUFFER_DESC bufDesc;
//		ZeroMemory(&bufDesc, sizeof(bufDesc));
//		bufDesc.ByteWidth = sizeof(MATERIAL_CONSTANT_DATA);
//		bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//		bufDesc.CPUAccessFlags = 0;
//
//		hr = d3d11.pD3DDevice->CreateBuffer(&bufDesc, NULL, &meshNode.materialData[i].pMaterialCb);
//
//		meshNode.materialData[i].materialConstantData.ambient = meshNode.materialData[i].ambient;
//		meshNode.materialData[i].materialConstantData.diffuse = meshNode.materialData[i].diffuse;
//		meshNode.materialData[i].materialConstantData.specular = meshNode.materialData[i].specular;
//		meshNode.materialData[i].materialConstantData.emmisive = meshNode.materialData[i].emmisive;
//	}
//
//	return hr;
//}
//
//HRESULT FBXRenderDX11::BoneConstruction(FBX_MESH_NODE &fbxNode, MESH_NODE& meshNode, vector<VERTEX_DATA>& vData)
//{
//	//FBX���璊�o���ׂ����́A���_���Ƃ̃{�[���C���f�b�N�X�A���_���Ƃ̃{�[���E�F�C�g�A�o�C���h�s��A�|�[�Y�s��@��4����
//
//	int i, k;
//	int iNumBone = 0;//�{�[����
//
//	iNumBone = fbxNode.m_boneArray.size();
//	if (iNumBone == 0)
//		return E_FAIL;
//
//	//�ʏ�̏ꍇ�@�i���_����=UV���@pvVB�����_�C���f�b�N�X�x�[�X�̏ꍇ�j
//	if (fbxNode.vertexCount >= fbxNode.UVCount)
//	{
//		//���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�@��������t�ɁA���_�x�[�X�Ń{�[���C���f�b�N�X�E�d�݂𐮓ڂ���
//		for (i = 0; i<iNumBone; i++)
//		{
//			int iNumIndex = fbxNode.m_boneArray[i]->GetControlPointIndicesCount();//���̃{�[���ɉe�����󂯂钸�_��
//			int* piIndex = fbxNode.m_boneArray[i]->GetControlPointIndices();
//			double* pdWeight = fbxNode.m_boneArray[i]->GetControlPointWeights();
//			//���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������
//			for (k = 0; k<iNumIndex; k++)
//			{
//				for (int m = 0; m<4; m++)//FBX��CG�\�t�g���{�[��4�{�ȓ��Ƃ͌���Ȃ��B5�{�ȏ�̏ꍇ�́A�d�݂̑傫������4�{�ɍi�� 
//				{
//					if (pdWeight[k] > vData[piIndex[k]].boneWeight[m])
//					{
//						vData[piIndex[k]].boneIndex[m] = i;
//						vData[piIndex[k]].boneWeight[m] = pdWeight[k];
//						break;
//					}
//				}
//			}
//		}
//	}
//	//UV�C���f�b�N�X�x�[�X�̏ꍇ�@�i���_��<UV���j
//	else
//	{
//		int PolyIndex = 0;
//		int UVIndex = 0;
//		//���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�@��������t�ɁA���_�x�[�X�Ń{�[���C���f�b�N�X�E�d�݂𐮓ڂ���
//		for (i = 0; i<iNumBone; i++)
//		{
//			int iNumIndex = fbxNode.m_boneArray[i]->GetControlPointIndicesCount();//���̃{�[���ɉe�����󂯂钸�_��
//			int* piIndex = fbxNode.m_boneArray[i]->GetControlPointIndices();
//			double* pdWeight = fbxNode.m_boneArray[i]->GetControlPointWeights();
//			//���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������
//			for (k = 0; k<iNumIndex; k++)
//			{
//				//���̒��_���܂�ł���|���S�����ׂĂɁA���̃{�[���ƃE�F�C�g��K�p
//				for (int p = 0; p<fbxNode.polyTable[piIndex[k]].NumRef; p++)
//				{
//					//���_�������|���S�������̃|���S����UV�C���f�b�N�X�@�Ƌt����
//					PolyIndex = fbxNode.polyTable[piIndex[k]].PolyIndex[p];
//					UVIndex = fbxNode.m_pMesh->GetTextureUVIndex(PolyIndex, fbxNode.polyTable[piIndex[k]].Index123[p], FbxLayerElement::eTextureDiffuse);
//
//					for (int m = 0; m<4; m++)//FBX��CG�\�t�g���{�[��4�{�ȓ��Ƃ͌���Ȃ��B5�{�ȏ�̏ꍇ�́A�d�݂̑傫������4�{�ɍi�� 
//					{
//						if (pdWeight[k] > vData[UVIndex].boneWeight[m])
//						{
//							vData[UVIndex].boneIndex[m] = i;
//							vData[UVIndex].boneWeight[m] = pdWeight[k];
//							break;
//						}
//					}
//				}
//			}
//		}
//	}
//
//	XMMATRIX m(-1, 0, 0, 0,
//		0, 1, 0, 0,
//		0, 0, 1, 0,
//		0, 0, 0, 1);
//
//	//
//	//�{�[���𐶐�
//	meshNode.boneCount = iNumBone;
//	meshNode.boneArray.resize(iNumBone);
//
//	for (int i = 0; i<meshNode.boneCount; i++)
//	{
//		FbxAMatrix mat;
//		fbxNode.m_boneArray[i]->GetTransformLinkMatrix(mat);
//		int a = fbxNode.m_boneArray[i]->GetControlPointIndicesCount();
//		FbxMatrix initMat = FbxMatrix(mat);
//
//		for (int x = 0; x<4; x++)
//		{
//			for (int y = 0; y<4; y++)
//			{
//				meshNode.boneArray[i].mBindPose.m[y][x] = initMat.Get(y, x);
//			}
//		}
//
//		/*XMMATRIX bindBone = XMLoadFloat4x4(&meshNode.boneArray[i].mBindPose);
//		XMStoreFloat4x4(&meshNode.boneArray[i].mBindPose, XMMatrixMultiply(bindBone, m));*/
//	}
//
//	//SetNewPoseMatrix(fbxNode, &meshNode);
//	//delete
//
//	return S_OK;
//}
//
////
////HRESULT FBXRenderDX11::CreateInputLayout(ID3D11Device*	pd3dDevice, const void* pShaderBytecodeWithInputSignature, size_t BytecodeLength, D3D11_INPUT_ELEMENT_DESC* pLayout, unsigned int layoutSize)
////{
////	// InputeLayout�͒��_�V�F�[�_�̃R���p�C�����ʂ��K�v
////	if(!pd3dDevice || !pShaderBytecodeWithInputSignature || !pLayout)
////		return E_FAIL;
////
////	HRESULT hr = S_OK;
////
////	for (auto meshNode = m_meshNodeArray.begin(); meshNode != m_meshNodeArray.end();++meshNode)
////	{
////		hr = pd3dDevice->CreateInputLayout(pLayout, layoutSize, pShaderBytecodeWithInputSignature, BytecodeLength, &meshNode->m_pInputLayout);
////	}
////
////	return hr;
////}
//
//HRESULT FBXRenderDX11::RenderAll()
//{
//	size_t nodeCount = m_meshNodeArray.size();
//	if (nodeCount == 0)
//		return S_OK;
//
//	HRESULT hr = S_OK;
//
//	d3d11.pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	for (auto meshNode = m_meshNodeArray.begin(); meshNode != m_meshNodeArray.end(); ++meshNode)
//	{
//		if (meshNode->vertexCount == 0)
//			continue;
//
//		UINT stride = sizeof(VERTEX_DATA);
//		UINT offset = 0;
//		d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 1, meshNode->m_pVB.GetAddressOf(), &stride, &offset);
//
//		DXGI_FORMAT indexbit = DXGI_FORMAT_R16_UINT;
//		if (meshNode->m_indexBit == MESH_NODE::INDEX_32BIT)
//			indexbit = DXGI_FORMAT_R32_UINT;
//
//		//pImmediateContext->IASetInputLayout(meshNode->m_pInputLayout);
//		d3d11.pD3DDeviceContext->IASetIndexBuffer(meshNode->m_pIB[0].Get(), indexbit, 0);
//
//		d3d11.pD3DDeviceContext->DrawIndexed(meshNode->indexCount, 0, 0);
//	}
//
//	return hr;
//}
//
//HRESULT FBXRenderDX11::RenderNode(const size_t nodeId)
//{
//	ID3D11Buffer* boneBuf = DrawSystem::Instance().GetCBuffer(2);
//
//	size_t nodeCount = m_meshNodeArray.size();
//	if (nodeCount == 0 || nodeCount <= nodeId)
//		return S_OK;
//
//	HRESULT hr = S_OK;
//
//	MESH_NODE* node = &m_meshNodeArray[nodeId];
//
//	if (node->vertexCount == 0)
//		return S_OK;
//
//	//�t���[����i�߂����Ƃɂ��ω������|�[�Y�i�{�[���̍s��j���V�F�[�_�[�ɓn��
//	D3D11_MAPPED_SUBRESOURCE pData;
//	if (SUCCEEDED(d3d11.pD3DDeviceContext->Map(boneBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
//	{
//		SHADER_GLOBAL_BONES sg;
//		for (int i = 0; i < node->boneCount; i++)
//		{
//			XMMATRIX local = XMLoadFloat4x4(&node->mLocal);
//			XMMATRIX mat = local * XMLoadFloat4x4(&/*node->mLocal**/GetCurrentPoseMatrix(node, i));
//			mat = XMMatrixTranspose(mat);
//			XMStoreFloat4x4(&sg.mBone[i], mat);
//		}
//		memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(SHADER_GLOBAL_BONES));
//		d3d11.pD3DDeviceContext->Unmap(boneBuf, 0);
//	}
//
//	d3d11.pD3DDeviceContext->VSSetConstantBuffers(2, 1, &boneBuf);
//	d3d11.pD3DDeviceContext->PSSetConstantBuffers(2, 1, &boneBuf);
//
//	UINT stride = sizeof(VERTEX_DATA);
//	UINT offset = 0;
//	d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 1, node->m_pVB.GetAddressOf(), &stride, &offset);
//
//	//// �C���f�b�N�X�o�b�t�@�����݂���ꍇ
//	//if(node->m_indexBit!=MESH_NODE::INDEX_NOINDEX)
//	//{
//	//	DXGI_FORMAT indexbit = DXGI_FORMAT_R16_UINT;
//	//	if(node->m_indexBit==MESH_NODE::INDEX_32BIT)
//	//		indexbit = DXGI_FORMAT_R32_UINT;
//	//	
//	//	
//
//	//�}�e���A���̐������A���ꂼ��̃}�e���A���̃C���f�b�N�X�o�b�t�@�|��`��
//	for (int i = 0; i<node->materialCount; i++)
//	{
//		MATERIAL_DATA material = GetNodeMaterial(nodeId, i);
//
//		//�g�p����Ă��Ȃ��}�e���A���΍�
//		if (material.faceCount == 0)
//		{
//			continue;
//		}
//		//�C���f�b�N�X�o�b�t�@�[���Z�b�g
//		stride = sizeof(int);
//		offset = 0;
//		d3d11.pD3DDeviceContext->IASetIndexBuffer(node->m_pIB[i].Get(), DXGI_FORMAT_R32_UINT, 0);
//
//		d3d11.pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//		if (material.pMaterialCb)
//			d3d11.pD3DDeviceContext->UpdateSubresource(material.pMaterialCb.Get(), 0, NULL, &material.materialConstantData, 0, 0);
//
//		d3d11.pD3DDeviceContext->VSSetConstantBuffers(3, 1, material.pMaterialCb.GetAddressOf());
//		d3d11.pD3DDeviceContext->PSSetConstantBuffers(3, 1, material.pMaterialCb.GetAddressOf());
//
//		if (material.pSRV != nullptr)
//		{
//			d3d11.pD3DDeviceContext->PSSetShaderResources(0, 1, material.pSRV.GetAddressOf());
//			d3d11.pD3DDeviceContext->PSSetSamplers(0, 1, material.pSampler.GetAddressOf());
//		}
//
//		//Draw
//		d3d11.pD3DDeviceContext->DrawIndexed(material.faceCount * 3, 0, 0);
//	}
//	//}
//
//	return hr;
//}
//
//HRESULT FBXRenderDX11::RenderNodeInstancing(const size_t nodeId, const uint32_t InstanceCount)
//{
//	size_t nodeCount = m_meshNodeArray.size();
//	if (nodeCount == 0 || nodeCount <= nodeId || InstanceCount == 0)
//		return S_OK;
//
//	HRESULT hr = S_OK;
//
//	MESH_NODE* node = &m_meshNodeArray[nodeId];
//
//	if (node->vertexCount == 0)
//		return S_OK;
//
//	UINT stride = sizeof(VERTEX_DATA);
//	UINT offset = 0;
//	d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 1, node->m_pVB.GetAddressOf(), &stride, &offset);
//	//pImmediateContext->IASetInputLayout(node->m_pInputLayout);
//	d3d11.pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	// �C���f�b�N�X�o�b�t�@�����݂���ꍇ
//	if (node->m_indexBit != MESH_NODE::INDEX_NOINDEX)
//	{
//		DXGI_FORMAT indexbit = DXGI_FORMAT_R16_UINT;
//		if (node->m_indexBit == MESH_NODE::INDEX_32BIT)
//			indexbit = DXGI_FORMAT_R32_UINT;
//
//		d3d11.pD3DDeviceContext->IASetIndexBuffer(node->m_pIB[0].Get(), indexbit, 0);
//
//		d3d11.pD3DDeviceContext->DrawIndexedInstanced(node->indexCount, InstanceCount, 0, 0, 0);
//	}
//
//	return hr;
//}
//
//HRESULT FBXRenderDX11::RenderNodeInstancingIndirect(const size_t nodeId, ID3D11Buffer* pBufferForArgs, const uint32_t AlignedByteOffsetForArgs)
//{
//	size_t nodeCount = m_meshNodeArray.size();
//	if (nodeCount == 0 || nodeCount <= nodeId)
//		return S_OK;
//
//	HRESULT hr = S_OK;
//
//	MESH_NODE* node = &m_meshNodeArray[nodeId];
//
//	if (node->vertexCount == 0)
//		return S_OK;
//
//	UINT stride = sizeof(VERTEX_DATA);
//	UINT offset = 0;
//	d3d11.pD3DDeviceContext->IASetVertexBuffers(0, 1, node->m_pVB.GetAddressOf(), &stride, &offset);
//	//pImmediateContext->IASetInputLayout(node->m_pInputLayout);
//	d3d11.pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	// �C���f�b�N�X�o�b�t�@�����݂���ꍇ
//	if (node->m_indexBit != MESH_NODE::INDEX_NOINDEX)
//	{
//		DXGI_FORMAT indexbit = DXGI_FORMAT_R16_UINT;
//		if (node->m_indexBit == MESH_NODE::INDEX_32BIT)
//			indexbit = DXGI_FORMAT_R32_UINT;
//
//		d3d11.pD3DDeviceContext->IASetIndexBuffer(node->m_pIB[0].Get(), indexbit, 0);
//
//		d3d11.pD3DDeviceContext->DrawIndexedInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
//	}
//
//	return hr;
//}
//
//void FBXRenderDX11::Draw(int frame, bool bInstance)
//{
//	ID3D11Buffer* buf = DrawSystem::Instance().GetCBuffer(1);
//
//	// FBX Model��node�����擾
//	size_t nodeCount = GetNodeCount();
//
//	// �S�m�[�h��`��
//	for (int j = 0; j<nodeCount; j++)
//	{
//
//		GetNodeMatrix(j, &m_meshNodeArray[j].mLocal.m[0][0]);	// ����node��Matrix
//
//		D3D11_MAPPED_SUBRESOURCE MappedResource;
//		d3d11.pD3DDeviceContext->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
//
//		CBMATRIX*	cbFBX = (CBMATRIX*)MappedResource.pData;
//
//		XMMATRIX world = XMLoadFloat4x4(&m_World);
//		XMMATRIX view = XMLoadFloat4x4(&m_View);
//		XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
//		XMMATRIX local = XMLoadFloat4x4(&m_meshNodeArray[j].mLocal);
//
//		// ����n
//		cbFBX->mWorld = (m_World);
//		cbFBX->mView = (m_View);
//		cbFBX->mProj = (m_Proj);
//
//		XMStoreFloat4x4(&cbFBX->mWVP, XMMatrixTranspose(/*local**/world*view*proj));
//
//		d3d11.pD3DDeviceContext->Unmap(buf, 0);
//
//		d3d11.pD3DDeviceContext->VSSetConstantBuffers(1, 1, &buf);
//
//		SetNewPoseMatrix(m_pFBX->GetNode(j), &GetNode(j), frame);
//
//		//MATERIAL_DATA material = GetNodeMaterial(j);
//
//		//if (material.pMaterialCb)
//		//	d3d11.pD3DDeviceContext->UpdateSubresource(material.pMaterialCb.Get(), 0, NULL, &material.materialConstantData, 0, 0);
//
//		////pDirect3D11->pD3DDeviceContext->VSSetShaderResources(0, 1, &g_pTransformSRV);
//		//d3d11.pD3DDeviceContext->PSSetShaderResources(0, 1, material.pSRV.GetAddressOf());
//		//d3d11.pD3DDeviceContext->PSSetConstantBuffers(0, 1, material.pMaterialCb.GetAddressOf());
//		//d3d11.pD3DDeviceContext->PSSetSamplers(0, 1, material.pSampler.GetAddressOf());
//
//		RenderNode(j);
//	}
//
//}
//
//void FBXRenderDX11::SetMatrix(XMFLOAT4X4& world, XMFLOAT4X4& view, XMFLOAT4X4& proj)
//{
//	m_World = world;
//	m_View = view;
//	m_Proj = proj;
//}
//
////���́i���݂́j�|�[�Y�s���Ԃ�
//XMFLOAT4X4 FBXRenderDX11::GetCurrentPoseMatrix(MESH_NODE* node, int index)
//{
//	XMFLOAT4X4 result;
//	XMMATRIX inv;
//	XMMATRIX bindPose = XMLoadFloat4x4(&node->boneArray[index].mBindPose);
//	XMMATRIX newPose = XMLoadFloat4x4(&node->boneArray[index].mNewPose);
//
//	inv = XMMatrixInverse(0, bindPose);//FBX�̃o�C���h�|�[�Y�͏����p���i��΍��W�j
//	XMStoreFloat4x4(&result, inv * newPose); //�o�C���h�|�[�Y�̋t�s��ƃt���[���p���s���������B�Ȃ��A�o�C���h�|�[�Y���̂����ɋt�s��ł���Ƃ���l��������B�iFBX�̏ꍇ�͈Ⴄ���j
//
//	return result;
//}
//
////�{�[�������̃|�[�Y�ʒu�ɃZ�b�g����
//void FBXRenderDX11::SetNewPoseMatrix(FBX_MESH_NODE &fbxNode, MESH_NODE* meshNode, int frame)
//{
//	XMMATRIX m(-1, 0, 0, 0,
//		0, 1, 0, 0,
//		0, 0, 1, 0,
//		0, 0, 0, 1);
//
//	int i;
//	FbxTime time;
//	time.SetTime(0, 0, 0, frame, 0, FbxTime::eFrames60);//30�t���[��/�b�@�Ɛ���@�����ɂ͏󋵂��Ƃɒ��ׂ�K�v����
//
//	for (int i = 0; i<meshNode->boneCount; i++)
//	{
//		FbxAMatrix mat = fbxNode.m_boneArray[i]->GetLink()->EvaluateGlobalTransform(time);
//
//		FbxMatrix newMat = FbxMatrix(mat);
//
//		for (int x = 0; x<4; x++)
//		{
//			for (int y = 0; y<4; y++)
//			{
//				meshNode->boneArray[i].mNewPose.m[y][x] = newMat.Get(y, x);
//			}
//		}
//
//		/*XMMATRIX newPose = XMLoadFloat4x4(&meshNode->boneArray[i].mNewPose);
//		XMStoreFloat4x4(&meshNode->boneArray[i].mNewPose, XMMatrixMultiply(newPose, m));*/
//	}
//}